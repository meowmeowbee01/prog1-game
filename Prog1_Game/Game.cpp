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

	DrawGrid();
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
Rectf GetRectFromGridPosition(int row, int column)
{
	return Rectf
	{
		g_GridTopLeft.x + g_Padding + static_cast<float>(column) * g_CellSize + static_cast<float>(column) * g_Margin,
		g_GridTopLeft.y + g_Padding + static_cast<float>(row) * g_CellSize + static_cast<float>(row) * g_Margin,
		g_CellSize,
		g_CellSize
	};
}

void DrawGrid()
{
	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j {0}; j < g_Collumns; ++j)
		{
			SetColor(0.2f, 0.2f, 0.2f);
			FillRect(GetRectFromGridPosition(i, j));
		}
	}
}

#pragma endregion ownDefinitions