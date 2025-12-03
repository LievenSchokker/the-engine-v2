#pragma once


#include "IRenderQueueWriter.h"
#include "Rendering/Color.h"

class IRenderer;

#include <queue>

/**
 * @brief Lightweight queue that stores all rendercommands scheduled for a
 * frame.
 */
class RenderQueue : public IRenderQueueWriter
{
public:
	void push(RenderCommand& command) override;
	void sort();
	bool isEmpty() const;
	void clear();
	std::vector<RenderCommand>& getCommands();
private:
	std::vector<RenderCommand> commands;
};