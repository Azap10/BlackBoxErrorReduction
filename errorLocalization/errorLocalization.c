#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

void writePPM(const char* filename, const int xRes, const int yRes, float* values);
float func2Error(float x, float y);
float func2Correct(float x, float y);

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: ./differenceVisualizer [xCenter] [yCenter]\n");
        return 1;
    }

    int testSize = 500;
    float xCenter = atof(argv[1]);
    float yCenter = atof(argv[2]);
    printf("%f\n", yCenter);
    float* values = (float*)calloc(4 * testSize * testSize, sizeof(float));

    union castUnion {
        float center;
        int intVal;
    };
    
    union castUnion xCast;
    union castUnion yCast;
    float xFlt;
    float yFlt;

    for (int i = -testSize; i < testSize; i++) {
        yCast.center = yCenter;
        yCast.intVal += i;
        memcpy(&yFlt, &yCast.intVal, sizeof(yFlt));
        for (int j = -testSize; j < testSize; j++) {
            xCast.center = xCenter;
            xCast.intVal += j;
            memcpy(&xFlt, &xCast.intVal, sizeof(xFlt));
            values[((i + testSize) * 2 * testSize) + (j + testSize)] = func2Correct(xFlt, yFlt);
        }
    }

    writePPM("visualization.ppm", 2 * testSize, 2 * testSize, values);
    free(values);
    return 0;
}

void writePPM(const char* filename, const int xRes, const int yRes, float* values) {
    // find threshold for data visualization
    int totalCells = xRes * yRes;
    float maxDiff = 0;
    float colorThreshold = 1;
    float valAbs = 0;
    for (int i = 0; i < totalCells; i++) {
        valAbs = values[i] < 0 ? -values[i] : values[i];
        if (valAbs > maxDiff) {
            maxDiff = valAbs;
        }
    }
    colorThreshold = 255 / maxDiff;
    printf("Largest absolute value difference = %f\n", maxDiff);

    // process values into  pixels
    unsigned char* pixels = (char*)calloc(sizeof(char), 3 * totalCells);
    for (int i = 0; i < totalCells; i++) {
        int pixelIndex = i * 3;
        if (values[i] < 0) {
            pixels[pixelIndex] = round(colorThreshold * values[i]);
        }
        else if (values[i] > 0) {
            pixels[pixelIndex + 1] = round(colorThreshold * values[i]);
        }
    }

    // open ppm    
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("failed to open file\n");
        return;
    }

    // add pixels to ppm
    fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
    fwrite(pixels, 1, totalCells * 3, fp);
    fclose(fp);
    free(pixels);
}

float func2Error(float x, float y) {
    return (x * x) - (y * y); // 3.5, 3.5
    // return exp(x) - log(y); // 2, 1618.177992
}

float func2Correct(float x, float y) {
    return (x - y) * (x + y);
}