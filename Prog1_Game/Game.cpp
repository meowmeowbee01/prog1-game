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
	if (e.button == 1)
	{
		PlaceTower();
		SelectTower();
	}
}
#pragma endregion inputHandling

#pragma region ownDefinitions

#pragma region utils

bool IsPath(TileState tileState)
{
	return tileState == TileState::pathHorizontal
		|| tileState == TileState::pathVertical
		|| tileState == TileState::pathTopLeft
		|| tileState == TileState::pathTopRight
		|| tileState == TileState::pathBottomleft
		|| tileState == TileState::pathBottomRight;
}

bool IsOnSameTile(TileIndex a, TileIndex b)
{
	if (a.row == b.row && a.column == b.column)
	{
		return true;
	}
	return false;
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

bool IsCellFree(TileIndex tileIndex)
{
	if (g_Grid[tileIndex.row][tileIndex.column].state == TileState::empty)
	{
		return true;
	}
	return false;
}
#pragma endregion

#pragma region start

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
	enum class Direction
	{
		forward = 0,
		up = 1,
		down = 2
	};

	int column {0};
	int row {g_Rows / 2};
	Direction direction {};

	while (column < g_Columns)
	{

		Direction newDirection {};
		const bool isTopRow {row == 0};
		const bool isBottomRow {row == g_Rows - 1};
		const bool isGoingDown {direction == Direction::down};
		const bool isGoingUp {direction == Direction::up};

		if (isTopRow && isGoingUp) newDirection = static_cast<Direction>(0);
		else if (isBottomRow && isGoingDown) newDirection = static_cast<Direction>(0);
		else if (isTopRow || isGoingDown) newDirection = static_cast<Direction>(RandomIntInRange(0, 1) * 2);
		else if (isBottomRow || isGoingUp) newDirection = static_cast<Direction>(RandomIntInRange(0, 1));
		else newDirection = static_cast<Direction>(RandomIntInRange(0, 2));

		g_PathIndeces.push_back(TileIndex {row, column});

		if (direction == Direction::forward)
		{
			if (newDirection == Direction::forward)
			{
				g_Grid[row][column].state = TileState::pathHorizontal;
			}
			else if (newDirection == Direction::up)
			{
				g_Grid[row][column].state = TileState::pathTopLeft;
			}
			else //down
			{
				g_Grid[row][column].state = TileState::pathBottomleft;
			}
		}
		else if (direction == Direction::up)
		{
			if (newDirection == Direction::forward)
			{
				g_Grid[row][column].state = TileState::pathBottomRight;
			}
			else //up
			{
				g_Grid[row][column].state = TileState::pathVertical;
			}
		}
		else //down
		{
			if (newDirection == Direction::forward)
			{
				g_Grid[row][column].state = TileState::pathTopRight;
			}
			else //down
			{
				g_Grid[row][column].state = TileState::pathVertical;
			}
		}

		direction = newDirection;

		switch (direction)
		{
		case Direction::forward:
			++column;
			break;
		case Direction::up:
			--row;
			break;
		case Direction::down:
			++row;
			break;
		default:
			break;
		}
	}
}

void InitializeTowers()
{
	const int maxTowers {10};
	g_Towers.reserve(maxTowers);
}
#pragma endregion

#pragma region draw

void DrawCell(TileIndex gridIndex)
{
	switch (g_Grid[gridIndex.row][gridIndex.column].state)
	{
	case TileState::tower:
	case TileState::empty:
		DrawTexture(g_GrassTexture, GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathHorizontal:
	case TileState::pathVertical:
	case TileState::pathTopLeft:
	case TileState::pathTopRight:
	case TileState::pathBottomleft:
	case TileState::pathBottomRight:
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
			DrawTexture(g_EnemySprites[0], GetRectFromGridPosition(g_PathIndeces.at(g_Enemies[enemyIndex].pathIndex)));
			break;
		case EnemyType::angryGoober:
			DrawTexture(g_EnemySprites[1], GetRectFromGridPosition(g_PathIndeces.at(g_Enemies[enemyIndex].pathIndex)));
			break;
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
#pragma endregion

#pragma region gameLogic

void AdvanceTurn()
{
	for (Enemy& enemy : g_Enemies)
	{
		++enemy.pathIndex;
		if (enemy.pathIndex >= g_PathIndeces.size())
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

void PlaceTower()
{
	if (!UpdateHoveredTile()) return;
	if (!IsCellFree(g_HoveredTile)) return;

	g_Grid[g_HoveredTile.row][g_HoveredTile.column].state = TileState::tower;
	const bool notSelected {false};
	g_Towers.push_back(Tower {TowerType::gun, g_HoveredTile, g_PathIndeces.at(0), notSelected});

	//TODO: Remove some kind of ressource (Action Point)
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
#pragma endregion

#pragma region update

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
#pragma endregion

#pragma region end

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
#pragma endregion

#pragma region input

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
	if (!IsPath(g_Grid[g_HoveredTile.row][g_HoveredTile.column].state)) return;
	if (g_Towers.at(towerIndex).isSelected == false) return;

	g_Towers.at(towerIndex).targetTile = g_HoveredTile;
}
#pragma endregion
#pragma endregion ownDefinitions