#include "Rendering/SDL/SDLRenderer.h"
#include "External/IBackendContext.h"
#include "Math/Vector2Utils.h"
#include "Rendering/IUIRenderHook.h"
#include "Rendering/Nuklear/NuklearSDLRenderHook.h"
#include "Rendering/Window/WindowOptions.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
constexpr double kRotationThresholdDegrees = 0.01;
constexpr int kMinWindowDimension = 1;
}  // namespace

SDLRenderer::SDLRenderer(IBackendContext& context)
{
	assert(context.wasInit(SDL_INIT_VIDEO) &&
		"SDL video subsystem not initialized");
}

SDLRenderer::~SDLRenderer()
{
	SDLRenderer::close();
}

void SDLRenderer::open(const WindowOptions& options)
{
	// Validate window dimensions
	if ( options.width < kMinWindowDimension ||
		 options.height < kMinWindowDimension )
	{
		std::cerr << "Invalid window dimensions: " << options.width << "x"
			<< options.height << " (minimum: " << kMinWindowDimension
			<< "x" << kMinWindowDimension << ")\n";
		return;
	}

	Uint32 flags = SDL_WINDOW_SHOWN;

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
	// Disable compositor bypass
	if ( !SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0") )
	{
		std::cerr << "SDL can not disable compositor bypass!" << std::endl;
		return;
	}
#endif

	window = SDL_CreateWindow(options.title.c_str(), SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, options.width,
	                          options.height, flags);

	if ( window == nullptr )
	{
		std::cerr << "Window could not be created! SDL_Error: "
			<< SDL_GetError() << "\n";
		return;
	}

	renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if ( renderer == nullptr )
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
		SDL_DestroyWindow(window);
		window = nullptr;
		return;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	//Might want to create factory for this..
	//But since we only use One library thought it was overkill.

	SDLRenderer::setUIRenderHook(
		std::make_unique<NuklearSDLRenderHook>(window, renderer));
	if (userInterfaceHook != nullptr) {
		userInterfaceHook->initialize();
	}
	SDL_RenderClear(renderer);
}

void SDLRenderer::close()
{
	if (userInterfaceHook != nullptr) {
		userInterfaceHook->close();
	}

	destroySolidQuadTexture();

	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void SDLRenderer::beginFrame(const Color& clearColor)
{
	if (renderer == nullptr) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
	                       clearColor.a);

	SDL_RenderClear(renderer);

	if (userInterfaceHook != nullptr) {
		userInterfaceHook->beginFrame();
	}
}

void SDLRenderer::presentFrame()
{
	beginFrame(Color::black());
	if (renderer == nullptr) {
		return;
	}

	if (userInterfaceHook != nullptr) {
		userInterfaceHook->presentFrame();
	}

	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

void SDLRenderer::setTitle(const std::string& title)
{
	if (window != nullptr) {
		SDL_SetWindowTitle(window, title.c_str());
	}
}

bool SDLRenderer::isOpen()
{
	return window != nullptr;
}

void SDLRenderer::drawCircle(const Vector2& center, double radius,
                             const Color& color, const Vector2& scale)
{
	if ( renderer == nullptr )
	{
		return;
	}

	const Vector2 finalScale = Vector2Utils::sanitizeScale(scale);
	const double scaledRadiusX = std::abs(radius * finalScale.x());
	const double scaledRadiusY = std::abs(radius * finalScale.y());

	if ( scaledRadiusX <= 0.0 || scaledRadiusY <= 0.0 )
	{
		return;
	}

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	const int centerX = static_cast<int>(std::round(center.x()));
	const int centerY = static_cast<int>(std::round(center.y()));
	const int rx = std::max(1, static_cast<int>(std::round(scaledRadiusX)));
	const int ry = std::max(1, static_cast<int>(std::round(scaledRadiusY)));

	for ( int y = -ry; y <= ry; ++y )
	{
		const double normalizedY =
			static_cast<double>(y) / static_cast<double>(ry);
		const double span =
			static_cast<double>(rx) *
			std::sqrt(std::max(0.0, 1.0 - normalizedY * normalizedY));
		const int startX = static_cast<int>(std::floor(-span));
		const int endX = static_cast<int>(std::ceil(span));
		SDL_RenderDrawLine(renderer, centerX + startX, centerY + y,
		                   centerX + endX, centerY + y);
	}
}

void SDLRenderer::drawRectangle(const Vector2& center, const Vector2& size,
                                double rotationDegrees, const Color& color,
                                const Vector2& scale)
{
	if ( renderer == nullptr )
	{
		return;
	}

	const Vector2 finalScale = Vector2Utils::sanitizeScale(scale);
	const double width = std::abs(size.x() * finalScale.x());
	const double height = std::abs(size.y() * finalScale.y());

	if ( width <= 0.0 || height <= 0.0 )
	{
		return;
	}

	if ( !ensureSolidQuadTexture() )
	{
		return;
	}

	SDL_SetTextureColorMod(solidQuadTexture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(solidQuadTexture, color.a);

	const double halfWidth = width * 0.5;
	const double halfHeight = height * 0.5;
	SDL_FRect rect{static_cast<float>(center.x() - halfWidth),
				   static_cast<float>(center.y() - halfHeight),
				   static_cast<float>(width), static_cast<float>(height)};

	if ( std::abs(rotationDegrees) < kRotationThresholdDegrees )
	{
		SDL_RenderCopyF(renderer, solidQuadTexture, nullptr, &rect);
		return;
	}

	SDL_RenderCopyExF(renderer, solidQuadTexture, nullptr, &rect,
	                  rotationDegrees, nullptr, SDL_FLIP_NONE);
}

bool SDLRenderer::ensureSolidQuadTexture()
{
	if ( solidQuadTexture != nullptr )
	{
		return true;
	}

	if ( renderer == nullptr )
	{
		return false;
	}

	solidQuadTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
										 SDL_TEXTUREACCESS_STATIC, 1, 1);
	if ( solidQuadTexture == nullptr )
	{
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << "\n";
		return false;
	}

	const Uint32 pixel = 0xFFFFFFFF;
	if ( SDL_UpdateTexture(solidQuadTexture, nullptr, &pixel, sizeof(pixel)) !=
		 0 )
	{
		std::cerr << "SDL_UpdateTexture Error: " << SDL_GetError() << "\n";
		SDL_DestroyTexture(solidQuadTexture);
		solidQuadTexture = nullptr;
		return false;
	}

	SDL_SetTextureBlendMode(solidQuadTexture, SDL_BLENDMODE_BLEND);
	return true;
}

void SDLRenderer::destroySolidQuadTexture()
{
	if ( solidQuadTexture != nullptr )
	{
		SDL_DestroyTexture(solidQuadTexture);
		solidQuadTexture = nullptr;
	}
}

void SDLRenderer::setUIRenderHook(std::unique_ptr<IUIRenderHook> hook)
{
	userInterfaceHook = std::move(hook);
}