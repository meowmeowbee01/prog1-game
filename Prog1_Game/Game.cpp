#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	InitializePath();

	JumpOverlappingEnemies();
}

void Draw()
{
	ClearBackground(0.f, 0.f, 0.f);

	DrawGrid();
	HighlightHoveredTile();

	DrawEnemies();

	DrawTowers();

	DrawPlayerHealth();
	DrawPlayerActionPoints();
	DrawSelectedTower();
}

void Update(float elapsedSec)
{

}

void End()
{
	FreeResources();
}
#pragma endregion

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key)
{
	switch (key)
	{
	case SDL_KeyCode::SDLK_SPACE:
		AdvanceTurn();
		break;
	case SDL_KeyCode::SDLK_u:
		UpgradeTower();
		break;
	case SDLK_F3:
		g_ActionPoints = 100;
		break;
	default:
		break;
	}
}

void OnKeyUpEvent(SDL_Keycode key)
{
	ChangeTowerType(key);
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
	if (e.button == 3)
	{
	}
}
#pragma endregion

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

bool IsTileFree(TileIndex tileIndex)
{
	if (g_Grid[tileIndex.row][tileIndex.column].state == TileState::empty)
	{
		return true;
	}
	return false;
}

bool IsTargetTileInRange(const Tower& tower)
{
	if (abs(tower.gridPosition.row - g_HoveredTile.row) > tower.range ||
		abs(tower.gridPosition.column - g_HoveredTile.column) > tower.range)
	{
		return false;
	}
	return true;
}

bool SetDefaultTargetTile(Tower& tower)
{
	const TileIndex startTile {tower.gridPosition};
	const int range {tower.range};

	for (int rowIndex {startTile.row - range}; rowIndex <= startTile.row + range; ++rowIndex)
	{
		if (rowIndex > g_Rows - 1 || rowIndex < 0) continue;
		for (int columnIndex {startTile.column - range}; columnIndex <= startTile.column; ++columnIndex)
		{
			if (columnIndex > g_Columns - 1 || columnIndex < 0) continue;

			if (g_Grid[rowIndex][columnIndex].state == TileState::empty ||
				g_Grid[rowIndex][columnIndex].state == TileState::tower) continue;

			tower.targetTile = TileIndex {rowIndex, columnIndex};
			return true;
		}
	}
	return false;
}

bool TileHasEnemy(int pathIndex)
{
	const TileIndex pathTile {g_PathIndices.at(pathIndex)};

	for (const Enemy& enemy : g_Enemies)
	{
		if (IsOnSameTile(g_PathIndices.at(enemy.pathIndex), pathTile)) return true;
	}
	return false;
}

size_t GetSelectedTower()
{
	for (size_t i = 0; i < g_Towers.size(); i++)
	{
		if (g_Towers.at(i).isSelected == false) continue;
		return i;
	}
	return -1; //this will crash but it's never called in a way that this is possible
}

bool CanAfford(int price)
{
	return price <= g_ActionPoints;
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
			std::cout << "Error loading enemy texture\n";
		}
	}

	const std::string GrassPath {"Resources/Grass.jpg"};
	if (!TextureFromFile(GrassPath, g_GrassTexture))
	{
		std::cout << "Error loading grass texture\n";
	}

	for (int i {0}; i < g_NumberOfPathTextures; ++i)
	{
		const std::string pathPath {g_PathPath + std::to_string(i) + ".png"};
		TextureFromFile(pathPath, g_PathTextures[i]);
	}

	if (!TextureFromFile("Resources/Hovered_Tile.png", g_HoveredTileTexture))
	{
		std::cout << "Error loading marker texture\n";
	}

	for (int i {0}; i <= g_MaxLevel; ++i)
	{
		std::string lightningTowerPath {g_LightningTowerPath + std::to_string(i) + ".png"};
		if (!TextureFromFile(lightningTowerPath, g_LightningTowerSprites[i]))
		{
			std::cout << "Error loading lightning tower texture\n";
		}

		std::string fireTowerPath {g_FireTowerPath + std::to_string(i) + ".png"};
		if (!TextureFromFile(fireTowerPath, g_FireTowerSprites[i]))
		{
			std::cout << "Error loading fire tower texture\n";
		}
	}

	TextureFromFile("Resources/CrossHair2.png", g_CrosshairSprite);

	TextureFromFile("Resources/heart_red.png", g_HeartSprite);

	TextureFromFile("Resources/EnergyTexture.png", g_ActionPointSprite);
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

		g_PathIndices.push_back(TileIndex {row, column});

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
#pragma endregion

