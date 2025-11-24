#pragma once
#include <vector>
#include <cstdint>
#include "CerealSerializer.h"

/**
 * @brief Serialization utilities with pluggable backend
 *
 * @tparam SerializerBackend Implementation providing serialize/deserialize.
 *                           Defaults to CerealSerializer.
 *
 * @details Template parameter enables swapping serialization libraries at
 *          compile-time without modifying call sites. Zero-cost abstraction
 *          as compiler inlines forwarding functions. Class-based template
 *          pattern chosen to enable compile-time backend selection while
 *          maintaining static-only interface.
 */
template<typename SerializerBackend = CerealSerializer>
class Serialization
{
public:
	Serialization() = delete;  // Prevent instantiation

	/**
	 * @brief Serializes object to binary format
	 *
	 * @details Delegates to backend's serialize() method. Isolated per-call
	 *          operation ensures thread-safety without synchronization.
	 */
	template<typename T>
	static std::vector<uint8_t> toBytes(const T& object);

	/**
	 * @brief Deserializes binary data into new object
	 *
	 * @details Returns by value to leverage move semantics. Backend handles
	 *          format-specific parsing and validation.
	 */
	template<typename T>
	static T fromBytes(const std::vector<uint8_t>& data);

	/**
	 * @brief Deserializes into existing object
	 *
	 * @details Avoids allocation for reusable objects in hot paths such as
	 *          network packet processing or object pooling systems.
	 */
	template<typename T>
	static void fromBytes(const std::vector<uint8_t>& data, T& object);
};

// Convenience alias for default backend
using DefaultSerialization = Serialization<CerealSerializer>;

#include "Serialization.hpp"