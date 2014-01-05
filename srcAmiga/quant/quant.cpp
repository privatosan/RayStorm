/***************
 * MODUL:         quant
 * NAME:          quant.cpp
 * VERSION:       1.0 08.02.1995
 * DESCRIPTION:   quantize the colors in a pixmap down to a specified number
 *		Uses the median-cut colormap generator.  It's based
 *		on Paul Heckbert's paper "Color Image Quantization for Frame Buffer
 *		Display", SIGGRAPH '82 Proceedings, page 297.
 * FUNCTIONS:
 *	AUTHORS:			Jef Poskanzer, Andreas Heumann
 * BUGS:          none
 * TO DO:         nothing
 * HISTORY:       DATE		NAME	COMMENT
 *						08.02.95	ah		first release
 **************/

#include <stdlib.h>
#include <time.h>
#include <new.h>

#include "quant.h"
#include "error.h"

// prototypes
static void mediancut(colorhist_item *chv, int colors,
	int sum, pixval maxval, int newcolors, colorhist_item *colormap);
static int redcompare(const void *ch1, const void *ch2);
static int greencompare(const void *ch1, const void *ch2);
static int bluecompare(const void *ch1, const void *ch2);
static int sumcompare(const void *ch1, const void *ch2);
static colorhist_vector computecolorhist(pixel** pixels, int cols, int rows,
	int maxcolors, int* colorsP);
static colorhash_table computecolorhash(pixel** pixels, int cols, int rows,
	int maxcolors, int* colorsP);
static colorhist_vector colorhashtocolorhist(colorhash_table cht, int maxcolors);
static colorhash_table alloccolorhash();
static void freecolorhash(colorhash_table cht);
static void freecolorhist(colorhist_vector chv);
static int lookupcolor(colorhash_table cht, pixel* colorP);
static int addtocolorhash(colorhash_table cht, pixel* colorP, int value);
static pixel* allocrow(int cols, int size);
static void freerow(pixel *row);

