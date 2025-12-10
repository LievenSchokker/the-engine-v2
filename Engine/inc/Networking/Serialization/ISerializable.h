#pragma once

class CerealWriteArchive;
class CerealReadArchive;

/**
 * @brief Interface for objects that can be serialized/deserialized.
 */
class ISerializable
{
public:
	virtual ~ISerializable() = default;

	virtual void serialize(CerealWriteArchive& archive) const = 0;
	virtual void deserialize(CerealReadArchive& archive) = 0;
};