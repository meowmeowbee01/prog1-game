#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j {0}; j < g_Collumns; ++j)
		{
			g_Grid[i][j].state = i == 5 ? Cellstate::path : Cellstate::empty;

			if (g_Grid[i][j].state == Cellstate::path)
			{
				g_Path[j] = gridIndex {i,j};
			}
		}
	}
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
void InitializeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		std::string enemyPath {g_EnemyPath += std::to_string(i) + ".png"};
		if (!TextureFromFile(enemyPath, g_EnemySprites[i]))
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
			g_Grid[rowIndex][columnIndex].state == Cellstate::path ? SetColor(0.4f, 0.4f, 0.4f) : SetColor(0.2f, 0.2f, 0.2f);
			FillRect(GetRectFromGridPosition(rowIndex, columnIndex));
		}
	}
}

void FreeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		DeleteTexture(g_EnemySprites[i]);
	}
}

#pragma endregion ownDefinitions