/*************
 * FUNCTION:		quant
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	main function for colorquantization
 *						returns array with colors
 *						pixels is changed to pen numbers
 * INPUT:			cols			picture columns
 *						rows			picture rows
 *						newcolors	number of colors
 *						pixels		pointerarry to truecolor-bitmap rows (4 byte/pixel)
 *						colormap		pointer to colorarray (newcolors elements)
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
void quant(int cols, int rows, int newcolors, pixel** pixels, colorhist_item *colormap, char floyd)
{
	register pixel* pP;
	int row;
	register int col, limitcol;
	pixval maxval, newmaxval;
	int colors;
	register int ind;
	colorhist_item *chv;
	colorhash_table cht;
	int usehash;
	long* thisrerr;
	long* nextrerr;
	long* thisgerr;
	long* nextgerr;
	long* thisberr;
	long* nextberr;
	long* temperr;
	register long sr, sg, sb, err;
	int fs_direction;

	fs_direction = 0;

	// Step 1: attempt to make a histogram of the colors, unclustered.
	// If at first we don't succeed, lower maxval to increase color
	// coherence and try again.  This will eventually terminate, with
	// maxval at worst 15, since 32^3 is approximately MAXCOLORS.
	maxval = 0xFFFFFF;
	for ( ; ; )
	{
//		cout << "making histogram...\n";
		chv = computecolorhist(pixels, cols, rows, MAXCOLORS, &colors);
		if (chv)
			break;
//		cout << "too many colors!\n";
		newmaxval = maxval / 2;
//		cout << "scaling colors from maxval=" << maxval << " to maxval=" << newmaxval << " to improve clustering...\n";
		for (row = 0; row < rows; ++row)
			for (col = 0, pP = pixels[row]; col < cols; ++col, ++pP)
				DEPTH(*pP, *pP, maxval, newmaxval);
			maxval = newmaxval;
	}
//	cout << colors << " colors found\n";

	// Step 2: apply median-cut to histogram, making the new colormap.
//	cout << "choosing " << newcolors << " colors...\n";
	mediancut(chv, colors, rows*cols, maxval, newcolors, colormap);
	freecolorhist(chv);

	// Step 3: map the colors in the image to their closest match in the
	// new colormap.
//	cout << "mapping image to new colors...\n";
	cht = alloccolorhash();
	usehash = 1;
	if (floyd)
	{
		// Initialize Floyd-Steinberg error vectors.
		thisrerr = (long*)allocrow(cols+2, sizeof(long));
		nextrerr = (long*)allocrow(cols+2, sizeof(long));
		thisgerr = (long*)allocrow(cols+2, sizeof(long));
		nextgerr = (long*)allocrow(cols+2, sizeof(long));
		thisberr = (long*)allocrow(cols+2, sizeof(long));
		nextberr = (long*)allocrow(cols+2, sizeof(long));
		srand(time(0));
		for (col=0; col<cols+2; ++col)
		{
			thisrerr[col] = rand() % (FS_SCALE*2) - FS_SCALE;
			thisgerr[col] = rand() % (FS_SCALE*2) - FS_SCALE;
			thisberr[col] = rand() % (FS_SCALE*2) - FS_SCALE;
			// (random errors in [-1 .. 1])
		}
		fs_direction = 1;
	}
	for (row=0; row<rows; ++row)
	{
		if (floyd)
			for (col=0; col<cols+2; ++col)
				nextrerr[col] = nextgerr[col] = nextberr[col] = 0;
		if ((!floyd) || fs_direction)
		{
			col = 0;
			limitcol = cols;
			pP = pixels[row];
		}
		else
		{
			col = cols - 1;
			limitcol = -1;
			pP = &(pixels[row][col]);
		}
		do
		{
			if (floyd)
			{
				// Use Floyd-Steinberg errors to adjust actual color.
				sr = GETR(*pP)+thisrerr[col+1]/FS_SCALE;
				sg = GETG(*pP)+thisgerr[col+1]/FS_SCALE;
				sb = GETB(*pP)+thisberr[col+1]/FS_SCALE;
				if (sr<0)
					sr = 0;
				else
					if (sr>maxval)
						sr = maxval;
				if (sg<0)
					sg = 0;
				else
					if (sg>maxval)
						sg = maxval;
				if (sb<0)
					sb = 0;
				else
					if (sb>maxval)
						sb = maxval;
				ASSIGN(*pP, sr, sg, sb);
			}

			// Check hash table to see if we have already matched this color.
			ind = lookupcolor(cht, pP);
			if (ind == -1)
			{ // No; search colormap for closest match.
				register int i, r1, g1, b1, r2, g2, b2;
				register long dist, newdist;
				r1 = GETR(*pP);
				g1 = GETG(*pP);
				b1 = GETB(*pP);
				dist = 2000000000;
				for (i=0; i<newcolors; ++i)
				{
					r2 = GETR(colormap[i].color);
					g2 = GETG(colormap[i].color);
					b2 = GETB(colormap[i].color);
					newdist = (r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2);
					if (newdist<dist)
					{
						ind = i;
						dist = newdist;
					}
				}
				if (usehash)
				{
					if (addtocolorhash(cht, pP, ind) < 0)
					{
//						cout << "out of memory adding to hash table, proceeding without it\n";
						usehash = 0;
					}
				}
			}

			if (floyd)
			{
				// Propagate Floyd-Steinberg error terms.
				if (fs_direction)
				{
					err = (sr-(long)GETR(colormap[ind].color)) * FS_SCALE;
					thisrerr[col+2] += (err*7)/16;
					nextrerr[col  ] += (err*3)/16;
					nextrerr[col+1] += (err*5)/16;
					nextrerr[col+2] += (err  )/16;
					err = (sg-(long)GETG(colormap[ind].color)) * FS_SCALE;
					thisgerr[col+2] += (err*7)/16;
					nextgerr[col  ] += (err*3)/16;
					nextgerr[col+1] += (err*5)/16;
					nextgerr[col+2] += (err  )/16;
					err = (sb-(long)GETB(colormap[ind].color)) * FS_SCALE;
					thisberr[col+2] += (err*7)/16;
					nextberr[col  ] += (err*3)/16;
					nextberr[col+1] += (err*5)/16;
					nextberr[col+2] += (err  )/16;
				}
				else
				{
					err = (sr-(long)GETR(colormap[ind].color)) * FS_SCALE;
					thisrerr[col  ] += (err*7)/16;
					nextrerr[col+2] += (err*3)/16;
					nextrerr[col+1] += (err*5)/16;
					nextrerr[col  ] += (err  )/16;
					err = (sg-(long)GETG(colormap[ind].color)) * FS_SCALE;
					thisgerr[col  ] += (err*7)/16;
					nextgerr[col+2] += (err*3)/16;
					nextgerr[col+1] += (err*5)/16;
					nextgerr[col  ] += (err  )/16;
					err = (sb-(long)GETB(colormap[ind].color)) * FS_SCALE;
					thisberr[col  ] += (err*7)/16;
					nextberr[col+2] += (err*3)/16;
					nextberr[col+1] += (err*5)/16;
					nextberr[col  ] += (err  )/16;
				}
			}

			pP->r = (pixval)ind;
//			*pP = colormap[ind].color;

			if ((!floyd) || fs_direction)
			{
				++col;
				++pP;
			}
			else
			{
				--col;
				--pP;
			}
		}
		while (col != limitcol);

		if (floyd)
		{
			temperr = thisrerr;
			thisrerr = nextrerr;
			nextrerr = temperr;
			temperr = thisgerr;
			thisgerr = nextgerr;
			nextgerr = temperr;
			temperr = thisberr;
			thisberr = nextberr;
			nextberr = temperr;
			fs_direction = ! fs_direction;
		}	
	}
	if(floyd)
	{
		freerow((pixel*)thisrerr);
		freerow((pixel*)nextrerr);
		freerow((pixel*)thisgerr);
		freerow((pixel*)nextgerr);
		freerow((pixel*)thisberr);
		freerow((pixel*)nextberr);
	}
}

/*************
 * FUNCTION:		mediancut
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	apply median-cut to histogram
 * INPUT:			chv			colorhistogram vector
 *						colors		actual number of colors
 *						sum			rows*columns
 *						maxval		maximal number of colors
 *						newcolors	new number of colors
 *						colormap		new colorhistogram vector
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
static void mediancut(colorhist_item *chv, int colors, int sum,
	pixval maxval, int newcolors, colorhist_item *colormap)
{
	struct box *bv;
	int bi, i;
	int boxes;
	int indx, clrs;
	int minr, maxr, ming, maxg, minb, maxb, v;
	int sm;
	int halfsum, lowersum;
#ifdef REP_AVERAGE_PIXELS
	long r, g, b;
	long sumcol;
#endif
#ifdef REP_AVERAGE_COLORS
	long r, g, b;
#endif

	bv = new struct box[newcolors];
	if (!bv)
		Error(RL_ABORT, ERR_MEM);
	for (i=0; i<newcolors; i++)
	{
		ASSIGN(colormap[i].color, 0, 0, 0);
	}

	// Set up the initial box.
	bv[0].ind = 0;
	bv[0].colors = colors;
	bv[0].sum = sum;
	boxes = 1;

	// Main loop: split boxes until we have enough.
	while (boxes < newcolors)
	{
		// Find the first splittable box.
		for (bi=0; bi<boxes; ++bi)
		{
			if (bv[bi].colors >= 2)
				break;
		}
		if (bi == boxes)
			break;	// ran out of colors!
		indx = bv[bi].ind;
		clrs = bv[bi].colors;
		sm = bv[bi].sum;

		// Go through the box finding the minimum and maximum of each
		// component - the boundaries of the box.
		minr = maxr = GETR(chv[indx].color);
		ming = maxg = GETG(chv[indx].color);
		minb = maxb = GETB(chv[indx].color);
		for (i=1; i<clrs; ++i)
		{
			v = GETR(chv[indx+i].color);
			if (v<minr)
				minr = v;
			if (v>maxr)
				maxr = v;
			v = GETG(chv[indx+i].color);
			if (v<ming)
				ming = v;
			if (v>maxg)
				maxg = v;
			v = GETB(chv[indx+i].color);
			if (v<minb)
				minb = v;
			if (v>maxb)
				maxb = v;
		}

		// Find the largest dimension, and sort by that component. This
		// is done by simply comparing the range in RGB space.
		if (maxr-minr>=maxg-ming && maxr-minr>=maxb-minb)
			qsort((char*)&(chv[indx]), clrs, sizeof(struct colorhist_item),
					redcompare);
		else
			if (maxg-ming>=maxb-minb)
				qsort((char*)&(chv[indx]), clrs, sizeof(struct colorhist_item),
					greencompare);
			else
				qsort((char*)&(chv[indx]), clrs, sizeof(struct colorhist_item),
						bluecompare);

		// Now find the median based on the counts, so that about half the
		// pixels (not colors, pixels) are in each subdivision.
		lowersum = chv[indx].value;
		halfsum = sm/2;
		for (i=1; i<clrs-1; ++i)
		{
			if (lowersum>=halfsum)
				break;
			lowersum += chv[indx+i].value;
		}
	
		// Split the box, and sort to bring the biggest boxes to the top.
		bv[bi].colors = i;
		bv[bi].sum = lowersum;
		bv[boxes].ind = indx+i;
		bv[boxes].colors = clrs-i;
		bv[boxes].sum = sm-lowersum;
		++boxes;
		qsort((char*)bv, boxes, sizeof(struct box), sumcompare);
	}

	// Ok, we've got enough boxes.  Now choose a representative color for
	// each box.  There are a number of possible ways to make this choice.
	// One would be to choose the center of the box; this ignores any structure
	// within the boxes.  Another method would be to average all the colors in
	// the box - this is the method specified in Heckbert's paper.  A third
	// method is to average all the pixels in the box.  You can switch which
	// method is used by switching the commenting on the REP_ defines at
	// the beginning of this source file.
	for (bi=0; bi<boxes; ++bi)
	{
		#ifdef REP_CENTER_BOX
			indx = bv[bi].ind;
			clrs = bv[bi].colors;

			minr = maxr = GETR(chv[indx].color);
			ming = maxg = GETG(chv[indx].color);
			minb = maxb = GETB(chv[indx].color);
			for (i=1; i<clrs; ++i)
			{
				v = GETR(chv[indx+i].color);
				minr = min(minr, v);
				maxr = max(maxr, v);
				v = GETG(chv[indx+i].color);
				ming = min(ming, v);
				maxg = max(maxg, v);
				v = GETB(chv[indx+i].color);
				minb = min(minb, v);
				maxb = max(maxb, v);
			}
			ASSIGN(colormap[bi].color, (minr+maxr)/2, (ming+maxg)/2, (minb+maxb)/2);
		#endif /*REP_CENTER_BOX*/
		#ifdef REP_AVERAGE_COLORS
			indx = bv[bi].ind;
			clrs = bv[bi].colors;
			r = g = b = 0;

			for (i=0; i<clrs; ++i)
			{
				r += GETR(chv[indx+i].color);
				g += GETG(chv[indx+i].color);
				b += GETB(chv[indx+i].color);
			}
			r = r/clrs;
			g = g/clrs;
			b = b/clrs;
			ASSIGN(colormap[bi].color, r, g, b);
		#endif /*REP_AVERAGE_COLORS*/
		#ifdef REP_AVERAGE_PIXELS
			indx = bv[bi].ind;
			clrs = bv[bi].colors;
			r = g = b = sumcol = 0;

			for (i=0; i<clrs; ++i)
			{
				r += GETR(chv[indx+i].color) * chv[indx+i].value;
				g += GETG(chv[indx+i].color) * chv[indx+i].value;
				b += GETB(chv[indx+i].color) * chv[indx+i].value;
				sumcol += chv[indx+i].value;
			}
			r = r/sumcol;
			if (r>maxval)
				r = maxval;	// avoid math errors
			g = g/sumcol;
			if (g>maxval)
				g = maxval;
			b = b/sumcol;
			if (b>maxval)
				b = maxval;
			ASSIGN(colormap[bi].color, r, g, b);
		#endif // REP_AVERAGE_PIXELS
	}
	delete [ ] bv;
}

