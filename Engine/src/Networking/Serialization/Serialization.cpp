#include "SerializationUtil.h"
#include "Networking/Serialization/ISerealizer.h"
#include "Networking/Serialization/Serealization.h"

void SerializationUtil::setSerializer(std::unique_ptr<ISerializer> newSerializer)
{
	Serialization::serializer = std::move(newSerializer);
}

ISerializer& SerializationUtil::getInstance()
{
	if (!Serialization::serializer)
	{
		Serialization::serializer = std::make_unique<CerealSerializer>();
	}
	return *Serialization::serializer;
}