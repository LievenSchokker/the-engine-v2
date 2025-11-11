#pragma once
#include <string>
#include "window/WindowOptions.h"


class IRender
{
public:
	virtual ~IRender() = default;

	virtual void open(const WindowOptions &opts) = 0;

	virtual void presentFrame() = 0;

	virtual void close() = 0;

	virtual bool isOpen() = 0;

	virtual void setTitle(const std::string &title) = 0;
};
