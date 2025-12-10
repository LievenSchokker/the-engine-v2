#pragma once

#include "CerealWriteArchive.h"
#include "CerealReadArchive.h"

/**
 * @class CerealReadArchive
 * @brief Reconstructs objects from serialized byte data received over the network.
 *
 * This is the deserialization counterpart to CerealWriteArchive. The symmetric API
 * (both use process()) allows serialization and deserialization code to be nearly
 * identical, reducing bugs from field ordering mismatches—a common source of
 * subtle networking issues that only manifest at runtime.
 *
 * Like its write counterpart, this wrapper isolates Cereal specifics from callers,
 * enabling future serialization library swaps without rippling changes through
 * the codebase.
 */
class CerealReadArchive
{
public:
    /**
     * @brief Constructs a read archive from raw network bytes.
     *
     * Accepts raw pointers rather than a vector because network receive buffers
     * often aren't vector-backed—forcing a copy into a vector at every deserialization
     * would add unnecessary overhead in the hot path.
     *
     * The reinterpret_cast from std::byte to char is unavoidable here; std::istringstream
     * predates std::byte and only accepts char data. This cast is safe because we're
     * treating the data as opaque bytes, not as text.
     *
     *
     * @param data   Pointer to the received byte buffer. Must remain valid for
     *               the duration of construction (data is copied into the stream).
     * @param length Size of the buffer in bytes. Mismatches between stated length
     *               and actual serialized content will cause process() to fail.
     */
    CerealReadArchive(const std::byte* data, size_t length)
       : stream(std::string(reinterpret_cast<const char*>(data), length), std::ios::binary)
       , archive(stream)
    {}

    /**
     * @brief Extracts the next value from the serialized stream.
     * @tparam T The type to deserialize into. Must match what was serialized.
     *
     * No CerealSerializable constraint here (unlike CerealWriteArchive) because
     * deserialization errors from type mismatches will surface naturally when
     * Cereal can't read the expected format. The write-side constraint catches
     * most issues; adding it here would be redundant.
     *
     * Callers must invoke process() in exactly the same order as the sender's
     * serialization—binary archives have no field markers or type tags, so
     * out-of-order reads produce garbage, not errors.
     *
     * @param value Reference to populate with deserialized data. Previous contents
     *              are overwritten.
     */
    template<typename T>
    void process(T& value)
    {
       archive(value);
    }

    size_t getBytesRead() const
    {
        return static_cast<size_t>(stream.tellg());
    }
private:
    mutable std::istringstream stream;          ///< Wraps raw bytes for Cereal consumption; constructed from input data
    cereal::BinaryInputArchive archive; ///< Performs actual deserialization; reads sequentially from stream
};