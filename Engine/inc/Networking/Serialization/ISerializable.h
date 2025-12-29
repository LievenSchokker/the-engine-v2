#pragma once


#include "Serialization.h"

class CerealWriteArchive;
class CerealReadArchive;

/**
 * @brief Interface for objects that can be serialized/deserialized.
 */
class ISerializable
{
public:
	virtual ~ISerializable() = default;

	virtual void serialize(WriteArchive& archive) const = 0;
	virtual void deserialize(ReadArchive& archive) = 0;
};