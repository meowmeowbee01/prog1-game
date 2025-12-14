#pragma once
#include <utils.h>
#include <vector>

using namespace utils;

#pragma region gameInformation
const std::string g_GameTitleString {"The Goobercide!"};
std::string g_WindowTitle {g_GameTitleString + " - Unholy C {Tim Scheuer, Arno Buyckx} - 1GDE"};

float g_WindowWidth {1000};
float g_WindowHeight {700};
#pragma endregion gameInformation



#pragma region ownDeclarations
#pragma region enumsAndStructs
enum class GameState
{
	startMenu,
	playing,
	gameOver
};
enum class ButtonType
{
	start,
	quit
};
enum class MouseButtons
{
	idle,
	leftClick,
	middleClick,
	rightClick
};
enum class TileState
{
	empty,
	tower,
	pathHorizontal,
	pathVertical,
	pathTopLeft,
	pathTopRight,
	pathBottomleft,
	pathBottomRight
};
enum class EnemyType
{
	goober,
	angryGoober
};
enum class TowerType
{
	lightning,
	fire
};
enum class EnemyAilment
{
	none,
	burnt
};
struct MenuButton
{
	Rectf position;
	Texture texture;
	ButtonType type;
};
struct Tile
{
	TileState state;
};
struct TileIndex
{
	int row;
	int column;
};
struct Enemy
{
	EnemyType enemyType;
	int pathIndex;
	int maxHealth;
	int health;
	int burnStacks;
	int speed;
};
struct Tower
{
	TowerType towerType;
	TileIndex gridPosition;
	TileIndex targetTile;
	bool isSelected;
	int range;
	int level;
	bool isShooting;
	Point2f projectilePosition;
};
#pragma endregion
GameState g_GameState {GameState::startMenu};


const int g_Rows {10};
const int g_Columns {20};
Tile g_Grid[g_Rows][g_Columns] {};
std::vector<TileIndex> g_PathIndices {};

std::vector<Enemy> g_Enemies;

std::vector<Tower> g_Towers;
TowerType g_SelectedTowerType {};
const int g_MaxLevel {1};
const int g_LightningTowerRange {1};
const int g_FireTowerRange {2};
const int g_LightningTowerCost {2};
const int g_FireTowerCost {4};

bool g_IsAnimationRunning {false};

int g_TurnCounter {0};

int g_ActionPoints {2};
int g_ActionPointGrowth {1};
int g_ActionPointProgress {};
const int g_ActionPointGenerationThreshhold {5};

int g_MaxActionPoints {3};
int g_MaxActionPointProgress {};
const int g_MaxActionPointIncreaseThreshhold {3};

#pragma region scaleAndCenterGridConstants
const Rectf g_GridArea {0.f, g_WindowHeight * 0.2f, g_WindowWidth, g_WindowHeight * 0.8f}; //the screenspace rect where the grid will be
const float g_SmallestGridLength {g_GridArea.width < g_GridArea.height ? g_GridArea.width : g_GridArea.height};
const float g_Padding {g_SmallestGridLength * 0.025f}; //space between edge of gridArea and grid
const float g_Margin {1.f}; //space between grid elements
const float g_MaxCellWidth {(g_GridArea.width - 2.f * g_Padding - static_cast<float>(g_Columns - 1) * g_Margin) / g_Columns};
const float g_MaxCellHeight {(g_GridArea.height - 2.f * g_Padding - static_cast<float>(g_Rows - 1) * g_Margin) / g_Rows};
const float g_CellSize {g_MaxCellWidth < g_MaxCellHeight ? g_MaxCellWidth : g_MaxCellHeight};
const float g_GridWidth {g_CellSize * g_Columns + 2.f * g_Padding + static_cast<float>(g_Columns - 1) * g_Margin};
const float g_GridHeight {g_CellSize * g_Rows + 2.f * g_Padding + static_cast<float>(g_Rows - 1) * g_Margin};
const bool g_IsOffsetHorizontal {g_GridArea.width / g_GridWidth > g_GridArea.height / g_GridHeight};
const Point2f g_GridTopLeft
{
	g_GridArea.left + (g_IsOffsetHorizontal ? g_GridArea.width * 0.5f - g_GridWidth * 0.5f : 0.f),
	g_GridArea.top + (g_IsOffsetHorizontal ? 0.f : g_GridArea.height * 0.5f - g_GridHeight * 0.5f)
};
#pragma endregion

#pragma region textures
std::string g_EnemyPath {"Resources/Enemy_"};
const int g_NumEnemyTypes {2};
Texture g_EnemySprites[g_NumEnemyTypes] {};

Texture g_GrassTexture {};

