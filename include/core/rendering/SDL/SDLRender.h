#pragma once
#include <map>
#include <SDL.h>
#include "../IRender.h"
#include "external/SdlContext.h"

class SDLRender : public IRender {
public:

	explicit SDLRender(SdlContext &ctx);
	~SDLRender() override;

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