#pragma region draw

void DrawTile(TileIndex gridIndex)
{
	enum class PathTextureIndeces
	{
		horizontal,
		vertical,
		topLeft,
		topRight,
		bottomLeft,
		bottomRight
	};

	switch (g_Grid[gridIndex.row][gridIndex.column].state)
	{
	case TileState::tower:
	case TileState::empty:
		DrawTexture(g_GrassTexture, GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathHorizontal:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::horizontal)], GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathVertical:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::vertical)], GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathTopLeft:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::topLeft)], GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathTopRight:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::topRight)], GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathBottomleft:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::bottomLeft)], GetRectFromGridPosition(gridIndex));
		break;
	case TileState::pathBottomRight:
		DrawTexture(g_PathTextures[static_cast<int>(PathTextureIndeces::bottomRight)], GetRectFromGridPosition(gridIndex));
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
			DrawTile(TileIndex {rowIndex, columnIndex});
		}
	}
}

void DrawEnemies()
{
	for (const Enemy& enemy : g_Enemies)
	{
		switch (enemy.enemyType)
		{
		case EnemyType::goober:
			DrawTexture(g_EnemySprites[0], GetRectFromGridPosition(g_PathIndices.at(enemy.pathIndex)));
			break;
		case EnemyType::angryGoober:
			DrawTexture(g_EnemySprites[1], GetRectFromGridPosition(g_PathIndices.at(enemy.pathIndex)));
			break;
		default:
			break;
		}
		DrawEnemyHealth(enemy);
	}
}

void DrawEnemyHealth(const Enemy& enemy)
{
	const Rectf enemyRect {GetRectFromGridPosition(g_PathIndices.at(enemy.pathIndex))};
	const float widthPerHealth {enemyRect.width / enemy.maxHealth};
	const float currentHealthWidth {widthPerHealth * enemy.health};
	const float healthBarHeight {enemyRect.height / 10};
	Rectf healthBar
	{
		enemyRect.left,
		enemyRect.top,
		currentHealthWidth,
		healthBarHeight
	};

	SetColor(1.f, 0.f, 0.f);
	FillRect(healthBar);
}

void DrawTowers()
{
	for (size_t towerIndex {0}; towerIndex < g_Towers.size(); ++towerIndex)
	{
		const Tower tower {g_Towers.at(towerIndex)};
		DrawTower(tower);

		if (tower.isSelected)
		{
			HighlightTargetTile(tower.targetTile);

			SetColor(1.f, 0.f, 0.f);
			DrawRect(GetRectFromGridPosition(tower.gridPosition));

			int range {tower.range};
			DrawRange(towerIndex, range);
		}
	}
}

void DrawTower(const Tower& tower)
{
	DrawTower(tower.towerType, tower.level, GetRectFromGridPosition(tower.gridPosition));
}

void DrawTower(const TowerType towerType, const int towerLevel, const Rectf& destinationRect)
{
	switch (towerType)
	{
	case TowerType::lightning:
		DrawLightningTower(towerLevel, destinationRect);
		break;
	case TowerType::fire:
		DrawFireTower(towerLevel, destinationRect);
		break;
	default:
		break;
	}
}

