#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	InitializePath();
}


void Draw()
{
	ClearBackground(0.f, 0.f, 0.f);

	DrawGrid();

	DrawEnemies();
}

void Update(float elapsedSec)
{

}

void End()
{
	FreeResources();
	delete[] g_PathIndeces;
}
#pragma endregion gameFunctions

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key)
{
	switch (key)
	{
	case SDL_KeyCode::SDLK_SPACE:
		AdvanceTurn();
		break;
	default:
		break;
	}
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

	std::string waterPath {"Resources/Water.jpg"};
	if (!TextureFromFile(waterPath, g_WaterTexture))
	{
		std::cout << "Error loading texture";
	}
}

void InitializePath()
{
	g_PathIndeces = new GridIndex[g_PathLength];

	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j {0}; j < g_Collumns; ++j)
		{
			g_Grid[i][j].state = i == 5 ? Cellstate::path : Cellstate::empty;

			if (g_Grid[i][j].state == Cellstate::path)
			{
				g_PathIndeces[j] = GridIndex {i,j};
			}
		}
	}
}

Rectf GetRectFromGridPosition(GridIndex gridIndex)
{
	return Rectf
	{
		g_GridTopLeft.x + g_Padding + static_cast<float>(gridIndex.column) * g_CellSize + static_cast<float>(gridIndex.column) * g_Margin,
		g_GridTopLeft.y + g_Padding + static_cast<float>(gridIndex.row) * g_CellSize + static_cast<float>(gridIndex.row) * g_Margin,
		g_CellSize,
		g_CellSize
	};
}

void DrawCell(GridIndex gridIndex)
{
	switch (g_Grid[gridIndex.row][gridIndex.column].state)
	{
	case Cellstate::empty:
		DrawTexture(g_WaterTexture, GetRectFromGridPosition(gridIndex));
		break;
	case Cellstate::path:
		FillRect(GetRectFromGridPosition(gridIndex));
		break;
	}
}

void DrawGrid()
{
	SetColor(0.2f, 0.2f, 0.2f);
	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Collumns; ++columnIndex)
		{
			DrawCell(GridIndex {rowIndex, columnIndex});
		}
	}
}

void DrawEnemies()
{
	for (int i {0}; i < g_NumberOfEnemies; ++i)
	{
		switch (g_Enemies[i].enemyType)
		{
		case EnemyType::goober:
			DrawTexture(g_EnemySprites[0], GetRectFromGridPosition(g_PathIndeces[g_Enemies[i].pathIndex]));
		default:
			break;
		}
	}
}

void AdvanceTurn()
{
	for (int i {g_NumberOfEnemies - 1}; i >= 0; --i)
	{
		++g_Enemies[i].pathIndex;
		if (g_Enemies[i].pathIndex >= g_PathLength)
		{
			//enemy cleared the path
		}
	}
}

void FreeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		DeleteTexture(g_EnemySprites[i]);
	}
	DeleteTexture(g_WaterTexture);
}

#pragma endregion ownDefinitions