const int g_NumberOfPathTextures {6};
std::string g_PathPath {"Resources/Path_"};
Texture g_PathTextures[g_NumberOfPathTextures] {};

Texture g_HoveredTileTexture {};

const int g_NumTowerTypes {2};

const std::string g_LightningTowerPath {"Resources/LightningTower_"};
Texture g_LightningTowerSprites[g_MaxLevel + 1] {};

const std::string g_FireTowerPath {"Resources/FireTower_"};
Texture g_FireTowerSprites[g_MaxLevel + 1] {};

Texture g_CrosshairSprite {};

Texture g_HeartSprite {};

Texture g_ActionPointSprite {};

Texture g_IdleMenuButton {};
Texture g_HoveredMenuButton {};

Texture g_StartGameText {};
Texture g_QuitGameText {};

Texture g_GameTitle {};
Texture g_GameOverText {};

Texture g_FireballTexture {};
Texture g_LightningTexture {};
#pragma endregion textures

Point2f g_MousePosition {};
TileIndex g_HoveredTile {};

int g_PlayerHealth {5};

const float g_ButtonDistance {25.f};
const float g_ButtonWidth {g_WindowWidth / 3};
const float g_ButtonHeight {g_WindowHeight / 10};

const int g_NumberOfStartMenuButtons {2};
MenuButton g_MenuButtons[g_NumberOfStartMenuButtons] {};

#pragma region functions

#pragma region utils

bool IsPath(TileState tileState);
bool IsOnSameTile(TileIndex a, TileIndex b);
Rectf GetRectFromGridPosition(TileIndex gridIndex);
bool IsTileFree(TileIndex tileIndex);
bool IsTargetTileInRange(const Tower& tower);
bool TileHasEnemy(int pathIndex);
size_t GetSelectedTowerIndex();
bool CanAfford(int price);
bool IsAnimationRunning();
#pragma endregion

#pragma region start

void InitializeResources();
void InitializePath();
void InitializeMenuButtons();
#pragma endregion

#pragma region draw

void DrawTile(TileIndex gridIndex);
void DrawGrid();

void DrawEnemies();
void DrawEnemyHealth(const Enemy& enemy);

void DrawTowers();
void DrawTower(const Tower& tower);
void DrawTower(const TowerType towerType, const int towerLevel, const Rectf& destinationRect);
void DrawFireTower(const int level, const Rectf& destinationRect);
void DrawLightningTower(const int level, const Rectf& destinationRect);
void DrawRange(const Tower& tower);
void HighlightTargetTile(TileIndex targetTile);
void DrawProjectile(Point2f& projectilePosition, const Texture& texture);

void HighlightHoveredTile();
void DrawPlayerHealth();
void DrawPlayerActionPoints();
void DrawSelectedTower();

void DrawStartScreen();
void DrawTitle(const Texture& text);

void DrawGameOverScreen();
#pragma endregion

#pragma region gameLogic

void AdvanceTurn();

void KeepJumpingOverlappingAndHandleReachedGoal();

void SpawnEnemies();

void AdvanceEnemies();
bool JumpOverlappingEnemies();
bool JumpIfOverlapping(Enemy& enemy);
void HandleReachedGoalEnemies();
void HandleDeadEnemies();

void SetTowerAnimationFlag();
void ActivateTowerEffects(const Tower& tower);
void LightningChainDamage(Enemy& enemy, int towerLevel);
void FireTowerDamage(Enemy& enemy, int towerLevel);
void ApplyBurnDamage();

void PlaceTower();
void PlaceLightningTower();
void PlaceFireTower();

void AddActionPoints();
#pragma endregion

#pragma region update

void UpdateMousePosition(const SDL_MouseMotionEvent& e);
bool UpdateHoveredTile();

void UpdateProjectilePositions(float elapsedSec);

void UpdateStartScreen();
#pragma endregion

#pragma region end

void FreeResources();
#pragma endregion

#pragma region input

void SelectTower();
void DeselectOtherTowers(size_t selectedTowerIndex);
void SelectNewTargetTile(size_t towerIndex);
void UpgradeTower();
void ChangeTowerType(TowerType type);
void IncreaseMaxEnergy();

void ClickMenuButton();
#pragma endregion
#pragma endregion
#pragma endregion

#pragma region gameFunctions
void Start();
void Draw();
void Update(float elapsedSec);
void End();
#pragma endregion

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key);
void OnKeyUpEvent(SDL_Keycode key);
void OnMouseMotionEvent(const SDL_MouseMotionEvent& e);
void OnMouseDownEvent(const SDL_MouseButtonEvent& e);
void OnMouseUpEvent(const SDL_MouseButtonEvent& e);
#pragma endregion