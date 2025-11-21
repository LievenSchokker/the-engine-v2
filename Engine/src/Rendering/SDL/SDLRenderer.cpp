///
/// Created by Lieven Schokker on 11/11/2025.
///

#include "../../../inc/Rendering/SDL/SDLRenderer.h"

#include "../../../inc/External/SdlContext.h"
#include "../../../inc/GameObject/Vector2Utils.h"
#include "../../../inc/Rendering/Window/WindowOptions.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
constexpr double kPi = 3.14159265358979323846;
constexpr double kRotationThresholdDegrees =
	0.01;  // Threshold below which rotation is treated as zero
constexpr int kMinWindowDimension = 1;	// Minimum window width/height
}  // namespace

SDLRenderer::SDLRenderer(SdlContext& context)
{
	assert(context.wasInit(SDL_INIT_VIDEO) &&
		   "SDL video subsystem not initialized");
}

SDLRenderer::~SDLRenderer()
{
	close();
}

void SDLRenderer::open(const WindowOptions& opts)
{
	// Validate window dimensions
	if ( opts.width < kMinWindowDimension ||
		 opts.height < kMinWindowDimension ) {
		std::cerr << "Invalid window dimensions: " << opts.width << "x"
				  << opts.height << " (minimum: " << kMinWindowDimension << "x"
				  << kMinWindowDimension << ")\n";
		return;
	}

	Uint32 flags = SDL_WINDOW_SHOWN;

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
	// Disable compositor bypass
	if ( !SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0") ) {
		std::cerr << "SDL can not disable compositor bypass!" << std::endl;
		return;
	}
#endif

	window = SDL_CreateWindow(opts.title.c_str(), SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED, opts.width, opts.height,
							  flags);

	if ( window == nullptr ) {
		std::cerr << "Window could not be created! SDL_Error: "
				  << SDL_GetError() << "\n";
		return;
	}

	renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if ( renderer == nullptr ) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
		SDL_DestroyWindow(window);
		window = nullptr;
		return;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

bool SDLRenderer::isOpen()
{
	return window != nullptr;
}

void SDLRenderer::close()
{
	if ( renderer != nullptr ) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if ( window != nullptr ) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void SDLRenderer::setTitle(const std::string& title)
{
	if ( window != nullptr ) {
		SDL_SetWindowTitle(window, title.c_str());
	}
}

void SDLRenderer::beginFrame(const Color& clearColor)
{
	if ( renderer == nullptr ) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
						   clearColor.a);
	SDL_RenderClear(renderer);
}

void SDLRenderer::presentFrame()
{
	if ( renderer == nullptr ) {
		return;
	}

	SDL_RenderPresent(renderer);
}

void SDLRenderer::drawCircle(const Vector2& center, double radius,
							 const Color& color, const Vector2& scale)
{
	if ( renderer == nullptr ) {
		return;
	}

	const Vector2 finalScale = Vector2Utils::sanitizeScale(scale);
	const double scaledRadiusX = std::abs(radius * finalScale.x);
	const double scaledRadiusY = std::abs(radius * finalScale.y);

	if ( scaledRadiusX <= 0.0 || scaledRadiusY <= 0.0 ) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	const int centerX = static_cast<int>(std::round(center.x));
	const int centerY = static_cast<int>(std::round(center.y));
	const int rx = std::max(1, static_cast<int>(std::round(scaledRadiusX)));
	const int ry = std::max(1, static_cast<int>(std::round(scaledRadiusY)));

	for ( int y = -ry; y <= ry; ++y ) {
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
	if ( renderer == nullptr ) {
		return;
	}

	const Vector2 finalScale = Vector2Utils::sanitizeScale(scale);
	const double width = std::abs(size.x * finalScale.x);
	const double height = std::abs(size.y * finalScale.y);

	if ( width <= 0.0 || height <= 0.0 ) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	const double halfWidth = width * 0.5;
	const double halfHeight = height * 0.5;

	if ( std::abs(rotationDegrees) < kRotationThresholdDegrees ) {
		SDL_FRect rect{static_cast<float>(center.x - halfWidth),
					   static_cast<float>(center.y - halfHeight),
					   static_cast<float>(width), static_cast<float>(height)};
		SDL_RenderFillRectF(renderer, &rect);
		return;
	}

#if SDL_VERSION_ATLEAST(2, 0, 18)
	const double radians = rotationDegrees * (kPi / 180.0);
	const double cosTheta = std::cos(radians);
	const double sinTheta = std::sin(radians);
	const std::array<Vector2, 4> corners = {{
		{-halfWidth, -halfHeight},
		{halfWidth, -halfHeight},
		{halfWidth, halfHeight},
		{-halfWidth, halfHeight},
	}};

	SDL_Vertex vertices[4];
	const SDL_Color sdlColor{color.r, color.g, color.b, color.a};

	for ( size_t i = 0; i < corners.size(); ++i ) {
		const double rotatedX =
			corners[i].x * cosTheta - corners[i].y * sinTheta;
		const double rotatedY =
			corners[i].x * sinTheta + corners[i].y * cosTheta;
		vertices[i].position.x = static_cast<float>(center.x + rotatedX);
		vertices[i].position.y = static_cast<float>(center.y + rotatedY);
		vertices[i].color = sdlColor;
		vertices[i].tex_coord.x = 0.0f;
		vertices[i].tex_coord.y = 0.0f;
	}

	const int indices[6] = {0, 1, 2, 2, 3, 0};
	SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
#else
	SDL_FRect rect{static_cast<float>(center.x - halfWidth),
				   static_cast<float>(center.y - halfHeight),
				   static_cast<float>(width), static_cast<float>(height)};
	SDL_RenderFillRectF(renderer, &rect);
#endif
}
