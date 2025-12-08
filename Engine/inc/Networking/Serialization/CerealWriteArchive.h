#pragma once

#include <sstream>
#include <vector>
#include <string>

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

/**
 * @concept CerealSerializable
 * @brief Enforces compile-time serialization compatibility.
 *
 * This gives us compiler errors when trying to serialize data that Cereal does not support.
 * That way not being able to serialize a type isn't our problem anymore but we're just passing that responsibility to the
 * GameDeveloper.
 * To all GameDev's: supported serialization types can be found here: https://uscilab.github.io/cereal/index.html
 *
 * By constraining at the concept level rather than letting Cereal fail deep in template
 * instantiation, we surface clear, actionable errors at the call site. This drastically
 * reduces debugging time when someone tries to network a non-serializable type.
 */
template <typename T>
concept CerealSerializable = requires(cereal::BinaryOutputArchive& archive,
                                      T& value)
{
	archive(value);
};

/**
 * @class CerealWriteArchive
 * @brief Accumulates serialized data into a byte buffer for network transmission.
 *
 * This wrapper exists to isolate the rest of the codebase from Cereal's API specifics.
 * If we later switch serialization libraries (for performance, licensing, or feature reasons),
 * only this class needs modification—callers continue using process() and getBytes() unchanged.
 *
 * The class also bridges Cereal's stream-based output to the raw byte vectors.
 */
class CerealWriteArchive
{
public:
	/**
	 * @brief Initializes the archive with a fresh output stream.
	 *
	 * Construction binds the archive to its stream immediately because Cereal archives
	 * are not rebindable—this isn't a limitation we impose but one we inherit.
	 *
	 */
	CerealWriteArchive();

	/**
	 * @brief Finalizes the archive, flushing any buffered data.
	 *
	 * Explicit destructor ensures proper cleanup ordering between the archive
	 * and its underlying stream. Cereal may buffer writes, so destruction
	 * order matters for data integrity.
	 *
	 */
	~CerealWriteArchive();

	/**
	 * @brief Appends a value to the serialization buffer.
	 * @tparam T Any type satisfying CerealSerializable.
	 *
	 * Takes by reference to support both serializing existing objects and allowing Cereal
	 * to handle complex types that may need internal traversal.
	 *
	 * Intentionally mirrors ReadArchive's interface so serialization and deserialization
	 * code can be structurally identical, reducing bugs from asymmetric field ordering.
	 *
	 */
	template <typename T> requires CerealSerializable<T>
	void process(T& value)
	{
		archive(value);
	}

	/**
	 * @brief Extracts the accumulated serialized data as a byte vector.
	 *
	 * Returns a copy rather than a view because the underlying stream may be
	 * invalidated or reused after this call. The const qualifier signals that
	 * this is a read-only extraction—calling it doesn't reset or modify the archive.
	 *
	 * Returns std::byte (not char) to clearly distinguish serialized
	 * network data from text strings, preventing accidental misuse.
	 */
	std::vector<std::byte> getBytes() const;

private:
	std::ostringstream stream; ///< Backing buffer; must outlive archive
	cereal::BinaryOutputArchive archive;
	///< Performs actual serialization; bound to stream at construction
};