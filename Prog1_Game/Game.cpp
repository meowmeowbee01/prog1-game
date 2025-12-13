#include "pch.h"
#include "Game.h"
#include <iostream>
#include "MyUtils.h"

#pragma region gameFunctions
void Start()
{
	InitializeResources();

	InitializePath();

	InitializeMenuButtons();
}

void Draw()
{
	switch (g_GameState)
	{
	case GameState::startMenu:
		ClearBackground(0.3f, 0.3f, 0.8f);

		DrawStartScreen();
		break;
	case GameState::playing:
		ClearBackground(0.f, 0.f, 0.f);

		DrawGrid();
		HighlightHoveredTile();

		DrawEnemies();

		DrawTowers();

		DrawPlayerHealth();
		DrawPlayerActionPoints();
		DrawSelectedTower();
		break;
	case GameState::gameOver:
		ClearBackground(0.8f, 0.2f, 0.2f);

		DrawGameOverScreen();
		break;
	default:
		break;
	}
}

void Update(float elapsedSec)
{
	UpdateProjectilePositions();

	if (g_GameState == GameState::playing) return;
	UpdateStartScreen();
}

void End()
{
	FreeResources();
}
#pragma endregion

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key)
{
	if (g_GameState != GameState::playing) return;
	if (IsAnimationRunning()) return;
	switch (key)
	{
	case SDLK_SPACE:
		AdvanceTurn();
		break;
	case SDLK_u:
		UpgradeTower();
		break;
	case SDLK_m:
		IncreaseMaxEnergy();
		break;
	case SDLK_1:
		ChangeTowerType(TowerType::lightning);
		break;
	case SDLK_2:
		ChangeTowerType(TowerType::fire);
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
	if (IsAnimationRunning()) return;
	switch (g_GameState)
	{
	case GameState::startMenu:
	case GameState::gameOver:
		if (e.button == 1)
		{
			ClickMenuButton();
		}
		break;
	case GameState::playing:
		if (e.button == 1)
		{
			PlaceTower();
			SelectTower();
		}
		break;
	default:
		break;
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
		for (int columnIndex {startTile.column - range}; columnIndex <= startTile.column + range; ++columnIndex)
		{
			if (columnIndex > g_Columns - 1 || columnIndex < 0) continue;

			const bool TileIsEmpty {g_Grid[rowIndex][columnIndex].state == TileState::empty};
			const bool TileIsTower {g_Grid[rowIndex][columnIndex].state == TileState::empty};

			if (TileIsEmpty || TileIsTower) continue;

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

size_t GetSelectedTowerIndex()
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

	TextureFromFile("Resources/button_wide_idle.png", g_IdleMenuButton);
	TextureFromFile("Resources/button_wide_hover.png", g_HoveredMenuButton);

	const int ptSize {50};
	const Color4f textColor {1.f, 1.f, 1.f, 1.f};
	const std::string fontPath {"Resources/GameFont.ttf"};
	TextureFromString("Start Game", fontPath, ptSize, textColor, g_StartGameText);
	TextureFromString("Quit Game", fontPath, ptSize, textColor, g_QuitGameText);

	TextureFromString(g_GameTitleString, fontPath, ptSize, textColor, g_GameTitle);
	TextureFromString("Game Over", fontPath, ptSize, textColor, g_GameOverText);

	TextureFromFile("Resources/FireBall.png", g_FireballTexture);
	TextureFromFile("Resources/Lightning.png", g_LightningTexture);
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

void InitializeMenuButtons()
{
	Point2f buttonPosition
	{
		g_WindowWidth / 2 - g_ButtonWidth / 2,
		g_WindowHeight / 3
	};

	float buttonDistance {50.f};

	for (int i {0}; i < g_NumberOfStartMenuButtons; ++i)
	{
		g_MenuButtons[i].position =
			Rectf
		{
			buttonPosition.x,
			buttonPosition.y + ((g_ButtonHeight + buttonDistance) * i),
			g_ButtonWidth,
			g_ButtonHeight
		};

		g_MenuButtons[i].texture = g_IdleMenuButton;
	}

	g_MenuButtons[0].type = ButtonType::start;
	g_MenuButtons[1].type = ButtonType::quit;
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
	for (Tower& tower : g_Towers)
	{
		DrawTower(tower);

		if (tower.isSelected)
		{
			HighlightTargetTile(tower.targetTile);

			SetColor(1.f, 0.f, 0.f);
			DrawRect(GetRectFromGridPosition(tower.gridPosition));

			int range {tower.range};
			DrawRange(tower);
		}

		if (!tower.isShooting) continue;
		
		switch (tower.towerType)
		{
		case TowerType::lightning:
 			DrawProjectile(tower.projectilePosition, g_LightningTexture);
			break;
		case TowerType::fire:
			DrawProjectile(tower.projectilePosition, g_FireballTexture);
			break;
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

void DrawRange(const Tower& tower)
{
	const TileIndex topLeftTile {tower.gridPosition.row - tower.range, tower.gridPosition.column - tower.range};
	const TileIndex bottomRightTile {tower.gridPosition.row + tower.range, tower.gridPosition.column + tower.range};
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

void DrawProjectile(Point2f& projectilePosition, const Texture& texture)
{
	const TileIndex genericTileIndex {1, 1};
	Rectf dstRect
	{
		projectilePosition.x,
		projectilePosition.y,
		GetRectFromGridPosition(genericTileIndex).width,
		GetRectFromGridPosition(genericTileIndex).height
	};
	DrawTexture(texture, dstRect);
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

void DrawStartScreen()
{
	for (int i {0}; i < g_NumberOfStartMenuButtons; ++i)
	{
		DrawTexture(g_MenuButtons[i].texture, g_MenuButtons[i].position);
	}

	const Rectf startGameDstRect
	{
		g_MenuButtons[static_cast<int>(ButtonType::start)].position.left + g_MenuButtons[static_cast<int>(ButtonType::start)].position.width * 0.05f,
		g_MenuButtons[static_cast<int>(ButtonType::start)].position.top,
		g_MenuButtons[static_cast<int>(ButtonType::start)].position.width * 0.9f,
		g_MenuButtons[static_cast<int>(ButtonType::start)].position.height * 0.95f
	};
	DrawTexture(g_StartGameText, startGameDstRect);

	const Rectf quitGameDstRect
	{
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.left + g_MenuButtons[static_cast<int>(ButtonType::start)].position.width * 0.05f,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.top,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.width * 0.9f,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.height * 0.95f
	};
	DrawTexture(g_QuitGameText, quitGameDstRect);

	DrawTitle(g_GameTitle);
}

void DrawTitle(const Texture& text)
{
	const float gameTitleWidth {g_WindowWidth * 0.75f};
	const float gameTitleHeight {g_WindowHeight / 7};
	const Rectf gameTitleDstRect
	{
		g_WindowWidth / 2 - gameTitleWidth / 2,
		g_WindowHeight / 10,
		gameTitleWidth,
		gameTitleHeight
	};

	DrawTexture(text, gameTitleDstRect);
}

void DrawGameOverScreen()
{
	DrawTexture(g_MenuButtons[static_cast<int>(ButtonType::quit)].texture, g_MenuButtons[static_cast<int>(ButtonType::quit)].position);

	const Rectf quitGameDstRect
	{
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.left + g_MenuButtons[static_cast<int>(ButtonType::start)].position.width * 0.05f,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.top,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.width * 0.9f,
		g_MenuButtons[static_cast<int>(ButtonType::quit)].position.height * 0.95f
	};
	DrawTexture(g_QuitGameText, quitGameDstRect);

	DrawTitle(g_GameOverText);
}
#pragma endregion

#pragma region gameLogic

void AdvanceTurn()
{
	AdvanceEnemies();
	HandleReachedGoalEnemies();

	SpawnEnemies();
	KeepJumpingOverlappingAndHandleReachedGoal();

	SetTowerAnimationFlag();
	ApplyBurnDamage();

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
	const int minEnemies {5};
	const int maxEnemies {15};
	const int currentEnemies {static_cast<int>(g_Enemies.size())};
	const float spawnChanceNumerator {static_cast<float>(currentEnemies - minEnemies)};
	const float spawnChanceDenominator {static_cast<float>(maxEnemies - minEnemies)};
	const float spawnChance {1.f - (spawnChanceNumerator / spawnChanceDenominator)};

	if (RandomDecimal() < spawnChance)
	{
		const int minBatchSize {5};
		const int maxBatchSize {static_cast<int>(spawnChance * (maxEnemies - minEnemies))};
		if (maxBatchSize < minBatchSize) return;
		const int batchSize {RandomIntInRange(minBatchSize, maxBatchSize)};

		for (int i {0}; i < batchSize; ++i)
		{
			const int startPathIndex {0};
			const int burnStacks {0};

			const float angryChance {g_TurnCounter / 1000.f};

			if (RandomDecimal() < angryChance)
			{
				const int angrySpeed {2};
				const int maxHealth {4};
				Enemy newEnemy {EnemyType::angryGoober, startPathIndex, maxHealth, maxHealth, burnStacks, angrySpeed};
				g_Enemies.push_back(newEnemy);
			}
			else
			{
				const int speed {1};
				const int maximumHealth {4};
				Enemy newEnemy {EnemyType::goober, startPathIndex, maximumHealth, maximumHealth, burnStacks, speed};
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
		g_GameState = GameState::gameOver;
	}
}

void SetTowerAnimationFlag()
{
	for (Tower& tower : g_Towers)
	{
		for (const Enemy& enemy : g_Enemies)
		{
			if (!IsOnSameTile(g_PathIndices.at(enemy.pathIndex), tower.targetTile)) continue;

			tower.isShooting = true;
		}
	}
}

void ActivateTowerEffects(Tower& tower)
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
	enemy.burnStacks += towerDamage;
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
	Point2f projectilePosition
	{
		GetRectFromGridPosition(g_HoveredTile).left,
		GetRectFromGridPosition(g_HoveredTile).top
	};
	Tower defaultLightningTower {TowerType::lightning, g_HoveredTile, g_PathIndices.at(0), selected, g_LightningTowerRange, level, false, projectilePosition};
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
	Point2f projectilePosition
	{
		GetRectFromGridPosition(g_HoveredTile).left,
		GetRectFromGridPosition(g_HoveredTile).top
	};
	Tower defaultFireTower {TowerType::fire, g_HoveredTile, g_PathIndices.at(0), selected, g_FireTowerRange, level, false, projectilePosition};
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

	if (g_ActionPointProgress != g_ActionPointGenerationThreshhold) return;
	g_ActionPointProgress = 0;

	if (g_ActionPoints >= g_MaxActionPoints) return;
	++g_ActionPoints;
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

void UpdateProjectilePositions()
{
	for (Tower& tower : g_Towers)
	{
		if (!tower.isShooting) continue;

		Point2f TargetPosition
		{
			GetRectFromGridPosition(tower.targetTile).left,
			GetRectFromGridPosition(tower.targetTile).top
		};

		Point2f vector {Subtract(TargetPosition, tower.projectilePosition)};

		Point2f normalizedVec {NormalizeOrZero(vector)};

		tower.projectilePosition = Add(tower.projectilePosition, normalizedVec);

		const float margin {1.f};
		if (Distance(tower.projectilePosition, TargetPosition) > margin) continue;

		tower.isShooting = false;

		Point2f towerPosition
		{
			GetRectFromGridPosition(tower.gridPosition).left,
			GetRectFromGridPosition(tower.gridPosition).top
		};
		tower.projectilePosition = towerPosition;
		ActivateTowerEffects(tower);
		HandleDeadEnemies();
	}
}

bool IsAnimationRunning()
{
	for (Tower& tower : g_Towers)
	{
		if (!tower.isShooting) continue;
		return true;
	}
	return false;
}

void UpdateStartScreen()
{
	for (int i {0}; i < g_NumberOfStartMenuButtons; ++i)
	{
		if (!IsPointInRect(g_MousePosition, g_MenuButtons[i].position))
		{
			g_MenuButtons[i].texture = g_HoveredMenuButton;
		}
		else
		{
			g_MenuButtons[i].texture = g_IdleMenuButton;
		}
	}
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
	DeleteTexture(g_HoveredTileTexture);

	for (int i {0}; i <= g_MaxLevel; ++i)
	{
		DeleteTexture(g_LightningTowerSprites[i]);
		DeleteTexture(g_FireTowerSprites[i]);
	}

	for (int i {0}; i < g_NumberOfPathTextures; ++i)
	{
		DeleteTexture(g_PathTextures[i]);
	}

	DeleteTexture(g_CrosshairSprite);
	DeleteTexture(g_HeartSprite);
	DeleteTexture(g_ActionPointSprite);
	DeleteTexture(g_IdleMenuButton);
	DeleteTexture(g_HoveredMenuButton);
	DeleteTexture(g_StartGameText);
	DeleteTexture(g_QuitGameText);
	DeleteTexture(g_GameTitle);
	DeleteTexture(g_GameOverText);
	DeleteTexture(g_FireballTexture);
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
	const int upgradeCost {4};
	if (g_Towers.at(GetSelectedTowerIndex()).level >= g_MaxLevel) return;
	if (g_ActionPoints < upgradeCost) return;
	++g_Towers.at(GetSelectedTowerIndex()).level;
}

void ChangeTowerType(TowerType type)
{
	g_SelectedTowerType = type;
}

void IncreaseMaxEnergy()
{
	if (g_ActionPoints < g_MaxActionPoints) return;

	g_ActionPoints = 0;
	++g_MaxActionPointProgress;

	if (g_MaxActionPointProgress < g_MaxActionPointIncreaseThreshhold) return;

	++g_MaxActionPoints;
	g_MaxActionPointProgress = 0;
}

void ClickMenuButton()
{
	for (int i {0}; i < g_NumberOfStartMenuButtons; ++i)
	{
		if (!IsPointInRect(g_MousePosition, g_MenuButtons[i].position)) continue;

		switch (g_MenuButtons[i].type)
		{
		case ButtonType::start:
			g_GameState = GameState::playing;
			break;
		case ButtonType::quit:
			End();
			std::exit(0);
			break;
		default:
			break;
		}
	}
}
#pragma endregion
#pragma endregion