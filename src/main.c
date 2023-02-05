#include "main.h"


int main(int argc, char **argv) {
	printf("Simple image editor.\nRerun with -h for help.\n\n");
	Image *img = NULL;

	char *opts = "gnrlih?f:a:s:e:c:t:w:b:p:u:";
	struct option longOpts[] = {
		{"grayscale", no_argument, NULL, 'g'},
		{"negative", no_argument, NULL, 'n'},
		{"resize", no_argument, NULL, 'r'},
		{"line", no_argument, NULL, 'l'},
		{"info", no_argument, NULL, 'i'},
		{"help", no_argument, NULL, 'h'},
		{"file", required_argument, NULL, 'f'},
		{"algorithm", required_argument, NULL, 'a'},
		{"start", required_argument, NULL, 's'},
		{"end", required_argument, NULL, 'e'},
		{"color", required_argument, NULL, 'c'},
		{"thickness", required_argument, NULL, 't'},
		{"width", required_argument, NULL, 'w'},
		{"bigness", required_argument, NULL, 'b'},
		{"position", required_argument, NULL, 'p'},
		{"url", required_argument, NULL, 'u'},
		{NULL, no_argument, NULL, 0}
	};
	int opt, longIndex;
	opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	char *savePath = calloc(8, 1);
	Config cfg = {0};
	cfg.hasOpts = 0;
	unsigned char hasChanged = 0;
	unsigned char writeInfo = 0;
	strcpy(savePath, "out.bmp");
	if (opt == -1) {
		printHelp();
	}
	while (opt != -1){
		if (!hasChanged && strchr("gnrlf", opt)) {
			hasChanged = 1;
		}
		switch (opt) {
			case 'i':
				writeInfo = 1;
				break;
			case 'h':
			case '?':
				printHelp();
				break;
			case 'g':
			case 'n':
			case 'r':
			case 'l':
				if (cfg.opt && strchr("gnrl", cfg.opt)) {
					puts("Sorry, this app cannot do more than one action. Please, run again.");
					exitFree(img);
				}
				cfg.opt = opt;
				break;
			case 'f':
				savePath = realloc(savePath, strlen(optarg) + 1);
				strcpy(savePath, optarg);
				break;
			case 'u':
				if (!strstr(optarg, ".bmp") && !strstr(optarg, ".dib") && !strstr(optarg, ".rle")) {
					fprintf(stderr, "File Error - %s: -u argument has to be a .bmp file! Print -h or -? for help.\n", optarg);
					exitFree(img);
				}
				img = uploadImg(optarg);
				break;
			default:
				writeConfg(img, &cfg, opt);
				break;
		}
		opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	}
	if (!img && (cfg.opt || writeInfo)) {
		puts("You should specify an image.");
		exitFree(img);
	}
	callAction(cfg, img);
	if (writeInfo) {
		printFileHeader(img->fileHeader);
		printInfoHeader(img->infoHeader);
	}
	if (hasChanged) {
		saveImg(img, savePath);
	}
	
	free(savePath);
	return 0;
}


void printHelp() {
	// Вместо пробелов можно было бы использовать \t, но так лучше читается
	puts("Required key:");
	puts("\t-u --url \"path/to/file.bmp\" — path to image you want to edit");
	puts("Optional keys without arguments:");
	puts("\t-h -? --help   — help");
	puts("\t-i --info      — information about this file");
	puts("\t-g --grayscale — make image in black and white. Parameters: -s --start, -e --end, -a --algorithm");
	puts("\t-n --negative  — make image in negative. Parameters: -s --start, -e --end");
	puts("\t-r --resize    — resize image. Parameters: -w --width, -b --bigness, -c --color, -p --position (default 000000)");
	puts("\t-l --line      — draw a line. Parameters: -s --start, -e --end, -t --thickness, -c --color (default 000000)");
	puts("Optional keys with arguments:");
	puts("\t-f --file      \"where/to/save.bmp\"        — path to image where you want to be saved");
	puts("\t-s --start     <x coord>,<y coord>        — start of the action. Write to integer numbers separated by comma. Default 0,0");
	puts("\t-e --end       <x coord>,<y coord>        — end of the action. Write to integer numbers separated by comma. Default <image width>,<image height>");
	puts("\t-a --algorithm <luminosity|avg|lightness> — algorithm of grayscale. Default luminosity. Avg - average");
	puts("\t-c --color     <color in HEX>             — color of the action");
	puts("\t-w --width     <integer value>            — width of new image");
	puts("\t-b --bigness   <integer value>            — height of new image");
	puts("\t-p --position  <lb|rb|c|lt|rt>            — position of resizing. Left bottom | right bottom | center | left top | right top");
	puts("\t-t --thickness <integer value>            — thickness of the line");
}


