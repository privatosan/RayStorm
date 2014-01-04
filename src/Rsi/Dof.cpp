/***************
* MODULE:		DoF.cpp
* DESCRIPTION:	DoF-Class. Implementation of the algorithm of my diploma:
*					"Effiziente Berechnung von Tiefenschärfe zur
*					realistischen Darstellung von Bildern"
*					(Efficient calculaton of depth of field to receive
*               realistic pictures)
* AUTHORS:		Mike Hesser
***************/

#include <stdio.h>
#include <time.h>
#include "DoF.h"
#include "math.h"
#include "context.h"

const float pi = 3.14159265358979f;
const float startRadius = 0.5642f;   // equivalent to 1/sqrt(pi)

#define MAX_RADIUS 30
#define MAX_LUT 600
#define DOF_HIDDENMARK (2*MAX_LUT + 1)


// error messages
char *aszErrors[] =
{
	"Ok",
	"Out of Memory",
	"Could not open file for write",
	"Could not write to file",
	"No picture data available",
	"The width or height of the picture are out of range",
	"The field of view is out of range",
	"The aperture number is out of range",
	"The focal distance is out of range",
	"The number of samples is out of range",
	"The render field is out of range",
	"Init() was not called or went fail",
	"Unbekannter Fehler (Fehlernummer ausserhalb des zulässigen Bereichs)",
};

/*************
* DESCRIPTION: initialize class variables
* INPUT:			-
* OUTPUT:		-
*************/
CDoF::CDoF()
{
	CoCMap = NULL;
	aSegment = NULL;
	aLUT = NULL;
	xres = yres = 0;
	left = top = right = bottom = 0;
	f = 1.f;
	n = 1.f;
	focaldist = 1.f;
	Cmax = 0.f;
	scale = 0.f;
	hfov = 25.f*pi/180.f;
	error = errOk;
	cntAnti = cntJump = 0;
	samples = 5;
	flags = DoFHidden|DoFAccelerate;
	bInit = false;
}

/*************
* DESCRIPTION: free allocated resources
* INPUT:			-
* OUTPUT:		-
*************/
CDoF::~CDoF()
{
	Cleanup();
}

/*************
* DESCRIPTION: allocate depth-buffer
* INPUT:			w, h	resolution in x- and y-direction
*					fd		distance to look point
* OUTPUT:		true -> ok, false -> error
*************/
bool CDoF::Init(DOF_DATA *pD)
{
	float radius, angle;
	int i, h;

	// free resources
	Cleanup();

	xres = pD->xres;
	yres = pD->yres;

	if (xres <= 0 || yres <= 0)
		return Error(errPicSize);

	left = pD->left;
	top = pD->top;
	right = pD->right;
	bottom = pD->bottom;

	if (left < 0  || left >= xres  ||
		 top < 0   || top >= yres   ||
		 right < 0 || right >= xres ||
		 bottom < 0 || bottom >= yres)
		 return Error(errRenderField);
	
	if (left > right)
	{
		h = left;
		left = right;
		right = h;
	}
	if (top > bottom)
	{
		h = top;
		top = bottom;
		bottom = h;
	}
	hfov = pD->hfov;
	vfov = pD->vfov;

	if (hfov <= 0 || vfov <= 0)
		return Error(errFieldOfView);

	focaldist = pD->focaldist;
	if (focaldist <= 1)
		return Error(errFocalDistance);

	if (pD->aperture <= 0.0f)
		return Error(errAperture);

	if (pD->samples < 0 || pD->samples > 10)
		return Error(errSamples);

	samples = pD->samples;
	samples2 = samples*samples;

	flags = pD->flags;

	aLUT = new DOF_LUT_ENTRY[MAX_LUT];
	if (!aLUT)
		return Error(errMemory);

	CoCMap = new DOF_PIXEL[xres*yres];
	if (!CoCMap)
		return Error(errMemory);

	aSegment = new DOF_SEGMENT[yres];
	if (!aSegment)
		return Error(errMemory);

	aAperture = new DOF_APERTUREPOINT[samples2];
	if (!aAperture)
		return Error(errMemory);

	// initialise depth map and segment table
	for (i = 0; i < xres*yres; i++)
		CoCMap[i] = 0;

	for (i = 0; i < yres; i++)
	{
		aSegment[i].toLine = aSegment[i].fromLine = 0;
		aSegment[i].pLine = NULL;
	}

	// calculate look-up-tables
	CalculateLUTs();

	// calculate some values (refer to diploma work)
	Db = (0.5f*xres)/(float)tan(0.5f*hfov);	// distance of image plane
	f = 1/(1/focaldist + 1/Db);					// the focal distance follows from the distance to the image plane and the distance of the look point
	n = f/pD->aperture;								// the aperture number results from the focal distance divided through the aperture
	Cmax = float(fabs(f - Db))/n;					// CoC-diameter at infinite distance
	Clr = 0.5f*pD->aperture;						// radius of the aperture

	// calculate spread-table for aperture
	for (i = 0; i < samples2; i++)
	{
		radius = (1 - (i/samples)*(1.0f/samples))*Clr;
		angle = (i*2.0f*pi)/samples + (5*i/samples)*(1.0f/samples)*samples;
		aAperture[i].x = float(cos(angle)*radius);
		aAperture[i].y = float(sin(angle)*radius);
	}

	bInit = true;

	return true;
}

