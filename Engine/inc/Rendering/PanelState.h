//
// Created by thijs on 9-1-2026.
//
#pragma once
#include "Component/UIElement/Docking.h"

struct PanelState {
	float x, y, width, height;
	bool initialized = false;
	bool isMinimized = false;
	bool isClosed = false;
	UIDock appliedDock = UIDock::TopRight;
};