#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitResources();
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
	FreeResources();
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
void InitResources()
{
	for (int i = 0; i < g_NumEnemyTypes; ++i)
	{
		g_EnemyPath += std::to_string(i) + ".png";
		if (!TextureFromFile(g_EnemyPath, g_EnemySprites[i]))
		{
			std::cout << "Error loading texture";
		}
	}
}

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
	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Collumns; ++columnIndex)
		{
			SetColor(0.2f, 0.2f, 0.2f);
			FillRect(GetRectFromGridPosition(rowIndex, columnIndex));
		}
	}
}

void FreeResources()
{
	for (int i = 0; i < g_NumEnemyTypes; ++i)
	{
		DeleteTexture(g_EnemySprites[i]);
	}
}

#pragma endregion ownDefinitions