/*************
 * FUNCTION:		redcompare
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	comparefunction for qsort; compares red components
 * INPUT:			ch1		pointer to first colorhistorgram item
 *						ch2		pointer to second colorhistorgram item
 * OUTPUT:			-1,0,1 for less, equal, greater
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
static int redcompare(const void *ch1, const void *ch2)
{
	return GETR(((colorhist_vector)ch1)->color ) - GETR(((colorhist_vector)ch2)->color);
}

/*************
 * FUNCTION:		greencompare
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	comparefunction for qsort; compares green components
 * INPUT:			ch1		pointer to first colorhistorgram item
 *						ch2		pointer to second colorhistorgram item
 * OUTPUT:			-1,0,1 for less, equal, greater
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
static int greencompare(const void *ch1, const void *ch2)
{
	return GETG(((colorhist_vector)ch1)->color ) - GETG( ((colorhist_vector)ch2)->color);
}

/*************
 * FUNCTION:		bluecompare
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	comparefunction for qsort; compares blue components
 * INPUT:			ch1		pointer to first colorhistorgram item
 *						ch2		pointer to second colorhistorgram item
 * OUTPUT:			-1,0,1 for less, equal, greater
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
static int bluecompare(const void *ch1, const void *ch2)
{
	return GETB(((colorhist_vector)ch1)->color ) - GETB( ((colorhist_vector)ch2)->color);
}

/*************
 * FUNCTION:		sumcompare
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	comparefunction for qsort; compares color sums
 * INPUT:			ch1		pointer to first boxvector
 *						ch2		pointer to second boxvector
 * OUTPUT:			-1,0,1 for less, equal, greater
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
static int sumcompare(const void *b1, const void *b2)
{
	return ((box*)b2)->sum - ((box*)b1)->sum;
}

/*************
 * FUNCTION:		computecolorhist
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	computes the color histogramm
 * INPUT:			pixels		doublepointer to pixmap
 *						cols			columns of pixmap
 *						rows			rows of pixmap
 *						maxcolors	maximum number of colors
 *						colorsP		pointer to colorvariable (here are the number
 *										of found colors stored)
 * OUTPUT:			colorhistogram vector
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
colorhist_vector computecolorhist(pixel** pixels, int cols, int rows, int maxcolors, int* colorsP)
{
	colorhash_table cht;
	colorhist_item *chv;

	cht = computecolorhash(pixels, cols, rows, maxcolors, colorsP);
	if (!cht)
		return NULL;
	chv = colorhashtocolorhist(cht, maxcolors);
	freecolorhash(cht);
	return chv;
}

/*************
 * FUNCTION:		computecolorhash
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	
 * INPUT:			pixels		doublepointer to pixmap
 *						cols			columns of pixmap
 *						rows			rows of pixmap
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
colorhash_table computecolorhash(pixel** pixels, int cols, int rows,
	int maxcolors, int* colorsP)
{
	colorhash_table cht;
	register pixel* pP;
	colorhist_list chl;
	int col, row, hash;

	cht = alloccolorhash();
	*colorsP = 0;

	/* Go through the entire image, building a hash table of colors. */
	for (row = 0; row < rows; ++row)
	{
		for (col = 0, pP = pixels[row]; col < cols; ++col, ++pP)
		{
			hash = hashpixel(*pP);
			for (chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next)
			{
				if (EQUAL(chl->ch.color, *pP))
					break;
			}
			if (chl != (colorhist_list)0)
			{
				++(chl->ch.value);
			}
    		else
			{
				if (++(*colorsP) > maxcolors)
		    	{
					freecolorhash(cht);
					return NULL;
				}
				chl = (colorhist_list) new struct colorhist_list_item;
				if (chl == 0)
					Error(RL_ABORT, ERR_MEM);
				chl->ch.color = *pP;
				chl->ch.value = 1;
				chl->next = cht[hash];
				cht[hash] = chl;
			}
		}
	}
	return cht;
}

