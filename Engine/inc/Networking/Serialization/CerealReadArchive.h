#pragma once


#include "CerealWriteArchive.h"

class CerealReadArchive
{
public:
	CerealReadArchive(const uint8_t* data, size_t length)
		: stream(std::string(reinterpret_cast<const char*>(data), length), std::ios::binary)
		, archive(stream)
	{}

	template<typename T>
	void process(T& value)
	{
		archive(value);
	}

private:
	std::istringstream stream;
	cereal::BinaryInputArchive archive;
};