/*************
* DESCRIPTION: free all allocated resources
* INPUT:			-
* OUTPUT:		-
*************/
void CDoF::Cleanup()
{
	int i;

	if (aLUT)
	{
		for (i = 0; i < MAX_LUT; i++)
		{
			if (aLUT[i].pData)
			{
				delete aLUT[i].pData;
				aLUT[i].pData = NULL;
			}
		}
		delete aLUT;
		aLUT = NULL;
	}

	if (CoCMap)
	{
		delete CoCMap;
		CoCMap = NULL;
	}

	if (aSegment)
	{
		for (i = 0; i < yres; i++)
			if (aSegment[i].pLine)
				delete aSegment[i].pLine;

		delete aSegment;
		aSegment = NULL;
	}

	bInit = false;
}

/*************
* DESCRIPTION:	calculate look-up-tables for the CoC-Radius till maxRadius
* INPUT:			-
* OUTPUT:		true -> ok, false ->error
*************/
bool CDoF::CalculateLUTs()
{
	float x, y, r, value, xmax, *pMatrix;
	int i, j, h, cellsize, x1, y1, x2, y2, pos;
	float buffer[10*MAX_RADIUS];

	pMatrix = new float[(MAX_RADIUS + 1)*(MAX_RADIUS + 1)];
	if (!pMatrix)
		return Error(errMemory);

	stepRadius = (MAX_RADIUS - startRadius)/MAX_LUT;

	r = startRadius;
	for (i = 0; i < MAX_LUT; i++)
	{
		cellsize = int(ceil(r + 0.5f));

		aLUT[i].size = cellsize;
		aLUT[i].radius = r;

		for (x1 = 0; x1 < cellsize; x1++)
		{
			for (y1 = cellsize - 1; y1 >= x1; y1--)
			{
				x = float(x1) + 0.5f;
				y = y1 - 0.5f;
				if (y < 0.0f)
					y = 0.0f;

				// integrate to xmax (point of intersection arc <-> y-axis)
				xmax = float(sqrt(r*r - y*y));
				if (xmax > x)
					xmax = x;

				// calculate area from 0 to xmax
				value = float(0.5f*(xmax*sqrt(r*r - xmax*xmax) + r*r*asin(xmax/r)) - y*xmax);

				// subtract the parts of the areas above and left of the current cell
				for (y2 = y1; y2 < cellsize; y2++)
				{
					for (x2 = 0; x2 <= x1; x2++)
					{
						if (x2 != x1 || y2 != y1)
							value -= pMatrix[x2 + y2*cellsize];
					}
				}

				// save the calculated areas (together with the mirrored data around the bisector of an angle)				pMatrix[x1 + y1*cellsize] = pMatrix[y1 + x1*cellsize] = value;
			}
		}
		
		// double the cell-areas at the x- and y-axis
		for (j = 0; j < cellsize; j++)
		{
			pMatrix[j] *= 2;
			pMatrix[j*cellsize] *= 2;
		}

		// encode the matrix into a byte-string
		pos = 0;
		for (y1 = 1; y1 <= cellsize; y1++)
		{
			j = y1*cellsize - 1;
			h = 0;
			while (pMatrix[j] == 0.0f)
			{
				h++;
				j--;
			}
			buffer[pos++] = float(h);
			pos++;
			h = 0;
			while (pMatrix[j] < 1.0f && j > (y1 - 1)*cellsize - 1)
			{
				buffer[pos++] = pMatrix[j]/(r*r*pi);
				j--;
				h++;
			}
			buffer[pos - h - 1] = float(h);
		}
		aLUT[i].pData = new float[pos];
		memcpy(aLUT[i].pData, buffer, sizeof(float)*pos);
		aLUT[i].intensity = 1.0f/(r*r*pi);

		r += stepRadius;
	}

	delete pMatrix;

	return true;
}

