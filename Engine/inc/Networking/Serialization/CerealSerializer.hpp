#pragma once
#include "CerealSerializer.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <sstream>
#include <stdexcept>

template<typename T>
std::vector<uint8_t> CerealSerializer::serialize(const T& object)
{
	std::ostringstream stream(std::ios::binary);

	{
		cereal::BinaryOutputArchive archive(stream);
		archive(object);
	}

	const std::string str = stream.str();
	return std::vector<uint8_t>(str.begin(), str.end());
}

template<typename T>
void CerealSerializer::deserialize(const std::vector<uint8_t>& data, T& object)
{
	if (data.empty())
	{
		throw std::runtime_error("Cannot deserialize empty data");
	}

	const std::string str(data.begin(), data.end());
	std::istringstream stream(str, std::ios::binary);

	cereal::BinaryInputArchive archive(stream);
	archive(object);
}