// Core/EngineLoopFactory.h
#pragma once

#include <memory>

class IEngineLoop;
class Game;

class EngineLoopFactory
{
public:
	static std::unique_ptr<IEngineLoop> createEngineLoop(std::unique_ptr<Game> game);

private:
	static std::unique_ptr<IEngineLoop> createClientLoop(std::unique_ptr<Game> game);
	static std::unique_ptr<IEngineLoop> createServerLoop(std::unique_ptr<Game> game);
};