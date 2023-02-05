#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include "img.h"
#include "editor.h"

typedef struct {
	char opt;
	int start[2];
	int end[2];
	char color[7];
	int thickness;
	int width;
	int height;
	char position[3];
	char algorithm[11];
	unsigned char hasOpts;
} Config;


void printHelp();
void argsErr(Image *img, char *arg, char key);
void writeConfg(Image *img, Config *cfg, char opt);
void callAction(Config cfg, Image *img);
int main(int argc, char **argv);
void rectEditing(Config cfg, Image *img);
void lineDrawing(Config cfg, Image *img);
void resizing(Config cfg, Image *img);

