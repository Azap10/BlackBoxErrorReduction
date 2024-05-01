#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define ARG_COUNT 8
#define BIG_NUMBER 10000000
#define RADIUS 0.5
#define GRANULARITY 0.001

float correctVal(float x, float y, float xErrStart, float xErrEnd, float yErrStart, float yErrEnd, int accuracy);
void findNearestBound(float* retX, float* retY, float x, float y, float xMin, float xMax, float yMin, float yMax, int inside);
float absFlt(float x);
float min(float x, float y);
float max(float x, float y);
float findA(float x, float y, float xErrStart, float xErrEnd, float yErrStart, float yErrEnd);
float property(float x, float y, float a);
float diffSquares(float x, float y);
void writePPM(const char* filename, const int xRes, const int yRes, float* values);

int main(int argc, char** argv) {
  if (argc < ARG_COUNT) {
    printf("usage: ./errorRepair [x val] [y val] [xErr start] [xErr end] [yErr start] [yErr end] [accuracy]\n");
    return 1;
  }
  float x = atof(argv[1]);
  float y = atof(argv[2]);
  float xErrStart = atof(argv[3]);
  float xErrEnd = atof(argv[4]);
  float yErrStart = atof(argv[5]);
  float yErrEnd = atof(argv[6]);
  int accuracy = atoi(argv[7]);

  float xVal;
  float yVal;
  int testSize = RADIUS / GRANULARITY;
  float value;
  float* values = (float*)calloc((4 * testSize * testSize), sizeof(float));
  for (int i = -testSize + 1; i < testSize + 1; i++) {
    yVal = y + ((float) i * (float) GRANULARITY);
    for (int j = -testSize; j < testSize; j++) {
      xVal = x + ((float) j * GRANULARITY);
      value = correctVal(xVal, yVal, xErrStart, xErrEnd, yErrStart, yErrEnd, accuracy);
      values[((2 * testSize) * (2 * testSize - (i + testSize))) + (j + testSize)] = value;
      if (i == 0 && j == 0)
        printf("output for (x,y): %f\n", value);
    }
  }
  writePPM("errorRepair.ppm", 2 * testSize, 2 * testSize, values);

  free(values);
}

float correctVal(float x, float y, float xErrStart, float xErrEnd, float yErrStart, float yErrEnd, int accuracy) {
  // is the input in the correct region? simply return the original function value
  if ((x <= xErrStart || x >= xErrEnd) || (y <= yErrStart || y >= yErrEnd)) {
    return diffSquares(x, y);
  }

  // is the input far from the edges of the error region (no concern for continuity)?
  float a;
  float xEpsilon = (xErrEnd - xErrStart) / accuracy;
  float yEpsilon = (yErrEnd - yErrStart) / accuracy;
  if ((x + xEpsilon < xErrEnd && x - xEpsilon > xErrStart) && (y + yEpsilon < yErrEnd && y - yEpsilon > yErrStart)) {
    a = findA(x, y, xErrStart, xErrEnd, yErrStart, yErrEnd);
    return property(x, y, a);
  }

  // is the input close to the edges of the error region and inside it?
  // find the two points in trusted regions closest to this one (along the axis to the center), average the two.
  // Inside variables correspond to finding bounds on inner bounding box, visa versa also true
  float xInside;
  float yInside;
  float xOutside;
  float yOutside;
  float aInside;
  findNearestBound(&xInside, &yInside, x, y, xErrStart + xEpsilon, xErrEnd - xEpsilon, yErrStart + yEpsilon, yErrEnd - yEpsilon, 0);
  findNearestBound(&xOutside, &yOutside, x, y, xErrStart, xErrEnd, yErrStart, yErrEnd, 1);
  aInside = findA(xInside, yInside, xErrStart, xErrEnd, yErrStart, yErrEnd);
  return (diffSquares(xOutside, yOutside) + property(xInside, yInside, aInside)) / 2;
}

float findA(float x, float y, float xErrStart, float xErrEnd, float yErrStart, float yErrEnd) {
  float x1;
  float x2;
  float y1;
  float y2;
  float a;
  int negativeSign = 0;

  // exponentially increase values of a, switching signs with each increase, to find value for which
  // property will hold for the given (x, y)
  a = 0.1;
  while (1) {
    x1 = -a + x;
    x2 = a + x;
    y1 = -a + y;
    y2 = a + y;
    if ((x1 < xErrStart || x1 > xErrEnd) && (x2 < xErrStart || x2 > xErrEnd) && 
        (y1 < yErrStart || y1 > yErrEnd) && (y2 < yErrStart || y2 > yErrEnd)) 
    {
      return a;  
    }
    else {
      if (negativeSign) {
        a = -2 * a;
        negativeSign = 0;
      }
      else {
        a = -a;
        negativeSign = 1;
      }
    }
  }
}

// find the nearest point to (x, y) on a rectangle defined by xMin, xMax, yMin, yMax
// on a line defined by (x, y) and (xCenter, yCenter);
void findNearestBound(float* retX, float* retY, float x, float y, float xMin, float xMax, float yMin, float yMax, int inside) {
  float xCenter = xMin + (xMax - xMin) / 2;
  float yCenter = yMin + (yMax - yMin) / 2;

  // taking a note out of ray tracing's book:
  float xDir = xCenter - x;
  float yDir = yCenter - y;
  float minTimeX;
  float minTimeY;
  if (xDir != 0) {
    minTimeX = min((xMin - x) / xDir, (xMax - x) / xDir);
  }
  else {
    minTimeX = 0;
  }
  if (yDir != 0) {
    minTimeY = min((yMin - y) / yDir, (yMax - y) / yDir);
  }
  else {
    minTimeY = 0;
  }
  float t = max(minTimeX, minTimeY);

  *retX = x + t * xDir;
  *retY = y + t * yDir;
  return;
}

float absFlt(float x) {
  return x < 0 ? -x : x;
}

float min(float x, float y) {
  return x > y ? y : x;
}

float max(float x, float y) {
  return x > y ? x : y;
}

float property(float x, float y, float a) {
  return (diffSquares(-a + x, y) + diffSquares(a + x, y) + diffSquares(x, -a + y) + diffSquares(x, a + y)) / 4;
}

float diffSquares(float x, float y) {
  return (x * x) - (y * y);
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