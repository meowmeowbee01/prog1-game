#pragma once
#include <utils.h>
#include <vector>

using namespace utils;

#pragma region gameInformation
std::string g_WindowTitle {"Prog1-Game - Unholy C {Tim Scheuer, Arno Buyckx} - 1GDE"};

float g_WindowWidth {1000};
float g_WindowHeight {700};
#pragma endregion gameInformation



#pragma region ownDeclarations
#pragma region enumsAndStructs
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
	path,
	tower
};
enum class EnemyType
{
	goober,
	angryGoober
};
enum class EnemyState
{
	alive,
	dead,
	reachedGoal
};
enum class TowerType
{
	lightning,
	bomb
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
	EnemyState state;
};
struct Tower
{
	TowerType towerType;
	TileIndex gridPosition;
	TileIndex targetTile;
	bool isSelected;
};
#pragma endregion enumsAndStructs

const int g_Rows {10};
const int g_Columns {20};
Tile g_Grid[g_Rows][g_Columns] {};
int g_StartingRowIndex {3};
std::vector<TileIndex> g_PathIndeces {};

std::vector<Enemy> g_Enemies;

std::vector<Tower> g_Towers;


#pragma region scaleAndCenterGridConstants
const Rectf g_GridArea {0.f, g_WindowHeight * 0.2f, g_WindowWidth, g_WindowHeight * 0.8f};
const float g_SmallestGridLength {g_GridArea.width < g_GridArea.height ? g_GridArea.width : g_GridArea.height};
const float g_Padding {g_SmallestGridLength * 0.025f}; //space between edge of screen and grid
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
#pragma endregion scaleAndCenterGridConstants

#pragma region Textures
std::string g_EnemyPath {"Resources/Enemy_"};
const int g_NumEnemyTypes {2};
Texture g_EnemySprites[g_NumEnemyTypes] {};

Texture g_GrassTexture {};

Texture g_PathTexture {};

Texture g_HoveredTileTexture {};

std::string g_GunTowerPath {"Resources/LightningTower_"};
const int g_NumberOfTowerTypes {2};
Texture g_TowerSprites[g_NumberOfTowerTypes] {};
Texture g_CrosshairSprite {};
#pragma endregion Textures

Point2f g_MousePosition {};

TileIndex g_HoveredTile {};

#pragma region Functions

#pragma region utils

bool IsOnSameTile(TileIndex a, TileIndex b);
Rectf GetRectFromGridPosition(TileIndex gridIndex);
bool IsCellFree(TileIndex tileIndex);
#pragma endregion

#pragma region start

void InitializeResources();
void InitializePath();
void InitializeTowers();
#pragma endregion

#pragma region Draw

void DrawCell(TileIndex gridIndex);
void DrawGrid();
void DrawEnemies();
void DrawTowers();
void HighlightTargetTile(TileIndex targetTile);
void HighlightHoveredTile();
#pragma endregion

#pragma region gameLogic

void AdvanceTurn();
void PlaceTower();
void JumpOverlappingEnemies();
void JumpIfOverlapping(Enemy& enemy);
#pragma endregion

#pragma region Update

void UpdateMousePosition(const SDL_MouseMotionEvent& e);
bool UpdateHoveredTile();
#pragma endregion

#pragma region End

void FreeResources();
#pragma endregion

#pragma region Input

void SelectTower();
void DeselectOtherTowers(size_t selectedTowerIndex);
void SelectNewTargetTile(size_t towerIndex);
#pragma endregion
#pragma endregion Functions
#pragma endregion ownDeclarations

#pragma region gameFunctions
void Start();
void Draw();
void Update(float elapsedSec);
void End();
#pragma endregion gameFunctions

#pragma region inputHandling
void OnKeyDownEvent(SDL_Keycode key);
void OnKeyUpEvent(SDL_Keycode key);
void OnMouseMotionEvent(const SDL_MouseMotionEvent& e);
void OnMouseDownEvent(const SDL_MouseButtonEvent& e);
void OnMouseUpEvent(const SDL_MouseButtonEvent& e);
#pragma endregion inputHandling