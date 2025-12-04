#pragma once

#include "RenderQueueBase.h"

#include <algorithm>

template<typename TCommand>
void RenderQueueBase<TCommand>::push(TCommand& command)
{
	commands.push_back(command);
}

template<typename TCommand>
void RenderQueueBase<TCommand>::sort()
{
	std::sort(commands.begin(), commands.end(),
		[](const TCommand& a, const TCommand& b)
		{
			if (a.layer != b.layer)
				return a.layer < b.layer;
			return a.orderInLayer < b.orderInLayer;
		});
}

template<typename TCommand>
bool RenderQueueBase<TCommand>::isEmpty() const
{
	return commands.empty();
}

template<typename TCommand>
void RenderQueueBase<TCommand>::clear()
{
	commands.clear();
}

template<typename TCommand>
std::vector<TCommand>& RenderQueueBase<TCommand>::getCommands()
{
	return commands;
}