#pragma once
#include <utils.h>
using namespace utils;

#pragma region gameInformation
std::string g_WindowTitle {"Prog1-Game - Unholy C {Tim Scheuer, Arno Buyckx} - 1GDE"};

float g_WindowWidth {1000};
float g_WindowHeight {700};
#pragma endregion gameInformation



#pragma region ownDeclarations
enum class Cellstate
{
	empty,
	path,
	occupied
};
struct Cell
{
	Cellstate state;
};

const int g_Rows {15};
const int g_Collumns {25};
Cell g_Grid[g_Rows][g_Collumns] {};

#pragma region scaleAndCenterGridConstants
const float g_SmallestWindowLength {g_WindowWidth < g_WindowHeight ? g_WindowWidth : g_WindowHeight};
const float g_Padding {g_SmallestWindowLength * 0.025f}; //space between edge of screen and grid
const float g_Margin {g_SmallestWindowLength * 0.0125f}; //space between grid elements
const float g_MaxCellWidth {(g_WindowWidth - 2.f * g_Padding - static_cast<float>(g_Collumns - 1) * g_Margin) / g_Collumns};
const float g_MaxCellHeight {(g_WindowHeight - 2.f * g_Padding - static_cast<float>(g_Rows - 1) * g_Margin) / g_Rows};
const float g_CellSize {g_MaxCellWidth < g_MaxCellHeight ? g_MaxCellWidth : g_MaxCellHeight};
const float g_GridWidth {g_CellSize * g_Collumns + 2.f * g_Padding + static_cast<float>(g_Collumns - 1) * g_Margin};
const float g_GridHeight {g_CellSize * g_Rows + 2.f * g_Padding + static_cast<float>(g_Rows - 1) * g_Margin};
const bool g_IsOffsetHorizontal {g_WindowWidth / g_GridWidth > g_WindowHeight / g_GridHeight};
const Point2f g_GridTopLeft
{
	g_IsOffsetHorizontal ? g_WindowWidth * 0.5f - g_GridWidth * 0.5f : 0.f,
	g_IsOffsetHorizontal ? 0.f : g_WindowHeight * 0.5f - g_GridHeight * 0.5f
};
#pragma endregion scaleAndCenterGridConstants

void DrawGrid();
Rectf GetRectFromGridPosition(int row, int column);
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