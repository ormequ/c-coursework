#include "editor.h"


void makeNegative(Image *img, int *coords) {
    puts("Making negative rectangle...");
    for (int i = img->h - min(2, coords[1], img->h) - 1; i >= img->h - min(2, coords[3], img->h) && i >= 0; i--) {
        for (int j = min(2, coords[0], img->w); j < min(2, coords[2], img->w); j++) {
            img->pixels[i][j].r = 255 - img->pixels[i][j].r;
            img->pixels[i][j].g = 255 - img->pixels[i][j].g;
            img->pixels[i][j].b = 255 - img->pixels[i][j].b;
        }
    }
}


void makeGrayscale(Image *img, int *coords, char *algorithm) {
    printf("Making black and white rectangle by algorithm %s...\n", algorithm);
    for (int i = img->h - min(2, coords[1], img->h) - 1; i >= img->h - min(2, coords[3], img->h) && i >= 0; i--) {
        for (int j = min(2, coords[0], img->w); j < min(2, coords[2], img->w); j++) {
            int value;
            int r = img->pixels[i][j].r;
            int g = img->pixels[i][j].g;
            int b = img->pixels[i][j].b;
            if (!strcmp(algorithm, "avg")) {
                value = (r + g + b) / 3;
            } else if (!strcmp(algorithm, "lightness")) {
                value = max(3, r, g, b);
                value += min(3, r, g, b);
                value /= 2;
            } else if (!strcmp(algorithm, "luminosity")) {
                value = r * 0.21 + g * 0.72 + b * 0.07;
            } else {
                fprintf(stderr, "Incorrect algorithm %s.\n", algorithm);
                exitFree(img);
            }
            img->pixels[i][j].r = value;
            img->pixels[i][j].g = value;
            img->pixels[i][j].b = value;
        }
    }
}


Rgb **_expandImg(Image *img, int oldW, int oldH, Rgb color, int padding[2]) {
    Rgb **newPixs = malloc(img->h * sizeof(Rgb *));
    for (int i = 0; i < img->h; i++) {
        newPixs[i] = malloc(img->w * sizeof(Rgb) + (img->w * 3) % 4);
        // До отступа заполняем нужным цветом
        for (int j = 0; j < padding[0]; j++) {
            newPixs[i][j] = color;
        }
        int start = 0;
        // Копируем, если находимся в зоне картинки
        if (i - padding[1] < oldH && i >= padding[1]) {
            for (int j = padding[0]; j < oldW + padding[0]; j++) {
                newPixs[i][j] = img->pixels[i - padding[1]][j - padding[0]];
            }
            start = oldW + padding[0];
        } 
        // После картинки заполняем нужным цветом
        for (int j = start; j < img->w; j++) {
            newPixs[i][j] = color;
        }
    }
    
    return newPixs;
}


Rgb **_cutImg(Image *img, int oldW, int oldH, Rgb color, int padding[2]) {
    Rgb **newPixs = malloc(img->h * sizeof(Rgb *));
    for (int i = 0; i < img->h; i++) {
        newPixs[i] = malloc(img->w * sizeof(Rgb) + (img->w * 3) % 4);
        int start = 0;
        // Если находимся в зоне картинки, копируем нужную часть
        // Иначе заполняем цветом
        if (i - padding[1] < oldH && i >= padding[1]) {
            for (int j = padding[0]; j < img->w + padding[0]; j++) {
                newPixs[i][j - padding[0]] = img->pixels[i - padding[1]][j];
            }
        } else {
            for (int j = 0; j < img->w; j++) {
                newPixs[i][j] = color;
            }
        }
    }
    return newPixs;
}


