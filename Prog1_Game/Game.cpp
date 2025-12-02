#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	InitializePath();

	InitializeTowers();

	EnemyJump();
}


void Draw()
{
	ClearBackground(0.f, 0.f, 0.f);

	DrawGrid();
	HighlightHoveredTile();

	DrawEnemies();

	DrawTowers();
}

void Update(float elapsedSec)
{
	//EnemyJump();
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
	switch (e.button)
	{
	case 1:
		PlaceTower();
	}
}
#pragma endregion inputHandling

#pragma region ownDefinitions
void InitializeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		const std::string enemyPath {g_EnemyPath += std::to_string(i) + ".png"};
		if (!TextureFromFile(enemyPath, g_EnemySprites[i]))
		{
			std::cout << "Error loading enemy texture";
		}
	}

	const std::string GrassPath {"Resources/Grass.jpg"};
	if (!TextureFromFile(GrassPath, g_GrassTexture))
	{
		std::cout << "Error loading grass texture";
	}

	if (!TextureFromFile("Resources/Path.png", g_PathTexture))
	{
		std::cout << "Error loading path texture";
	}

	if (!TextureFromFile("Resources/Hovered_Tile.png", g_HoveredTileTexture))
	{
		std::cout << "Error loading marker texture";
	}

}

void InitializePath()
{
	g_PathIndeces = new TileIndex[g_PathLength];

	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Columns; ++columnIndex)
		{
			g_Grid[rowIndex][columnIndex].state = rowIndex == 5 ? Cellstate::path : Cellstate::empty;

			if (g_Grid[rowIndex][columnIndex].state != Cellstate::path) continue;

			g_PathIndeces[columnIndex] = TileIndex {rowIndex, columnIndex};
		}
	}
}

void InitializeTowers()
{
	const int maxTowers {g_NumberOfEnemies};
	g_Towers.reserve(maxTowers);
}

Rectf GetRectFromGridPosition(TileIndex gridIndex)
{
	return Rectf
	{
		g_GridTopLeft.x + g_Padding + static_cast<float>(gridIndex.column) * g_CellSize + static_cast<float>(gridIndex.column) * g_Margin,
		g_GridTopLeft.y + g_Padding + static_cast<float>(gridIndex.row) * g_CellSize + static_cast<float>(gridIndex.row) * g_Margin,
		g_CellSize,
		g_CellSize
	};
}

void DrawCell(TileIndex gridIndex)
{
	switch (g_Grid[gridIndex.row][gridIndex.column].state)
	{
	case Cellstate::empty:
		DrawTexture(g_GrassTexture, GetRectFromGridPosition(gridIndex));
		break;
	case Cellstate::path:
		DrawTexture(g_PathTexture, GetRectFromGridPosition(gridIndex));
		break;
	}
}

void DrawGrid()
{
	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Columns; ++columnIndex)
		{
			DrawCell(TileIndex {rowIndex, columnIndex});
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

void DrawTowers()
{
	for (size_t i {0}; i < g_Towers.size(); ++i)
	{
		for (int j {0}; j < g_Rows * g_Columns; ++j)
		{
			const TileIndex currentTile {GetRow(j, g_Columns), GetCol(j, g_Columns)};

			if (g_Grid[currentTile.row][currentTile.column].state != Cellstate::tower) continue;

			SetColor(g_GunTowerPlaceHolder);
			FillRect(GetRectFromGridPosition(currentTile));
			
		}
		DrawRect(GetRectFromGridPosition(g_Towers.at(i).TargetTile));
	}
}

void HighlightHoveredTile()
{
	DrawTexture(g_HoveredTileTexture, GetRectFromGridPosition(GetHoveredCell()));
}

bool IsCellFree(TileIndex tileIndex)
{
	if (g_Grid[tileIndex.row][tileIndex.column].state == Cellstate::empty)
	{
		return true;
	}
	return false;
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
	EnemyJump();
}

void PlaceTower()
{
	const TileIndex hoveredCell {GetHoveredCell()};
	if (!IsCellFree(hoveredCell)) return;

	g_Grid[hoveredCell.row][hoveredCell.column].state = Cellstate::tower;
	g_Towers.push_back(Tower {TowerType::gun, hoveredCell, g_PathIndeces[0]});

	//TODO: Remove some kind of ressource (Action Point)
}

void UpdateMousePosition(const SDL_MouseMotionEvent& e)
{
	g_MousePosition.x = static_cast<float>(e.x);
	g_MousePosition.y = static_cast<float>(e.y);
}

TileIndex GetHoveredCell()
{
	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j = 0; j < g_Columns; ++j)
		{
			const TileIndex currentGridIndex {i, j};
			const Rectf currentCell {GetRectFromGridPosition(currentGridIndex)};

			if (!IsPointInRect(g_MousePosition, currentCell)) continue;

			return currentGridIndex;
		}
	}
	return TileIndex {-2, -2};
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
	DeleteTexture(g_PathTexture);
	DeleteTexture(g_HoveredTileTexture);
}

#pragma endregion ownDefinitions