#pragma once
#include "utilities.h"
#include <GLFW/glfw3.h>

void DrawBox(Vec2 feetCoords, Vec2 headCoords);

void DrawHealthBar(Vec2 feetCoords, Vec2 headCoords, int health);

void DrawArmorBar(Vec2 feetCoords, Vec2 headCoords, int armor);

void DrawSnapLines(Vec2 feetCoords);