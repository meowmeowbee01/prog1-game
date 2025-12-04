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

	JumpOverlappingEnemies();
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
	case static_cast<int>(MouseButtons::leftClick):
		PlaceTower();
		break;
	case static_cast<int>(MouseButtons::rightClick):
		SelectTower();
		break;
	}
}
#pragma endregion inputHandling

#pragma region ownDefinitions

bool IsOnSameTile(TileIndex a, TileIndex b)
{
	if (a.row == b.row && a.column == b.column)
	{
		return true;
	}
	return false;
}

void InitializeResources()
{
	for (int i {0}; i < g_NumEnemyTypes; ++i)
	{
		const std::string enemyPath {g_EnemyPath + std::to_string(i) + ".png"};
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
	for (int i {0}; i < g_NumberOfTowerTypes; ++i)
	{
		const std::string gunTowerPath {g_GunTowerPath + std::to_string(i) + ".png"};
		TextureFromFile(gunTowerPath, g_TowerSprites[i]);
	}
	TextureFromFile("Resources/CrossHair2.png", g_CrosshairSprite);
}

void InitializePath()
{
	g_PathIndeces = new TileIndex[g_PathLength];

	for (int rowIndex {0}; rowIndex < g_Rows; ++rowIndex)
	{
		for (int columnIndex {0}; columnIndex < g_Columns; ++columnIndex)
		{
			g_Grid[rowIndex][columnIndex].state = rowIndex == 5 ? TileState::path : TileState::empty;

			if (g_Grid[rowIndex][columnIndex].state != TileState::path) continue;

			g_PathIndeces[columnIndex] = TileIndex {rowIndex, columnIndex};
		}
	}
}

void InitializeTowers()
{
	const int maxTowers {10};
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
	case TileState::tower:
	case TileState::empty:
		DrawTexture(g_GrassTexture, GetRectFromGridPosition(gridIndex));
		break;
	case TileState::path:
		DrawTexture(g_PathTexture, GetRectFromGridPosition(gridIndex));
		break;
	default:
		SetColor(1.f, 0.f, 0.f);
		FillRect(GetRectFromGridPosition(gridIndex));
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
	for (int enemyIndex {0}; enemyIndex < g_Enemies.size(); ++enemyIndex)
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
	for (size_t towerIndex {0}; towerIndex < g_Towers.size(); ++towerIndex)
	{
		for (int tileIndex {0}; tileIndex < g_Rows * g_Columns; ++tileIndex)
		{
			const TileIndex currentTile {GetRow(tileIndex, g_Columns), GetCol(tileIndex, g_Columns)};

			if (g_Grid[currentTile.row][currentTile.column].state != TileState::tower) continue;
			
			DrawTexture(g_TowerSprites[0], GetRectFromGridPosition(currentTile));
		}
		
		if (g_Towers.at(towerIndex).isSelected)
		{
			HighlightTargetTile(g_Towers.at(towerIndex).targetTile);
			
			SetColor(1.f, 0.f, 0.f);
			DrawRect(GetRectFromGridPosition(g_Towers.at(towerIndex).gridPosition));
		}
	}
}

void HighlightTargetTile(TileIndex targetTile)
{
	DrawTexture(g_CrosshairSprite, GetRectFromGridPosition(targetTile));
}

void HighlightHoveredTile()
{
	if (UpdateHoveredTile())
	{
		DrawTexture(g_HoveredTileTexture, GetRectFromGridPosition(g_HoveredTile));
	}
}

bool IsCellFree(TileIndex tileIndex)
{
	if (g_Grid[tileIndex.row][tileIndex.column].state == TileState::empty)
	{
		return true;
	}
	return false;
}

void AdvanceTurn()
{
	for (Enemy& enemy : g_Enemies)
	{
		++enemy.pathIndex;
		if (enemy.pathIndex >= g_PathLength)
		{
			enemy.state = EnemyState::reachedGoal;
		}
	}

	if (RandomIntInRange(0, 1) == 0)
	{
		g_Enemies.push_back(Enemy {});
	}
	JumpOverlappingEnemies();
}

void UpdateMousePosition(const SDL_MouseMotionEvent& e)
{
	g_MousePosition.x = static_cast<float>(e.x);
	g_MousePosition.y = static_cast<float>(e.y);
}

bool UpdateHoveredTile()
{
	for (int i {0}; i < g_Rows; ++i)
	{
		for (int j = 0; j < g_Columns; ++j)
		{
			const TileIndex currentTileIndex {i, j};
			const Rectf currentTile {GetRectFromGridPosition(currentTileIndex)};

			if (!IsPointInRect(g_MousePosition, currentTile)) continue;
			g_HoveredTile = currentTileIndex;
			return true;
		}
	}
	return false;
}

void PlaceTower()
{
	if (!UpdateHoveredTile()) return;
	if (!IsCellFree(g_HoveredTile)) return;

	g_Grid[g_HoveredTile.row][g_HoveredTile.column].state = TileState::tower;
	const bool notSelected {false};
	g_Towers.push_back(Tower {TowerType::gun, g_HoveredTile, g_PathIndeces[0], notSelected});

	//TODO: Remove some kind of ressource (Action Point)
}

void SelectTower()
{
	for (size_t i {0}; i < g_Towers.size(); ++i)
	{
		if (IsOnSameTile(g_HoveredTile, g_Towers.at(i).gridPosition))
		{
			g_Towers.at(i).isSelected = true;
			DeselectOtherTowers(i);
		}
		else
		{
			SelectNewTargetTile(i);
		}
	}
}

void DeselectOtherTowers(size_t selectedTowerIndex)
{
	for (size_t i {0}; i < g_Towers.size(); ++i)
	{
		if (i == selectedTowerIndex) continue;
		g_Towers.at(i).isSelected = false;
	}
}

void SelectNewTargetTile(size_t towerIndex)
{
	for (int i {0}; i < g_PathLength; ++i)
	{
		if (!IsOnSameTile(g_HoveredTile, g_PathIndeces[i])) continue;
		if (g_Towers.at(towerIndex).isSelected == false) continue;

		g_Towers.at(towerIndex).targetTile = g_HoveredTile;
	}
}

void JumpOverlappingEnemies()
{
	const size_t otherEnemies {g_Enemies.size()};
	for (int i {0}; i < otherEnemies; ++i) //keep looping until enemies can not be on the same spot
	{
		for (Enemy& enemy : g_Enemies) //for every enemy
		{
			JumpIfOverlapping(enemy);
		}
	}
}

void JumpIfOverlapping(Enemy& enemy)
{
	for (const Enemy& otherEnemy : g_Enemies) //compare with every other enemy
	{
		if (&enemy == &otherEnemy) continue; //not itself
		if (enemy.pathIndex == otherEnemy.pathIndex)
		{
			++enemy.pathIndex;
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

	for (int i {0}; i < g_NumberOfTowerTypes; ++i)
	{
		DeleteTexture(g_TowerSprites[i]);
	}
}

#pragma endregion ownDefinitions