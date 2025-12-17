#pragma once


#include <memory>

class IEngineLoop;
class Game;

class EngineLoopFactory
{
public:
	static std::unique_ptr<IEngineLoop> createEngineLoop(std::unique_ptr<Game> game);
};