#pragma once


#include <vector>

template<typename TCommand>
class RenderQueueBase
{
public:
	void push(TCommand& command);
	void sort();
	bool isEmpty() const;
	void clear();
	std::vector<TCommand>& getCommands();

protected:
	std::vector<TCommand> commands;
};

#include "RenderQueueBase.inl"