void DrawFireTower(const int level, const Rectf& destinationRect)
{
	DrawTexture(g_FireTowerSprites[level], destinationRect);
}

void DrawLightningTower(const int level, const Rectf& destinationRect)
{
	DrawTexture(g_LightningTowerSprites[level], destinationRect);
}

void DrawRange(size_t towerIndex, int range)
{
	const TileIndex topLeftTile {g_Towers.at(towerIndex).gridPosition.row - range, g_Towers.at(towerIndex).gridPosition.column - range};
	const TileIndex bottomRightTile {g_Towers.at(towerIndex).gridPosition.row + range, g_Towers.at(towerIndex).gridPosition.column + range};
	const Rectf topLeftRange
	{
		GetRectFromGridPosition(topLeftTile)
	};
	const Rectf bottomRightRange
	{
		GetRectFromGridPosition(bottomRightTile)
	};

	Rectf rangeRect
	{
		topLeftRange.left,
		topLeftRange.top,
		bottomRightRange.left - topLeftRange.left + topLeftRange.width,
		bottomRightRange.top + bottomRightRange.height - topLeftRange.top
	};
	Rectf gridTopLeft {GetRectFromGridPosition(TileIndex {0, 0})};
	if (rangeRect.top < gridTopLeft.top)
	{
		const float difference = gridTopLeft.top - rangeRect.top;
		rangeRect.top = gridTopLeft.top;
		rangeRect.height -= difference;
	}
	if (rangeRect.left < gridTopLeft.left)
	{
		const float difference = gridTopLeft.left - rangeRect.left;
		rangeRect.left = gridTopLeft.left;
		rangeRect.width -= difference;
	}

	DrawRect(rangeRect);
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

void DrawPlayerHealth()
{
	const int verticalOffset {-2};
	for (int i {0}; i < g_PlayerHealth; ++i)
	{
		const Rectf heartPosition {GetRectFromGridPosition(TileIndex{verticalOffset, i})};

		DrawTexture(g_HeartSprite, heartPosition);
	}
}

void DrawPlayerActionPoints()
{
	const int verticalOffset {-2};
	for (int i {0}; i < g_ActionPoints; ++i)
	{
		const Rectf position {GetRectFromGridPosition(TileIndex{verticalOffset, (g_Columns - 1) - i})};

		DrawTexture(g_ActionPointSprite, position);
	}

	if (g_ActionPointProgress == 0 || g_ActionPoints == g_MaxActionPoints) return;

	const float sourceRectHeight {g_ActionPointSprite.height / g_ActionPointGenerationThreshhold * g_ActionPointProgress};
	const Rectf sourceRect
	{
		0.f,
		0.f,
		g_ActionPointSprite.width,
		sourceRectHeight
	};

	const int unfinishedPointColumn {g_ActionPoints}; //left of the last full AP
	const Rectf fullTilePosition {GetRectFromGridPosition(TileIndex{verticalOffset, (g_Columns - 1) - unfinishedPointColumn})};

	const Rectf adjustedPosition
	{
		fullTilePosition.left,
		fullTilePosition.top,
		fullTilePosition.width,
		fullTilePosition.height / g_ActionPointGenerationThreshhold * g_ActionPointProgress
	};

	DrawTexture(g_ActionPointSprite, adjustedPosition, sourceRect);
}

void DrawSelectedTower()
{
	const int verticalOffset {-3};
	const Rectf previewPosition {GetRectFromGridPosition(TileIndex{verticalOffset, g_Columns - 1})};
	DrawTower(g_SelectedTowerType, 0, previewPosition);
}
#pragma endregion

#pragma region gameLogic

void AdvanceTurn()
{
	AdvanceEnemies();
	HandleReachedGoalEnemies();

	SpawnEnemies();
	KeepJumpingOverlappingAndHandleReachedGoal();

	ActivateTowerEffects();
	ApplyBurnDamage();

	HandleDeadEnemies();

	AddActionPoints();
	++g_TurnCounter;
}

void AdvanceEnemies()
{
	for (Enemy& enemy : g_Enemies)
	{
		enemy.pathIndex += enemy.speed;
	}
}

void SpawnEnemies()
{
	const int startPathIndex {0};

	const int gooberMaxHealth {4};
	const int angryGooberMaxHealth {8};

	const int burnStacks {0};

	const int speed {1};
	const int angrySpeed {2};

	float spawnChance {0.05f};
	if (g_Enemies.empty()) spawnChance = 1.f;
	const float angryChance {g_TurnCounter / 1000.f};

	if (RandomDecimal() < spawnChance)
	{
		const int batchSize {RandomIntInRange(1,15)};

		for (int i {0}; i < batchSize; ++i)
		{
			if (RandomDecimal() < angryChance)
			{
				Enemy newEnemy {EnemyType::angryGoober, startPathIndex, angryGooberMaxHealth, angryGooberMaxHealth, burnStacks, angrySpeed};
				g_Enemies.push_back(newEnemy);
			}
			else
			{
				Enemy newEnemy {EnemyType::goober, startPathIndex, gooberMaxHealth, gooberMaxHealth, burnStacks, speed};
				g_Enemies.push_back(newEnemy);
			}
		}
	}
}

void KeepJumpingOverlappingAndHandleReachedGoal()
{
	bool needsToJump {true};
	while (needsToJump)
	{
		needsToJump = JumpOverlappingEnemies();
		HandleReachedGoalEnemies();
	}
}

bool JumpOverlappingEnemies()
{
	bool hasJumped {false};

	for (int i = static_cast<int>(g_PathIndices.size()) - 1; i >= 0; --i)
	{
		for (Enemy& enemy : g_Enemies)
		{
			if (enemy.pathIndex != i) continue;
			if (JumpIfOverlapping(enemy)) hasJumped = true;
		}
	}

	return hasJumped;
}

bool JumpIfOverlapping(Enemy& enemy)
{
	for (const Enemy& otherEnemy : g_Enemies) //compare with every other enemy
	{
		if (&enemy == &otherEnemy) continue; //not itself
		if (enemy.pathIndex != otherEnemy.pathIndex) continue; //not the same position
		if (enemy.speed < otherEnemy.speed) continue; //let the faster enemy jump first
		enemy.pathIndex += enemy.speed;
		return true;
	}
	return false;
}

void HandleReachedGoalEnemies()
{
	for (const Enemy& enemy : g_Enemies)
	{
		if (enemy.pathIndex < g_PathIndices.size()) continue;
		--g_PlayerHealth;
	}

	std::erase_if(
		g_Enemies,
		[](const Enemy& enemy) -> bool
		{
			return enemy.pathIndex >= g_PathIndices.size();
		}
	);

	if (g_PlayerHealth <= 0)
	{
		std::cout << "GAME OVER NOOB!\n";
		//TODO: Add proper game over
	}
}

void ActivateTowerEffects()
{
	for (const Tower& tower : g_Towers)
	{
		for (Enemy& enemy : g_Enemies)
		{
			if (!IsOnSameTile(g_PathIndices.at(enemy.pathIndex), tower.targetTile)) continue;

			switch (tower.towerType)
			{
			case TowerType::lightning:
				LightningChainDamage(enemy, tower.level);
				break;
			case TowerType::fire:
				FireTowerDamage(enemy, tower.level);
				break;
			}
		}
	}
}

void LightningChainDamage(Enemy& enemy, int towerLevel)
{
	int pathIndex {enemy.pathIndex};
	const int maxChain {(towerLevel + 1) * 3};
	int currentChain {0};
	int towerDamage {towerLevel + 1};

	enemy.health -= towerDamage;

	while (TileHasEnemy(pathIndex) && currentChain < maxChain)
	{
		for (Enemy& chainEnemy : g_Enemies) //find enemy behind hit enemy
		{
			if (chainEnemy.pathIndex != pathIndex - 1) continue;
			--chainEnemy.health;
		}
		--pathIndex;
		++currentChain;
	}
}

void FireTowerDamage(Enemy& enemy, int towerLevel)
{
	int towerDamage {towerLevel + 1};

	enemy.health -= towerDamage;
	enemy.burnStacks += towerLevel;
}

void ApplyBurnDamage()
{
	for (Enemy& enemy : g_Enemies)
	{
		if (enemy.burnStacks <= 0) continue;

		--enemy.health;
		--enemy.burnStacks;
	}
}

void HandleDeadEnemies()
{
	std::erase_if(
		g_Enemies,
		[](const Enemy& enemy) -> bool
		{
			return enemy.health <= 0;
		}
	);
}

void PlaceTower()
{
	if (!UpdateHoveredTile()) return;
	if (!IsTileFree(g_HoveredTile)) return;

	switch (g_SelectedTowerType)
	{
	case TowerType::lightning:
		if (!CanAfford(g_LightningTowerCost)) break;
		PlaceLightningTower();
		g_Grid[g_HoveredTile.row][g_HoveredTile.column].state = TileState::tower;
		break;
	case TowerType::fire:
		if (!CanAfford(g_FireTowerCost)) break;
		PlaceFireTower();
		g_Grid[g_HoveredTile.row][g_HoveredTile.column].state = TileState::tower;
		break;
	default:
		break;
	}
}

void PlaceLightningTower()
{
	const bool selected {true};
	const int level {0};
	Tower defaultLightningTower {TowerType::lightning, g_HoveredTile, g_PathIndices.at(0), selected, g_LightningTowerRange, level};
	if (!SetDefaultTargetTile(defaultLightningTower))
	{
		defaultLightningTower.targetTile = defaultLightningTower.gridPosition;
	}
	g_Towers.push_back(defaultLightningTower);

	g_ActionPoints -= g_LightningTowerCost;
}

void PlaceFireTower()
{
	const bool selected {true};
	const int level {0};
	Tower defaultFireTower {TowerType::fire, g_HoveredTile, g_PathIndices.at(0), selected, g_FireTowerRange, level};
	if (!SetDefaultTargetTile(defaultFireTower))
	{
		defaultFireTower.targetTile = defaultFireTower.gridPosition;
	}
	g_Towers.push_back(defaultFireTower);

	g_ActionPoints -= g_FireTowerCost;
}

void AddActionPoints()
{
	g_ActionPointProgress += g_ActionPointGrowth;

	if (g_ActionPointProgress == g_ActionPointGenerationThreshhold && g_ActionPoints < g_MaxActionPoints)
	{
		++g_ActionPoints;
		g_ActionPointProgress = 0;
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

	for (int i {0}; i <= g_MaxLevel; ++i)
	{
		DeleteTexture(g_LightningTowerSprites[i]);
		DeleteTexture(g_FireTowerSprites[i]);
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
	const int selectionCost {1};
	if (!IsPath(g_Grid[g_HoveredTile.row][g_HoveredTile.column].state)) return;
	if (g_Towers.at(towerIndex).isSelected == false) return;
	if (!IsTargetTileInRange(g_Towers.at(towerIndex))) return;
	if (!CanAfford(selectionCost)) return;

	g_Towers.at(towerIndex).targetTile = g_HoveredTile;
}

void UpgradeTower()
{
	if (g_Towers.at(GetSelectedTower()).level >= g_MaxLevel) return;
	++g_Towers.at(GetSelectedTower()).level;
}

void ChangeTowerType(SDL_Keycode key)
{
	switch (key)
	{
	case SDLK_1:
		g_SelectedTowerType = TowerType::lightning;
		break;
	case SDLK_2:
		g_SelectedTowerType = TowerType::fire;
		break;
	}
}
#pragma endregion
#pragma endregion