void resizeImg(Image *img, int width, int height, char *colorStr, char *position) {
    printf("Resizing the image to %dx%d...\n", width, height);
    int oldH = img->h;
    int oldW = img->w;
    
    img->w = width;
    img->h = height;
    Rgb color = getColor(colorStr, img);
    
    // Отступ относительно текущего изображения
    int padding[2] = {0};
    if (!strcmp(position, "rb")) {
        padding[0] = abs(img->w - oldW);
    } else if (!strcmp(position, "lt")) {
        padding[1] = img->h - oldH;
    } else if (!strcmp(position, "rt")) {
        padding[0] = abs(img->w - oldW);
        padding[1] = img->h - oldH;
    } else if (!strcmp(position, "c")) {
        padding[0] = abs(img->w - oldW) / 2;
        padding[1] = (img->h - oldH) / 2;
    } else if (strcmp(position, "lb")) {
        fprintf(stderr, "Unknown position: %s.\n", position);
        exitFree(img);
    }

    Rgb **newPixs;
    if (oldW > img->w) {
        newPixs = _cutImg(img, oldW, oldH, color, padding);
    } else {
        newPixs = _expandImg(img, oldW, oldH, color, padding);
    }
    
    // Удаляем, чтобы не было утечек
    if (oldH > img->h) {
        for (int i = img->h; i < oldH; i++) {
            free(img->pixels[i]);
        }
    }
    img->pixels = realloc(img->pixels, img->h * sizeof(Rgb *));
    for (int i = 0; i < img->h; i++) {
		Rgb *tmp = realloc(img->pixels[i], img->w * sizeof(Rgb) + (img->w * 3) % 4);
        if (!tmp) {
            fprintf(stderr, "You have written too big size of new image!\n");
            // Не забываем очистить память в случае ошибки
            for (int j = i; j < img->h; j++) {
                free(newPixs[j]);
            }
            free(newPixs);
            exitFree(img);
        }
        img->pixels[i] = tmp;
        for (int j = 0; j < img->w; j++) {
            img->pixels[i][j] = newPixs[i][j];
        }
        free(newPixs[i]);
    }
    free(newPixs);
}


void drawLine(Image *img, int *start, int *end, int thickness, char *colorStr) {
    puts("Drawing the line...");
    start[1] = img->h - min(2, start[1], img->h) - 1;
    end[1] = img->h - min(2, end[1], img->h) - 1;
    // По алгоритму Брезенхема смещение относительно осей
    int deltaX = abs(end[0] - start[0]);
    int deltaY = abs(end[1] - start[1]);
    int signX = start[0] < end[0] ? 1 : -1;
    int signY = start[1] < end[1] ? 1 : -1;
    int error = deltaX - deltaY;
    Rgb color = getColor(colorStr, img);
    setPixels(img, end[0], end[1], thickness, color);
    while (start[0] != end[0] || start[1] != end[1]) {
        setPixels(img, start[0], start[1], thickness, color);
        int error2 = error * 2;
        if (error2 > -deltaY) {
            error -= deltaY;
            start[0] += signX;
        }
        if (error2 < deltaX) {
            error += deltaX;
            start[1] += signY;
        }
    }
}

unsigned char _inRound(int x0, int y0, int r, int x1, int y1) {
    return (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) <= r*r;
}
void setPixels(Image *img, int x, int y, int quan, Rgb color) {
    // Закрашиваем, если точка лежит в quan/2 окрестности (x, y)
    for (int i = max(2, y - quan / 2, 0); i < min(2, y + quan / 2 + 1, img->h); i++) {
        for (int j = max(2, x - quan / 2, 0); j < min(2, x + quan / 2 + 1, img->w); j++) {
            if (_inRound(x,y,quan/2,j,i)) {
                img->pixels[i][j] = color;
            } 
        }
    }
}





Rgb getColor(char *color, Image *img) {
    // Если пользователь ввел вместо цвета пробелы, будет черный
    int r=0, g=0, b=0;
    // FFFFFF <=> FFF
    if (strlen(color) == 6) {
        sscanf(color, "%02x%02x%02x", &r, &g, &b);
    } else if (strlen(color) == 3) {
        sscanf(color, "%01x%01x%01x", &r, &g, &b);
        r *= r;
        g *= g;
        b *= b;
    } else {
        fprintf(stderr, "Invalid color string %s.\n", color);
        exitFree(img);
    }
    Rgb res = {b, g, r};
    return res;
}


int min(int cnt,...) {
    va_list ap;
    int i, current, minimum;
    va_start(ap, cnt);
    minimum = INT_MAX;
    for (i = 0; i < cnt; i++){
        current = va_arg(ap, int);
        if (current < minimum)
            minimum = current;
    }
    va_end(ap);
    return minimum;
}

int max(int cnt,...) {
    va_list ap;
    int i, current, maximum;
    va_start(ap, cnt);
    maximum = INT_MIN;
    for (i = 0; i < cnt; i++){
        current = va_arg(ap, int);  
        if (current > maximum)
            maximum = current;
    }
    va_end(ap);
    return maximum;
}
