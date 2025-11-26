#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

#include "../../../inc/Rendering/SDL/SDLRenderer.h"

#include "../../../inc/External/SdlContext.h"
#include "../../../inc/GameObject/Vector2Utils.h"
#include "../../../inc/Rendering/Window/WindowOptions.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <math.h>


namespace
{
constexpr double kRotationThresholdDegrees = 0.01;
constexpr int kMinWindowDimension = 1;

// Nuklear context
struct nk_context* nkCtx = nullptr;
}  // namespace

// ... keep the constructor and destructor as-is ...

SDLRenderer::SDLRenderer(SdlContext& context)
{
    assert(context.wasInit(SDL_INIT_VIDEO) &&
           "SDL video subsystem not initialized");
}

SDLRenderer::~SDLRenderer()
{
    close();
}

void SDLRenderer::open(const WindowOptions& options)
{
    // ... keep all your existing window/renderer creation code ...

    if ( options.width < kMinWindowDimension ||
         options.height < kMinWindowDimension ) {
        std::cerr << "Invalid window dimensions: " << options.width << "x"
                  << options.height << " (minimum: " << kMinWindowDimension
                  << "x" << kMinWindowDimension << ")\n";
        return;
    }

    Uint32 flags = SDL_WINDOW_SHOWN;

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    if ( !SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0") ) {
        std::cerr << "SDL can not disable compositor bypass!" << std::endl;
        return;
    }
#endif

    window = SDL_CreateWindow(options.title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, options.width,
                              options.height, flags);

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

    // Initialize Nuklear
    nkCtx = nk_sdl_init(window, renderer);
    if (nkCtx) {
        struct nk_font_atlas* atlas;
        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();
        std::cout << "Nuklear initialized successfully!" << std::endl;
    }
	SDL_RenderClear(renderer);
}

void SDLRenderer::close()
{
    // Shutdown Nuklear
    if (nkCtx) {
        nk_sdl_shutdown();
        nkCtx = nullptr;
    }

    destroySolidQuadTexture();

    if ( renderer != nullptr ) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if ( window != nullptr ) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void SDLRenderer::handleEvent(SDL_Event* event)
{
	nk_sdl_handle_event(event);
}

void SDLRenderer::beginFrame(const Color& clearColor)
{
    if ( renderer == nullptr ) {
        return;
    }
	SDL_RenderClear(renderer);

    // Start Nuklear input
    nk_input_begin(nkCtx);

    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a);
}

void SDLRenderer::presentFrame()
{
    if ( renderer == nullptr ) {
        return;
    }

    // End Nuklear input
    nk_input_end(nkCtx);

    // Test Nuklear window
    if (nk_begin(nkCtx, "Test Window", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

        nk_layout_row_static(nkCtx, 30, 80, 1);
        if (nk_button_label(nkCtx, "Button")) {
            std::cout << "Button pressed!" << std::endl;
        }

        nk_layout_row_dynamic(nkCtx, 30, 2);
        static int option = 0;
        if (nk_option_label(nkCtx, "Easy", option == 0)) option = 0;
        if (nk_option_label(nkCtx, "Hard", option == 1)) option = 1;

        nk_layout_row_dynamic(nkCtx, 25, 1);
        static float value = 0.5f;
        nk_slider_float(nkCtx, 0, &value, 1.0f, 0.1f);
    }
    nk_end(nkCtx);

    // Render Nuklear
    nk_sdl_render(NK_ANTI_ALIASING_ON);

    SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

void SDLRenderer::setTitle(const std::string& title)
{
	if ( window != nullptr ) {
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

	if ( !ensureSolidQuadTexture() ) {
		return;
	}

	SDL_SetTextureColorMod(solidQuadTexture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(solidQuadTexture, color.a);

	const double halfWidth = width * 0.5;
	const double halfHeight = height * 0.5;
	SDL_FRect rect{static_cast<float>(center.x - halfWidth),
				   static_cast<float>(center.y - halfHeight),
				   static_cast<float>(width), static_cast<float>(height)};

	if ( std::abs(rotationDegrees) < kRotationThresholdDegrees ) {
		SDL_RenderCopyF(renderer, solidQuadTexture, nullptr, &rect);
		return;
	}

	SDL_RenderCopyExF(renderer, solidQuadTexture, nullptr, &rect,
					  rotationDegrees, nullptr, SDL_FLIP_NONE);
}

bool SDLRenderer::ensureSolidQuadTexture()
{
	if ( solidQuadTexture != nullptr ) {
		return true;
	}

	if ( renderer == nullptr ) {
		return false;
	}

	solidQuadTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
										 SDL_TEXTUREACCESS_STATIC, 1, 1);
	if ( solidQuadTexture == nullptr ) {
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << "\n";
		return false;
	}

	const Uint32 pixel = 0xFFFFFFFF;
	if ( SDL_UpdateTexture(solidQuadTexture, nullptr, &pixel, sizeof(pixel)) !=
		 0 ) {
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
	if ( solidQuadTexture != nullptr ) {
		SDL_DestroyTexture(solidQuadTexture);
		solidQuadTexture = nullptr;
	}
}
