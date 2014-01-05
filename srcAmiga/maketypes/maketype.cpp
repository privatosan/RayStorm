#include <iostream.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	short mask;
	char keys[16];
	char handler[8];
} KEY;

#define MAX_OBJECTS 6
KEY objectkeys[MAX_OBJECTS] =
{
	{ 0xF0F0,  'F',  'O',  'R',  'M',   0 ,   0 ,   0 ,   0 ,  'T',  'D',  'D',  'D',   0 ,   0 ,   0 ,   0 , 't', 'd', 'd', 'd', 0, 0, 0, 0 },
	{ 0xC000,  'M',  'M',   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 , '3', 'd', 's',   0, 0, 0, 0, 0 },
	{ 0xF0F0,  'F',  'O',  'R',  'M',   0 ,   0 ,   0 ,   0 ,  'R',  'S',  'O',  'B',   0 ,   0 ,   0 ,   0 , 'r', 's', 'o', 'b', 0, 0, 0, 0 },
	{ 0xF0F0,  'F',  'O',  'R',  'M',   0 ,   0 ,   0 ,   0 ,  'L',  'W',  'O',  'B',   0 ,   0 ,   0 ,   0 , 'l', 'w', 'o', 'b', 0, 0, 0, 0 },
	{ 0xE000,  'n',  'f',  'f',   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 , 'n', 'f', 'f',   0, 0, 0, 0, 0 },
	{ 0x0000,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 , 'r', 'a', 'w',   0, 0, 0, 0, 0 }
};

#define MAX_PICTURES 4
KEY picturekeys[MAX_PICTURES] =
{
	{ 0xF0F0,  'F',  'O',  'R',  'M',   0 ,   0 ,   0 ,   0 ,  'I',  'L',  'B',  'M',   0 ,   0 ,   0 ,   0 , 'i', 'l', 'b', 'm', 0, 0, 0, 0 },
	{ 0xFF00, 0x89,  'P',  'N',  'G', 0x0D, 0x0A, 0x1A, 0x0A,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 , 'p', 'n', 'g',   0, 0, 0, 0, 0 },
	{ 0xFFC0, 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10,  'J',  'F',  'I',  'F',   0 ,   0 ,   0 ,   0 ,   0 ,   0 , 'j', 'p', 'e', 'g', 0, 0, 0, 0 },
	{ 0x0000,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 , 't', 'g', 'a',   0, 0, 0, 0, 0 }
};

#ifdef __AMIGA__
#define OBJECT_TYPEFILE    "ray:modules/object/types"
#define PICTURE_TYPEFILE   "ray:modules/picture/types"
#else 
#define OBJECT_TYPEFILE    "otypes.dat"
#define PICTURE_TYPEFILE   "ptypes.dat"
#endif

int main()
{
	int i;
	FILE *f;
	short *p;
	char c = '\0';

	cout << "Creating object type file" << endl;
	f = fopen(OBJECT_TYPEFILE,"wb");
	if(!f)
	{
		cout << "Can't open " << OBJECT_TYPEFILE << endl;
		return 10;
	}
	for(i=0; i<MAX_OBJECTS; i++)
	{
		p = &objectkeys[i].mask;
#ifndef __AMIGA__
		*p = (((short)(*p)&0xFF) << 8) | (((short)(*p)&0xFF00) >> 8);
#endif
		fwrite(p, sizeof(char), sizeof(KEY), f);
	}
	fclose(f);

	cout << "Creating picture type file" << endl;
	f = fopen(PICTURE_TYPEFILE,"wb");
	if(!f)
	{
		cout << "Can't open " << PICTURE_TYPEFILE << endl;
		return 10;
	}
	for(i=0; i<MAX_PICTURES; i++)
	{
		p = &picturekeys[i].mask;
#ifndef __AMIGA__
		*p = (((short)(*p)&0xFF) << 8) | (((short)(*p)&0xFF00) >> 8);
#endif
		fwrite(&picturekeys[i].mask, sizeof(char), sizeof(KEY), f);
	}
	fclose(f);

	return 0;
}