void argsErr(Image *img, char *arg, char key) {
	fprintf(stderr, "Arguments Error - %s: Please write arguments of -%c in current format! Print -h or -? for help.\n", arg, key);
	exitFree(img);
}


void writeConfg(Image *img, Config *cfg, char opt) {
	cfg->hasOpts = 1;
	char *digitOpts = "setwb";
	if (strchr(digitOpts, opt) && !isdigit(optarg[0])) {
		argsErr(img, "incorrect type of argument", opt);
	}
	char *secondPart;
	switch(opt) {
		case 's':
			if (!strchr(optarg, ',')) {
				argsErr(img, "incorrect type of argument", opt);
			}
			secondPart = strchr(optarg, ',') + 1;
			// isdigit(*secondPart) вызывает ошибку
			if ((int)secondPart[0] > 47 && (int)secondPart[0] < 58) {
				cfg->start[0] = atoi(optarg);
				cfg->start[1] = atoi(secondPart);
			} else {
				argsErr(img, "incorrect type of argument", opt);
			}
			break;
		case 'e':
			if (!strchr(optarg, ',')) {
				argsErr(img, "incorrect type of argument", opt);
			}
			secondPart = strchr(optarg, ',') + 1;
			if ((int)secondPart[0] > 47 && (int)secondPart[0] < 58) {
				cfg->end[0] = atoi(optarg);
				cfg->end[1] = atoi(secondPart);
			} else {
				argsErr(img, "incorrect type of argument", opt);
			}
			break;
		case 'c':
			if (strlen(optarg) != 6 && strlen(optarg) != 3) {
				argsErr(img, "incorrect color: write color in HEX like FFFFFF", opt);
			} 
			strcpy(cfg->color, optarg);
			break;
		case 't':
			cfg->thickness = atoi(optarg);
			break;
		case 'w':
			cfg->width = atoi(optarg);
			break;
		case 'b':
			cfg->height = atoi(optarg);
			break;
		case 'p':
			if (strlen(optarg) > 2) {
				argsErr(img, "incorrect position", opt);
			} 
			strcpy(cfg->position, optarg);
			break;
		case 'a':
			if (strcmp(optarg, "luminosity") && strcmp(optarg, "avg") && strcmp(optarg, "lightness")) {
				argsErr(img, "incorrect algorithm", opt);
			} 
			strncpy(cfg->algorithm, optarg, 10);
			break;
	}
}


void callAction(Config cfg, Image *img) {
	switch (cfg.opt) {
		case 'g':
		case 'n':
			rectEditing(cfg, img);
			break;
		case 'l':
			lineDrawing(cfg, img);
			break;
		case 'r':
			resizing(cfg, img);
			break;
	}
}


void resizing(Config cfg, Image *img) {
	if (!cfg.hasOpts) {
		argsErr(img, "too few arguments", 'r');
	}
	cfg.width = cfg.width ? cfg.width : img->w;
	cfg.height = cfg.height ? cfg.height : img->h;
	
	for (int i = 0; i < strlen(cfg.color); i++) {
		if (!strchr("1234567890abcdefABCDEF", cfg.color[i])) {
			argsErr(img, "write color in HEX format", 'r');
		}
	}

	if (!strlen(cfg.color)) {
		strcpy(cfg.color, "000000");
	}
	if (!strlen(cfg.position)) {
		strcpy(cfg.position, "lb");
	}
	resizeImg(img, cfg.width, cfg.height, cfg.color, cfg.position);
}


void lineDrawing(Config cfg, Image *img) {
	if (!cfg.hasOpts) {
		argsErr(img, "too few arguments", 'l');
	}
	for (int i = 0; i < strlen(cfg.color); i++) {
		if (!strchr("1234567890abcdefABCDEF", cfg.color[i])) {
			argsErr(img, "write color in HEX format", 'r');
		}
	}
	if (!strlen(cfg.color)) {
		strcpy(cfg.color, "000000");
	}
	cfg.thickness = cfg.thickness ? cfg.thickness : 1;
	drawLine(img, cfg.start, cfg.end, cfg.thickness, cfg.color);
}


void rectEditing(Config cfg, Image *img) {
	int coords[4] = {0, 0, img->w, img->h};
	char algorithm[11] = "luminosity";
	if (cfg.hasOpts) {
		if (strlen(cfg.algorithm)) {
			strncpy(algorithm, cfg.algorithm, 10);
		}
		coords[0] = cfg.start[0];
		coords[1] = cfg.start[1];
		coords[2] = cfg.end[0] ? cfg.end[0] : coords[2];
		coords[3] = cfg.end[1] ? cfg.end[1] : coords[3];
	}
	if (cfg.opt == 'g') {
		makeGrayscale(img, coords, algorithm);
	} else if (cfg.opt == 'n') {
		makeNegative(img, coords);
	}
}
