#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <mpfr.h>

#define MPFR_PRECISION 200
#define MPFR_ROUNDING_MODE MPFR_RNDD
#define ARG_COUNT 5
#define A_VAL (1.0 / 2147483648)

void writePPM(const char* filename, const int xRes, const int yRes, float* values);
float diffSquares(float x, float y);
float diffSquaresMpfr(mpfr_t x, mpfr_t y);
float determinant(float b, float c);
float determinantMpfr(mpfr_t b, mpfr_t c);

int main (int argc, char** argv) {
    if (argc != ARG_COUNT) {
        printf("usage: ./mpfrDifferenceVisualizer [xCenter] [yCenter] [0 for diff of squares, 1 for determinant] [0 for mapping zero rounding, 1 for difference]\n");
        return 1;
    }

    int testSize = 500;
    float xCenter = atof(argv[1]);
    float yCenter = atof(argv[2]);
    int detMode = atoi(argv[3]);
    int diffMode = atoi(argv[4]);
    // printf("%f, %f\n", xCenter, yCenter);
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

            // initialize MPFR values
            mpfr_t xMpfr;
            mpfr_t yMpfr;
            mpfr_init2(xMpfr, MPFR_PRECISION);
            mpfr_init2(yMpfr, MPFR_PRECISION);
            mpfr_set_flt(xMpfr, xFlt, MPFR_ROUNDING_MODE);
            mpfr_set_flt(yMpfr, yFlt, MPFR_ROUNDING_MODE);

            float value = 0;
            float fltVal;
            float mpfrVal;
            if (detMode) {
                fltVal = determinant(xFlt, yFlt);
                mpfrVal = determinantMpfr(xMpfr, yMpfr);
            } 
            else {
                fltVal = diffSquares(xFlt, yFlt);
                mpfrVal = diffSquaresMpfr(xMpfr, yMpfr);
            }
            if (diffMode) {
                value = fltVal - mpfrVal;
            }
            else {
                if (fltVal == 0) {
                    if (mpfrVal > 0) {
                        value = 1.0;
                    }
                    else if (mpfrVal < 0) {
                        value = -1.0;
                    }
                }
            }
    
            values[((2 * testSize) * (2 * testSize - (i + testSize))) + (j + testSize)] = value;
        }
    }

    writePPM("visualizationMpfr.ppm", 2 * testSize, 2 * testSize, values);
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
    printf("Largest magnitude graphed = %.10f\n", maxDiff);

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

float diffSquares(float x, float y) {
    return (x * x) - (y * y);
}

float diffSquaresMpfr(mpfr_t x, mpfr_t y) {
    mpfr_pow_ui(x, x, 2, MPFR_ROUNDING_MODE);
    mpfr_pow_ui(y, y, 2, MPFR_ROUNDING_MODE);
    mpfr_sub(x, x, y, MPFR_ROUNDING_MODE);
    return mpfr_get_flt(x, MPFR_ROUNDING_MODE);
}

float determinant(float b, float c) {
    float a = A_VAL;
    return (b * b) - 4 * (a * c);
}

// compute b^2 - 4ac. Should be erroneous around b = 2, c = 2e31
float determinantMpfr(mpfr_t b, mpfr_t c) {
    mpfr_t a;
    mpfr_t four;
    mpfr_init2(a, MPFR_PRECISION);
    mpfr_init2(four, MPFR_PRECISION);
    mpfr_set_flt(a, A_VAL, MPFR_ROUNDING_MODE);
    mpfr_set_flt(four, 4, MPFR_ROUNDING_MODE);
    mpfr_pow_ui(b, b, 2, MPFR_ROUNDING_MODE);
    mpfr_mul(a, a, c, MPFR_ROUNDING_MODE);
    mpfr_mul(a, a, four, MPFR_ROUNDING_MODE);
    mpfr_sub(a, b, a, MPFR_ROUNDING_MODE);
    return mpfr_get_flt(a, MPFR_ROUNDING_MODE);
}