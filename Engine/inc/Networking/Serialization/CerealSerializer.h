#pragma once
#include <vector>
#include <cstdint>

/**
 * @brief Cereal-based serialization backend
 *
 * @details Stateless implementation providing binary serialization.
 *          All methods are static to enable compile-time policy pattern
 *          without instantiation overhead.
 */
class CerealSerializer
{
public:
	CerealSerializer() = delete;

	template<typename T>
	static std::vector<uint8_t> serialize(const T& object);

	template<typename T>
	static void deserialize(const std::vector<uint8_t>& data, T& object);
};

#include "CerealSerializer.hpp"