/*************
* DESCRIPTION: set depth information at point x,y
* INPUT:			x, y	position of the point
*					depth	the depth of the point
* OUTPUT:		-
*************/
void CDoF::SetDepth(int x, int y, float depth, DOF_ACTION *pDoFAction)
{
	float r, sign;
	short lut;

	if (!CoCMap)
		return;

	// correct the length of the ray
	depth = Db*depth/(float)sqrt(Db*Db + 
				(0.5f*xres - x)*(0.5f*xres - x) + 
				(0.5f*yres - y)*(0.5f*yres - y));

	// a depth value of 0 corresponds to an infinte depth
	if (depth <= 0.00001f)
		r = 0.5f*Cmax;
	else
		r = 0.5f*float((f - (Db - (f*Db)/depth))/n);

	if (r < 0.0f)
	{
		sign = -1.0f;
		r = -r;
	}
	else
		sign = 1.0f;

	if (r > MAX_RADIUS)
		r = MAX_RADIUS;

	lut = 0;
	if (r > startRadius)
		lut = short(sign*((r - startRadius - stepRadius)/stepRadius));

	CoCMap[x + y*xres] = lut;

	if (!pDoFAction || (flags & DoFAccelerate) == 0)
		return;

	pDoFAction->action = DoFNormal;

	if (cntAnti == 0)
	{
		if (r > 1.5f && cntJump >= 0)
		{
			pDoFAction->action = DoFJump;
			// number of pixels to jump over
			cntJump = int(0.5f*r + 0.5f);
			if (x + cntJump > right)
				cntJump = right - x;

			pDoFAction->count = cntJump;
			// save the missing depth values
			for (int i = 1; i <= cntJump; i++)
			{
				if (x + i <= right)
					CoCMap[x + i + y*xres] = lut;
			}
			return;
		}
		else
		{
			if (cntJump < 0)
				cntJump++;
			if (cntJump > 0)
			{
				pDoFAction->action = DoFJump;
				cntJump = -cntJump - 1;
				pDoFAction->count = cntJump;
				return;
			}
		}
	}

	// no antialias needed, if CoC is greate than the filter width	
	if (r > startRadius)
	{
		if (cntAnti >= int(startRadius + 0.5f))
			pDoFAction->action = DoFNoAntialias;
		else
			cntAnti++;
	}
	else
	{
		if (cntAnti >= int(startRadius + 0.5f))
		{
			pDoFAction->action = DoFSampleBack;
			pDoFAction->count = int(startRadius + 0.5f);
			cntAnti = 0;
		}
	}
}

/*************
* DESCRIPTION: mark pixels in the hidden zone
* INPUT:			-
* OUTPUT:		-
*************/
void CDoF::MarkHidden()
{
	int i, j, x, y, radius, pos, h, posxy;
	float z1, z2, sign;
	short l1, l2, l3;

	for (y = top; y <= bottom; y++)
	{
		for (x = left; x <= right; x++)
		{
			pos = x + y*xres;

			l2 = CoCMap[pos];
			if (l2 > MAX_LUT)
				l2 -= DOF_HIDDENMARK;
	
			if (y < bottom)
			{
				l3 = CoCMap[x + (y + 1)*xres];
				if (l3 > MAX_LUT)
					l3 -= DOF_HIDDENMARK;
			}
			else
				l3 = l2;

			if (x < right)
			{
				l1 = CoCMap[(x + 1) + y*xres];
				if (l1 > MAX_LUT)
					l1 -= DOF_HIDDENMARK;
			}
			else
				l1 = l2;

			if (abs(l2 - l3) > abs(l2 - l1))
				l2 = l3;

			if (l2 > l1)
			{
				h = l1;
				l1 = l2;
				l2 = h;
			}

			// calculate depth-values out of the CoC-Radius
			sign = l1 < 0 ? -1.f : 1.f;
			z1 = Db*f/(2.f*sign*aLUT[abs(l1)].radius*n - f + Db);

			sign = l2 < 0 ? -1.f : 1.f;
			z2 = Db*f/(2.f*sign*aLUT[abs(l2)].radius*n - f + Db);

			// calculate the diameter of the light cone at this position
			radius = abs(int((Clr*Db*(z1 - z2))/(z1*z2)));

			// does it worth the hidden calculation?
			// yes, get the hidden points
			if (abs(radius) >= 1)
			{
				for (j = -radius; j <= radius; j++)
				{
					if (y + j >= top && y + j <= bottom)
					{
						h = int(sqrt(radius*radius - j*j));

						for (i = -h; i <= h; i++)
						{
							posxy = pos + i + j*xres; 
							if (x + i >= left && x + i <= right)
							{
								if (CoCMap[posxy] <= MAX_LUT)
									CoCMap[posxy] += DOF_HIDDENMARK;
							}
						}
					}
				}
			}
		}
	}
}

