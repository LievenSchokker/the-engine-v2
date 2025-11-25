#pragma once

#include <sstream>
#include <vector>
#include <string>

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

	std::vector<std::byte> getBytes()
	{
		auto data = stream.str();
		return {reinterpret_cast<const std::byte*>(data.data()),
				reinterpret_cast<const std::byte*>(data.data() + data.size())};
	}

private:
	std::ostringstream stream;
	cereal::BinaryOutputArchive archive;
};