#pragma once


#include <string>
#include <cstdint>

class IArchive
{
public:
	virtual ~IArchive() = default;

	virtual void process(uint8_t& value) = 0;
	virtual void process(int8_t& value) = 0;
	virtual void process(uint16_t& value) = 0;
	virtual void process(int16_t& value) = 0;
	virtual void process(uint32_t& value) = 0;
	virtual void process(int32_t& value) = 0;
	virtual void process(uint64_t& value) = 0;
	virtual void process(int64_t& value) = 0;
	virtual void process(float& value) = 0;
	virtual void process(double& value) = 0;
	virtual void process(bool& value) = 0;
	virtual void process(std::string& value) = 0;
};