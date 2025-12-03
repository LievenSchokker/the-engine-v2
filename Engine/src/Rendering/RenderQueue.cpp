#include "Rendering/RenderQueue/RenderQueue.h"
#include "Rendering/RenderCommand.h"
#include <queue>

void RenderQueue::push(RenderCommand& command)
{
	commands.emplace_back(command);
}

void RenderQueue::sort()
{
	std::ranges::sort(commands,
	                  [](const RenderCommand& command,
	                     const RenderCommand& commandOther)
	                  {
		                  return command.getSortKey() < commandOther.
		                         getSortKey();
	                  });
}

void RenderQueue::clear()
{
	commands.clear();
}

bool RenderQueue::isEmpty() const
{
	return commands.empty();
}

std::vector<RenderCommand>& RenderQueue::getCommands()
{
	return commands;
}