/*************
* DESCRIPTION:	calculate depth of field
* INPUT:			-
* OUTPUT:		true -> ok, false ->error
*************/
bool CDoF::CalculateDoF(RSICONTEXT *rc, void (*SamplePixel)(RSICONTEXT*, int, int, float, float, SMALL_COLOR*))
{
	int i, h, x, y, xx, yy, lut, posxy;
	int dx, dy, xStep, yStep, xBrd, yBrd, ri, rMax;
	int fromLine, toLine;
	float red, green, blue, intensity;
	float *pMatrix;
	SMALL_COLOR *pLine, color;
	PIXF *ph, col, colfull;
	float percent, percent_per_row;
	clock_t starttime;

	if (!bInit)
		return Error(errNoInit);

	if (flags & DoFHidden)
		MarkHidden();

	pSrc = rc->scrarray;
	if (!pSrc)
		return Error(errNoPicAvailable);

	starttime = clock();
	percent_per_row = (float)1.0f/(bottom - top);
	pLine = rc->scrarray + top*xres;

	rc->UpdateStatus(rc, 0.0f, 0.0f, 0, 0, rc->scrarray);

	// get the toLine and fromLine-values
	for (y = top; y <= bottom; y++)
	{
		rMax = 0;
		for (x = left; x <= right; x++)
		{
			lut = CoCMap[x + y*xres];
			if (lut > MAX_LUT)
				lut -= DOF_HIDDENMARK;
			ri = (int)aLUT[abs(lut)].size;

			if (ri > rMax)
				rMax = ri;
		}
		aSegment[y].pLine = NULL;
		aSegment[y].fromLine = y - rMax;
		if (aSegment[y].fromLine < top)
			aSegment[y].fromLine = top;

		for (i = y; i >= y - rMax; i--)
			if (i >= top && aSegment[i].fromLine > y - rMax)
				aSegment[i].fromLine = aSegment[y].fromLine;

		aSegment[y].toLine = y + rMax;
		if (aSegment[y].toLine > bottom)
			aSegment[y].toLine = bottom;
	}
	
	// makes sure, that the last lines will be deleted
	aSegment[bottom].fromLine = bottom + 1;
 
	fromLine = top;
	toLine = top - 1;
	for (y = top; y <= bottom; y++)
	{
		if (rc->CheckCancel(rc))
			break;

		// allocate additional lines when necessary
		if (aSegment[y].toLine > toLine)
		{
			for (i = toLine + 1; i <= aSegment[y].toLine; i++)
			{
				aSegment[i].pLine = new PIXF[xres];
				for (x = 0; x < xres; x++)
				{
					aSegment[i].pLine[x].r = 
					aSegment[i].pLine[x].g = 
					aSegment[i].pLine[x].b = 0.0f;
				}
			}
			toLine = aSegment[y].toLine;
		}
		for (x = left; x <= right; x++)
		{
			posxy = x + y*xres;

			lut = CoCMap[posxy];
			if (lut > MAX_LUT)
				lut -= DOF_HIDDENMARK;
			lut = abs(lut);

			ri = (int)aLUT[lut].size;

			// calculate the number of additional CoCs at the border of the picture
			dx = 1;
			xStep = 1;

			if (x == left)
			{
				dx = -ri;
				xStep = -1;
			}
			if (x == right)
				dx = ri;

			dy = 1;
			yStep = 1;

			if (y == top)
			{
				dy = -ri;
				yStep = -1;
			}
			if (y == bottom)
				dy = ri;

			col.r = float(pSrc[posxy].r);
			col.g = float(pSrc[posxy].g);
			col.b = float(pSrc[posxy].b);

			// the CoC fits in one pixel, just copy
			if (lut == 0)
			{
				ph = aSegment[y].pLine + x;
				ph->r += col.r;
				ph->g += col.g;
				ph->b += col.b;
			}
			else  // draw a circle otherwise
			{
				colfull.r = col.r*aLUT[lut].intensity;
				colfull.g = col.g*aLUT[lut].intensity;
				colfull.b = col.b*aLUT[lut].intensity;

				// the two loops draw additional CoCs, to compensate the brightness
				for (yBrd = y; yBrd != y + dy; yBrd += yStep)
				{
					for (xBrd = x; xBrd != x + dx; xBrd += xStep)
					{
						pMatrix = (float*)(aLUT[lut].pData);

						for (yy = 0; yy < ri; yy++)
						{
							xx = -ri + 1 + int(*pMatrix++);
							h = int(*pMatrix++);
							for (i = 0; xx <= 0; i++)
							{
								if (i < h)
								{
									intensity = *pMatrix++;
									red = col.r*intensity;
									green = col.g*intensity;
									blue = col.b*intensity;
								}
								else
								{
									red = colfull.r;
									green = colfull.g;
									blue = colfull.b;
								}

								if (yBrd + yy >= top && yBrd + yy <= bottom)
								{
									if (xBrd + xx >= left && xBrd + xx <= right)
									{
										ph = aSegment[yBrd + yy].pLine + xBrd + xx;
										ph->r += red;
										ph->g += green;
										ph->b += blue;
									}

									if (xx != 0 && xBrd - xx >= left && xBrd - xx <= right)
									{
										ph = aSegment[yBrd + yy].pLine + xBrd - xx;
										ph->r += red;
										ph->g += green;
										ph->b += blue;
									}
								}
								if (yy != 0 && yBrd - yy >= top && yBrd - yy <= bottom)
								{
									if (xBrd + xx >= left && xBrd + xx <= right)
									{
										ph = aSegment[yBrd - yy].pLine + xBrd + xx;
										ph->r += red;
										ph->g += green;
										ph->b += blue;
									}

									if (xx != 0 && xBrd - xx >= left && xBrd - xx <= right)
									{
										ph = aSegment[yBrd - yy].pLine + xBrd - xx;
										ph->r += red;
										ph->g += green;
										ph->b += blue;
									}
								}
								xx++;
							}
						}
					}
				}
			}
		}

		// deallocate lines which are not needed anymore and copy them to the 
		// picture buffer
		if (aSegment[y].fromLine > fromLine)
		{
			for (yy = fromLine; yy < aSegment[y].fromLine; yy++)
			{
				pLine = rc->scrarray + yy*xres + left;
				ph = aSegment[yy].pLine + left;

				for (xx = left; xx <= right; xx++)
				{
					// areas which are marked as hidden will be sampled multiple times and
					// inserted into the picture
					if ((CoCMap[xx + yy*xres] > MAX_LUT) && (flags & DoFHidden))
					{
						red = green = blue = 0;
						for (i = 0; i < samples2; i++)
						{
							SamplePixel(rc, xx, yres - yy - 1, 
											aAperture[i].x, aAperture[i].y, &color);
							red += color.r;
							green += color.g;
							blue += color.b;
						}
						aSegment[yy].pLine[xx].r = red/(samples2 + 1);
						aSegment[yy].pLine[xx].g = green/(samples2 + 1);
						aSegment[yy].pLine[xx].b = blue/(samples2 + 1);
					}

					// the color components must be clipped possibly
					if (ph->r > 255.0f)
						ph->r = 255.0f;
					if (ph->g > 255.0f)
						ph->g = 255.0f;
					if (ph->b > 255.0f)
						ph->b = 255.0f;

					pLine->r = int(ph->r);
					pLine->g = int(ph->g);
					pLine->b = int(ph->b);

					pLine++;
					ph++;
				}
				delete aSegment[yy].pLine;
				aSegment[yy].pLine = NULL;
			}
			percent = (y - top) * percent_per_row;
			rc->UpdateStatus(rc, percent, float(clock() - starttime)/CLOCKS_PER_SEC, 
									fromLine, aSegment[y].fromLine - fromLine, 
									rc->scrarray + fromLine*xres);
			fromLine = aSegment[y].fromLine;
		}
		rc->Cooperate(rc);
		pLine += xres;
	}

	rc->UpdateStatus(rc, 1.0f, float(clock() - starttime)/CLOCKS_PER_SEC,
						  0, 0, rc->scrarray);

	return true;
}

/*************
* DESCRIPTION: set the internal error code
*					(if an error was already set, it will be kept)
* INPUT:			err	error code
* OUTPUT:		false (this allows to use "return Error(errXX)")
*************/
bool CDoF::Error(int err)
{
	if (error == errOk)
		error = err;

	return false;
}

/*************
* DESCRIPTION: gives the error text to a given error number.
*					If the number is -1, the last error which occured is displayed
*					The function resets the error code
* INPUT:			err	errór number
* OUTPUT:		point to text
*************/
char *CDoF::GetErrorMsg(int err)
{
	if (err == -1)
		err = error;

	error = errOk;

	if (err < 0 || err >= _tErr)
		return aszErrors[errUnknown];

	return aszErrors[err];
}
