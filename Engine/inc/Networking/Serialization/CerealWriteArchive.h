#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <concepts>

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

template<typename T>
concept CerealSerializable = requires(cereal::BinaryOutputArchive& archive, T& value)
{
	archive(value);
};

class CerealWriteArchive
{
public:
	CerealWriteArchive()
		: stream(std::ios::binary)
		, archive(stream)
	{}

	template<typename T>
		requires CerealSerializable<T>
	void process(T& value)
	{
		archive(value);
	}

	std::vector<uint8_t> getBytes()
	{
		auto data = stream.str();
		return {data.begin(), data.end()};
	}

private:
	std::ostringstream stream;
	cereal::BinaryOutputArchive archive;
};