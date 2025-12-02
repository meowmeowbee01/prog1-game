#pragma once
#include "structs.h"

int RandomIntInRange(const int min, const int max);
float RandomDecimal();
float RandomFloatInRange(const float min, const float max);

float Magnitude(const Point2f& vector);
Point2f Difference(const Point2f& pointA, const Point2f& pointB);
float Distance(const Point2f& pointA, const Point2f& pointB);
Point2f NormalizeOrZero(Point2f vector);
Point2f Rotate(const Point2f& vector, float radians);

bool InRange(float number, float min, float max);
bool InArea(const Point2f& point, const Point2f& bottomLeft, const Point2f& topRight);

void PrintElements(const int array[], int arraySize);
void PrintElements(const int array[], int startIdx, int endIdx);
int GetGridIndex(int rowIdx, int colIdx, int nrCols);
int GetRow(int index, int numCols);
int GetCol(int index, int numCols);
void VisualizeGridArray(int array[], int numCols, int numRows);
void AssignRandomValues(int array[], int arraySize, int min, int max);
int Count(int array[], int arraySize, int compare);
int MinElement(int array[], int arraySize);
int MaxElement(int array[], int arraySize);
void Swap(int array[], int idx1, int idx2);
void Shuffle(int array[], int arraySize, int swaps);
void BubbleSort(int array[], int arraySize, int iteration = 0);
#pragma once