/*************
 * FUNCTION:		colorhashtocolorhist
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	Collate the hash table into a simple colorhist array.
 * INPUT:			cht			
 *						maxcolors	
 * OUTPUT:			colorhistogram vector
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
colorhist_vector colorhashtocolorhist(colorhash_table cht, int maxcolors)
{
	colorhist_vector chv;
	colorhist_list chl;
	int i, j;

	/* Now collate the hash table into a simple colorhist array. */
	chv = (colorhist_vector) new struct colorhist_item[maxcolors];
	/* (Leave room for expansion by caller.) */
	if (!chv)
		Error(RL_ABORT, ERR_MEM);

	/* Loop through the hash table. */
	j = 0;
	for (i = 0; i < HASH_SIZE; ++i)
		for (chl = cht[i]; chl != (colorhist_list) 0; chl = chl->next)
		{
			/* Add the new entry. */
			chv[j] = chl->ch;
			++j;
		}
	/* All done. */
	return chv;
}

/*************
 * FUNCTION:		alloccolorhash
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	get memory for colorhash table
 * INPUT:			none
 * OUTPUT:			colorhash_table
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
colorhash_table alloccolorhash()
{
	colorhash_table cht;
	int i;

	cht = (colorhash_table) new colorhist_list[HASH_SIZE];
	if (cht == 0)
		Error(RL_ABORT, ERR_MEM);

	for (i = 0; i < HASH_SIZE; ++i)
		cht[i] = (colorhist_list)0;

	return cht;
}

/*************
 * FUNCTION:		freecolorhash
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	free memory of colorhashtable
 * INPUT:			cht		colorhash table
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
void freecolorhash(colorhash_table cht)
{
	int i;
	colorhist_list chl, chlnext;

	for (i = 0; i < HASH_SIZE; ++i)
		for (chl = cht[i]; chl != (colorhist_list) 0; chl = chlnext)
		{
			chlnext = chl->next;
			delete chl;
		}
	delete [ ] cht;
}

/*************
 * FUNCTION:		freecolorhist
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	free color histogram vector
 * INPUT:			chv		colorhistogram vector
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
void freecolorhist(colorhist_vector chv)
{
	delete [ ] chv;
}

/*************
 * FUNCTION:		lookupcolor
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	Look in colorhashtable if color is already here
 * INPUT:			cht		colorhashtable
 *						colorP	color
 * OUTPUT:			index of colorhashtable
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
int lookupcolor(colorhash_table cht, pixel* colorP)
{
	int hash;
	colorhist_list chl;

	hash = hashpixel(*colorP);
	for (chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next)
	if (EQUAL(chl->ch.color, *colorP))
		return chl->ch.value;

	return -1;
}

/*************
 * FUNCTION:		addtocolorhash
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	adds a color to colorhashtable
 * INPUT:			cht		colorhashtable
 *						colorP	color
 *						value		colorhashvalue
 * OUTPUT:			-1 failed; 0 ok
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
int addtocolorhash(colorhash_table cht, pixel* colorP, int value)
{
	register int hash;
	register colorhist_list chl;
	
	chl = (colorhist_list)new struct colorhist_list_item;
	if (chl == 0)
		return -1;
	hash = hashpixel(*colorP);
	chl->ch.color = *colorP;
	chl->ch.value = value;
	chl->next = cht[hash];
	cht[hash] = chl;
		return 0;
}

/*************
 * FUNCTION:		allocrow
 * VERSION:			1.0 08.02.1995
 * DESCRIPTION:	allocates one pixel row
 * INPUT:			cols		number of columns
 *						size		size of one pixel
 * OUTPUT:			pointer to pixel row
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.02.95	ah			first release
 *************/
pixel* allocrow(int cols, int size)
{
	return (pixel*)new char[cols*size];
}

/*************
 * FUNCTION:		allocrow
 * VERSION:			1.0 04.03.1995
 * DESCRIPTION:	allocates one pixel row
 * INPUT:			row		pointer to pixel row
 * OUTPUT:			none
 * USES:				none
 * SIDEEFFECTS:	none
 * PROBLEMS:		none
 * BUGS:				none
 * HISTORY:       DATE		NAME		COMMENT
 *						04.03.95	ah			first release
 *************/
void freerow(pixel *row)
{
	delete [ ] row;
}


