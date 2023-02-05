#include <stdio.h>
#include <stdlib.h>
#define MIN(a, b) (a < b ? a : b)

#pragma once
#pragma pack (push, 1)
typedef struct
{
	unsigned short signature;
	unsigned int filesize;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int pixelArrOffset;
} BitmapFileHeader;

typedef struct {
    unsigned int cieXYZX;
    unsigned int ciexyzY;
    unsigned int ciexyzZ;
} CIEXYZ;

typedef struct{
    CIEXYZ ciexyzRed;
    CIEXYZ ciexyzGreen;
    CIEXYZ ciexyzBlue;
} CIEXYZTRIPLE;

typedef struct
{
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int xPixelsPerMeter;
	unsigned int yPixelsPerMeter;
	unsigned int colorsInColorTable;
	unsigned int importantColorCount;
	unsigned int redChannelBitmask;
	unsigned int greenChannelBitmask;
	unsigned int blueChannelBitmask;
	unsigned int alphaChannelBitmask;
	unsigned int colorSpaceType;
	CIEXYZTRIPLE colorSpaceEndpoints;
	unsigned int gammaForRedChannel;
	unsigned int gammaForGreenChannel;
	unsigned int gammaForBlueChannel;
	unsigned int intent;
	unsigned int ICCProfileData;
	unsigned int ICCProfileSize;
	unsigned int reserved;
} BitmapInfoHeader;

typedef struct
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
} Rgb;


typedef struct 
{
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	int h;
	int w;
	Rgb **pixels;
} Image;

#pragma pack(pop)


void printInfoHeader(BitmapInfoHeader header);
void printFileHeader(BitmapFileHeader header);
void exitFree(Image *img);
Image *uploadImg(char *path);
void saveImg(Image *img, char *path);