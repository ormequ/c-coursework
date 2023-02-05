#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "img.h"
#pragma once


void makeNegative(Image *img, int *coords);
void makeGrayscale(Image *img, int *coords, char *algorithm);
int min(int count, ...);
int max(int count, ...);
void drawLine(Image *img, int *start, int *end, int thickness, char *color);
void resizeImg(Image *img, int width, int height, char *color, char *position);
void setPixels(Image *img, int x, int y, int quantity, Rgb color);
Rgb getColor(char *color, Image *img);