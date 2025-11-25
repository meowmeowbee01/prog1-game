#include "pch.h"
#include <iostream>
#include "MyUtils.h"

#pragma region Random
int RandomIntInRange(const int min, const int max)
{
    const int difference = max - min;
    return rand() % (difference + 1) + min;
}

float RandomDecimal()
{
    return static_cast<float>(std::abs(rand())) / static_cast<float>(RAND_MAX);
}

float RandomFloatInRange(const float min, const float max)
{
    const float difference = max - min;
    return RandomDecimal() * difference + min;
}
#pragma endregion Random

#pragma region Vectors
float Magnitude(const Point2f& vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

Point2f Difference(const Point2f& pointA, const Point2f& pointB)
{
    return Point2f{ pointA.x - pointB.x, pointA.y - pointB.y };
}

float Distance(const Point2f& pointA, const Point2f& pointB)
{
    return Magnitude(Difference(pointA, pointB));
}

Point2f NormalizeOrZero(Point2f vector)
{
    float magnitude = Magnitude(vector);
    if (magnitude == 0.f) return vector;
    vector.x /= magnitude;
    vector.y /= magnitude;
    return vector;
}

Point2f Rotate(const Point2f& vector, float radians)
{
    return Point2f
    {
        vector.x * cosf(radians) - vector.y * sinf(radians),
        vector.x * sinf(radians) + vector.y * cosf(radians)
    };
}
#pragma endregion Vectors

#pragma region collision
bool InRange(float number, float min, float max)
{
    return number < max && number > min;
}

bool InArea(const Point2f& point, const Point2f& bottomLeft, const Point2f& topRight)
{
    return InRange(point.x, bottomLeft.x, topRight.x) && InRange(point.y, bottomLeft.y, topRight.y);
}
#pragma endregion collision

#pragma region arrays

void PrintElements(const int array[], int startIdx, int endIdx)
{
    for (int i = startIdx; i < endIdx + 1; ++i)
    {
        std::cout << array[i] << ' ';
    }
}

void PrintElements(const int array[], int arraySize)
{
    PrintElements(array, 0, arraySize - 1);
}

int GetGridIndex(int rowIdx, int colIdx, int nrCols)
{
    return colIdx + rowIdx * nrCols;
}

int GetRow(int index, int numCols)
{
    return index / numCols;
}

int GetCol(int index, int numCols)
{
    return index % numCols;
}

void VisualizeGridArray(int array[], int numCols, int numRows)
{
    const char space{ ' ' };

    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            std::cout << array[j + numCols * i] << space;
        }
        std::cout << std::endl;
    }
}

void AssignRandomValues(int array[], int arraySize, int min, int max)
{
	for (int i = 0; i < arraySize; ++i)
	{
		array[i] = RandomIntInRange(min, max);
	}
}

int Count(int array[], int arraySize, int compare)
{
	int equal{ 0 };
	for (int i = 0; i < arraySize; ++i)
	{
		if (array[i] != compare) continue;

		equal += 1;
	}
	return equal;
}


int MinElement(int array[], int arraySize)
{
	int min{ };
	for (int i = 0; i < arraySize; i++)
	{
		if (i == 0)
		{
			min = array[i];
		}

		if (array[i] > array[i - 1]) continue;

		min = array[i];
	}
	return min;
}

int MaxElement(int array[], int arraySize)
{
	int max{ };
	for (int i = 0; i < arraySize; i++)
	{
		if (i == 0)
		{
			max = array[i];
		}

		if (array[i] < array[i - 1]) continue;

		max = array[i];
	}
	return max;
}

void Swap(int array[], const int idx1, const int idx2)
{
	const int numberAtIdx1{ array[idx1] };
	const int numberAtIdx2{ array[idx2] };

	array[idx1] = numberAtIdx2;
	array[idx2] = numberAtIdx1;
}

void Shuffle(int array[], int arraySize, int swaps)
{
	for (int i = 0; i < swaps; ++i)
	{
		int idx1{ RandomIntInRange(0, arraySize - 1) };
		int idx2{ RandomIntInRange(0, arraySize - 1) };

		if (idx1 == idx2)
		{
			idx1 == arraySize - 1 ? --idx1 : ++idx1;
		}

		int placeHolder{ };

		placeHolder = array[idx2];
		array[idx2] = array[idx1];
		array[idx1] = placeHolder;
	}
}

void BubbleSort(int array[], int arraySize, int iteration)
{
	if (iteration > arraySize) return;

	for (int i = 0; i < arraySize - 1 - (1 * iteration); ++i)
	{
		if (array[i] > array[i + 1])
		{
			Swap(array, i, i + 1);
		}
	}

	BubbleSort(array, arraySize, iteration + 1);
}
#pragma endregion arrays