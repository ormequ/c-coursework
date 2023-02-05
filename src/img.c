#include "img.h"

void printFileHeader(BitmapFileHeader header){
	printf("\nBitmapFileHeader:\n");
	printf("\tsignature:\t%x (%hu)\n", header.signature, header.signature);
	printf("\tfilesize:\t%x (%u)\n", header.filesize, header.filesize);
	printf("\treserved1:\t%x (%hu)\n", header.reserved1, header.reserved1);
	printf("\treserved2:\t%x (%hu)\n", header.reserved2, header.reserved2);
	printf("\tpixelArrOffset:\t%x (%u)\n", header.pixelArrOffset, header.pixelArrOffset);
}

void printInfoHeader(BitmapInfoHeader header){
	printf("\nBitmapInfoHeader:\n");
	printf("\theaderSize:\t%x (%u)\n", header.headerSize, header.headerSize);
	printf("\twidth:     \t%x (%u)\n", header.width, header.width);
	printf("\theight:    \t%x (%u)\n", header.height, header.height);
	printf("\tplanes:    \t%x (%hu)\n", header.planes, header.planes);
	printf("\tbitsPerPixel:\t%x (%hu)\n", header.bitsPerPixel, header.bitsPerPixel);
	printf("\tcompression:\t%x (%u)\n", header.compression, header.compression);
	printf("\timageSize:\t%x (%u)\n", header.imageSize, header.imageSize);
	printf("\txPixelsPerMeter:\t%x (%u)\n", header.xPixelsPerMeter, header.xPixelsPerMeter);
	printf("\tyPixelsPerMeter:\t%x (%u)\n", header.yPixelsPerMeter, header.yPixelsPerMeter);
	printf("\tcolorsInColorTable:\t%x (%u)\n", header.colorsInColorTable, header.colorsInColorTable);
	printf("\timportantColorCount:\t%x (%u)\n", header.importantColorCount, header.importantColorCount);
}


void exitFree(Image *img) {
	if (img) {
		for(int i = 0; i < img->h; i++){
			free(img->pixels[i]);
		}
		free(img->pixels);
		free(img);
	}
	exit(EXIT_FAILURE);
}


Image *uploadImg(char *path) {
	printf("Uploading image: %s...\n", path);
	FILE *fileRead = fopen(path, "rb");
	if (fileRead == NULL) {
		fprintf(stderr, "This file does not exist.\n");
		exit(EXIT_FAILURE);
	}
	Image *img = calloc(1, sizeof(Image));
	fread(&img->fileHeader, 1, sizeof(BitmapFileHeader), fileRead);

	unsigned int headerSize;
	fread(&headerSize, 1, sizeof(unsigned int), fileRead); // размер заголовка

	img->infoHeader.headerSize = headerSize;
	
	unsigned int readSize = MIN(img->infoHeader.headerSize, sizeof(BitmapInfoHeader));
	fread(&img->infoHeader.width, 1, readSize - sizeof(unsigned int), fileRead);
	
	if (img->infoHeader.bitsPerPixel != 24 || img->infoHeader.colorsInColorTable) {
		fprintf(stderr, "Sorry, we do not support this color format in bmp files.\n");
		free(img);
		exit(EXIT_FAILURE);
	}
	int trashSize = img->infoHeader.headerSize - readSize;
	if (trashSize > 0) {
		char *trash = calloc(trashSize, 1);
		fread(trash, 1, trashSize, fileRead);
		free(trash);
	}

	img->h = img->infoHeader.height;
	img->w = img->infoHeader.width;

	img->pixels = malloc(img->h * sizeof(Rgb*));
	for(int i = 0; i < img->h; i++){
		img->pixels[i] = malloc(img->w * sizeof(Rgb) + (4 - ((img->w) * 3) % 4) % 4);
		fread(img->pixels[i], 1, img->w * sizeof(Rgb) + (4 - ((img->w) * 3) % 4) % 4, fileRead);
	}
	fclose(fileRead);
	return img;
}


void saveImg(Image *img, char *path) {
	printf("Saving new image: %s...\n", path);
	FILE *fileSave = fopen(path, "wb");
	if (!fileSave) {
		fprintf(stderr, "Error in saving file %s.\n", path);
		exitFree(img);
		exit(EXIT_FAILURE);
	}
	img->infoHeader.height = img->h;
	img->infoHeader.width = img->w;
	fwrite(&img->fileHeader, 1, sizeof(BitmapFileHeader), fileSave);

	unsigned int readSize = MIN(img->infoHeader.headerSize, sizeof(BitmapInfoHeader));
	fwrite(&img->infoHeader, 1, readSize, fileSave);
	
	int trashSize = img->infoHeader.headerSize - readSize;
	if (trashSize > 0) {
		char *trash = calloc(trashSize, 1);
		fwrite(&trash, 1, trashSize, fileSave);
		free(trash);
	}
	unsigned int w = (img->w) * sizeof(Rgb) + (4 - ((img->w) * 3) % 4) % 4;
	for(int i = 0; i < img->h; i++){
		fwrite(img->pixels[i], 1, w, fileSave);
		free(img->pixels[i]);
	}
	free(img->pixels);
	free(img);
	fclose(fileSave);
}
