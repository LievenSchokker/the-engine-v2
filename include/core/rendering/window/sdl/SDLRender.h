///
/// Created by Lieven Schokker on 11/11/2025.
///

#pragma once
#include <map>
#include <SDL.h>

#include "core/rendering/window/IRender.h"

class SdlWindow : public IRender {
public:
	SdlWindow();
	~SdlWindow();

	void open(const WindowOptions& opts) override;
	bool isOpen() override;
	void close() override;
	void setTitle(const std ::string& title) override;
	void presentFrame() override;

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	int windowWidth;
	int windowHeight;
};




