#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	//InitializePathTiles();

	InitializePath();
}


void Draw()
{
	ClearBackground(0.f, 0.f, 0.f);

	DrawGrid();
	HighlightHoveredTile();

	DrawEnemies();
}

void Update(float elapsedSec)
{
	EnemyJump();
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
	UpdateMousePosition(e);
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

	std::string GrassPath {"Resources/Grass.jpg"};
	if (!TextureFromFile(GrassPath, g_GrassTexture))
	{
		std::cout << "Error loading texture";
	}

	if (!TextureFromFile("Resources/Hovered_Tile.png", g_HoveredTileTexture))
	{
		std::cout << "Error loading texture";
	}
}

void InitializePath()
{
	g_PathIndeces = new GridIndex[g_PathLength];

	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Columns; ++columnIndex)
		{
			g_Grid[rowIndex][columnIndex].state = rowIndex == 5 ? Cellstate::path : Cellstate::empty;

			if (g_Grid[rowIndex][columnIndex].state != Cellstate::path) continue;

			g_PathIndeces[columnIndex] = GridIndex {rowIndex, columnIndex};
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
		DrawTexture(g_GrassTexture, GetRectFromGridPosition(gridIndex));
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
		for (int columnIndex {0}; columnIndex < g_Columns; ++columnIndex)
		{
			DrawCell(GridIndex {rowIndex, columnIndex});
		}
	}
}

void DrawEnemies()
{
	for (int enemyIndex {0}; enemyIndex < g_NumberOfEnemies; ++enemyIndex)
	{
		if (g_Enemies[enemyIndex].state != EnemyState::alive) continue;
		switch (g_Enemies[enemyIndex].enemyType)
		{
		case EnemyType::goober:
			DrawTexture(g_EnemySprites[0], GetRectFromGridPosition(g_PathIndeces[g_Enemies[enemyIndex].pathIndex]));
		default:
			break;
		}
	}
}

void HighlightHoveredTile()
{
	DrawTexture(g_HoveredTileTexture, GetRectFromGridPosition(GetHoveredCell()));
}

void AdvanceTurn()
{
	for (int enemyIndex {g_NumberOfEnemies - 1}; enemyIndex >= 0; --enemyIndex)
	{
		++g_Enemies[enemyIndex].pathIndex;
		if (g_Enemies[enemyIndex].pathIndex >= g_PathLength)
		{
			g_Enemies[enemyIndex].state = EnemyState::reachedGoal;
		}
	}
}

void UpdateMousePosition(const SDL_MouseMotionEvent& e)
{
	g_MousePosition.x = static_cast<float>(e.x);
	g_MousePosition.y = static_cast<float>(e.y);
}

GridIndex GetHoveredCell()
{
	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j = 0; j < g_Columns; ++j)
		{
			const GridIndex currentGridIndex {i, j};
			const Rectf currentCell {GetRectFromGridPosition(currentGridIndex)};

			if (!IsPointInRect(g_MousePosition, currentCell)) continue;

			return currentGridIndex;
		}
	}
}

void EnemyJump()
{
	for (int currentEnemyIndex {0}; currentEnemyIndex < g_NumberOfEnemies; ++currentEnemyIndex)
	{
		for (int j {currentEnemyIndex + 1}; j < g_NumberOfEnemies; ++j)
		{
			if (g_Enemies[currentEnemyIndex].pathIndex != g_Enemies[j].pathIndex) continue;

			g_Enemies[currentEnemyIndex].pathIndex += 1;
		}
	}
}

void FreeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		DeleteTexture(g_EnemySprites[i]);
	}

	DeleteTexture(g_GrassTexture);
}

#pragma endregion ownDefinitions