#pragma once

#include "Networking/NetworkingOptions.h"
#include "RenderSettings.h"
#include "Rendering/Color.h"
#include "Rendering/RenderBackend.h"
#include "Rendering/Window/WindowOptions.h"

enum class EngineSystem : uint32_t;

struct ApplicationSpecifications
{
	NetworkingOptions networkingOptions{};
	RenderSettings renderSettings{};
	EngineSystem engineSystem{};
	Color clearColor = Color::black();
	double maxFrameTime{};
};

enum class EngineSystem : uint32_t
{
	None = 0,
	Renderer = 1 << 0,
	Physics = 1 << 1,
	Audio = 1 << 2,
	Input = 1 << 3,
	NetClient = 1 << 4,
	NetServer = 1 << 5,
	Events = 1 << 6,

	Client = Renderer | Physics | Audio | Input | NetClient | Events,
	Server = Physics | NetServer | Events,
};

inline EngineSystem operator|(EngineSystem a, EngineSystem b)
{
	return static_cast<EngineSystem>(static_cast<uint32_t>(a) |
									 static_cast<uint32_t>(b));
}

inline EngineSystem operator&(EngineSystem a, EngineSystem b)
{
	return static_cast<EngineSystem>(static_cast<uint32_t>(a) &
									 static_cast<uint32_t>(b));
}

inline EngineSystem& operator|=(EngineSystem& a, EngineSystem b)
{
	return a = a | b;
}

inline bool hasFlag(const EngineSystem value, const EngineSystem flag)
{
	return (value & flag) == flag;
}
