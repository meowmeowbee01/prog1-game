#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{

}

void Draw()
{
	ClearBackground(0.f, 0.f, 0.f);


	const float smallestWindowLength {g_WindowWidth < g_WindowHeight ? g_WindowWidth : g_WindowHeight};
	const float padding {smallestWindowLength * 0.05f}; //space between edge of screen and grid
	const float margin {smallestWindowLength * 0.025f}; //space between grid elements
	DrawGrid(padding, margin);
}

void Update(float elapsedSec)
{

}

void End()
{
	// free game resources here
}
#pragma endregion gameFunctions

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key)
{

}

void OnKeyUpEvent(SDL_Keycode key)
{

}

void OnMouseMotionEvent(const SDL_MouseMotionEvent& e)
{

}

void OnMouseDownEvent(const SDL_MouseButtonEvent& e)
{

}

void OnMouseUpEvent(const SDL_MouseButtonEvent& e)
{

}
#pragma endregion inputHandling

#pragma region ownDefinitions
void DrawGrid(const float padding, const float margin)
{
	const float maxCellWidth {(g_WindowWidth - 2.f * padding - static_cast<float>(g_Collumns - 1) * margin) / g_Collumns};
	const float maxCellHeight {(g_WindowHeight - 2.f * padding - static_cast<float>(g_Rows - 1) * margin) / g_Rows};
	const float cellSize {maxCellWidth < maxCellHeight ? maxCellWidth : maxCellHeight};
	const float gridWidth {cellSize * g_Collumns + 2.f * padding + static_cast<float>(g_Collumns - 1) * margin};
	const float gridHeight {cellSize * g_Rows + 2.f * padding + static_cast<float>(g_Rows - 1) * margin};

	float horizontalOffset {0};
	float verticalOffset {0};
	if (g_WindowWidth > g_WindowHeight)
	{
		horizontalOffset = g_WindowWidth * 0.5f - gridWidth * 0.5f;
	}
	else
	{
		verticalOffset = g_WindowHeight * 0.5f - gridHeight * 0.5f;
	}

	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j {0}; j < g_Collumns; ++j)
		{

			const Point2f topLeft
			{
				horizontalOffset + padding + static_cast<float>(j) * cellSize + static_cast<float>(j) * margin,
				verticalOffset + padding + static_cast<float>(i) * cellSize + static_cast<float>(i) * margin
			};

			FillRect(topLeft, cellSize, cellSize);
		}
	}
}
#pragma endregion ownDefinitions