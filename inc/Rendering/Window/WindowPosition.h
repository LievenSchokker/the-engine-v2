///
/// @file WindowPosition.h
/// @brief Window position data structure
///
///
/// @struct WindowPosition
/// @brief Represents the position of a window on the screen
///
/// This structure holds the x and y in pixels for windowplacement.
/// Most Rendering API's use direct pixel translation for rendering this can be used
/// to translate between world position -> camera position -> window position
///
///


#pragma once


struct WindowPosition
{
	int x;
	int y;
};