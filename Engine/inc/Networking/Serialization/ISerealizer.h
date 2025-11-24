#pragma once
#include <vector>
#include <cstdint>
#include <memory>

/**
 * @brief Abstract serialization interface
 *
 * @details Decouples serialization logic from specific library implementation,
 *          enabling runtime or compile-time swapping of serialization backends
 *          without modifying client code.
 */
class ISerializer
{
public:
	virtual ~ISerializer() = default;

	/**
	 * @brief Serializes object to raw bytes
	 *
	 * @details Template method to maintain type safety while allowing
	 *          implementation-specific serialization strategies.
	 */
	template<typename T>
	std::vector<uint8_t> serialize(const T& object);

	/**
	 * @brief Deserializes raw bytes into object
	 *
	 * @details Modifies object in-place to avoid unnecessary copies
	 *          for large data structures.
	 */
	template<typename T>
	void deserialize(const std::vector<uint8_t>& data, T& object);

protected:
	/// @brief Implementation-specific serialization hook
	virtual std::vector<uint8_t> serializeImpl(const void* object, size_t typeHash) = 0;

	/// @brief Implementation-specific deserialization hook
	virtual void deserializeImpl(const std::vector<uint8_t>& data, void* object, size_t typeHash) = 0;
};