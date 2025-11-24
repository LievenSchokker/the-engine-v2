#pragma once
#include "Serealization.h"

template<typename SerializerBackend>
template<typename T>
std::vector<uint8_t> Serialization<SerializerBackend>::toBytes(const T& object)
{
	return SerializerBackend::serialize(object);
}

template<typename SerializerBackend>
template<typename T>
T Serialization<SerializerBackend>::fromBytes(const std::vector<uint8_t>& data)
{
	T object;
	SerializerBackend::deserialize(data, object);
	return object;
}

template<typename SerializerBackend>
template<typename T>
void Serialization<SerializerBackend>::fromBytes(const std::vector<uint8_t>& data, T& object)
{
	SerializerBackend::deserialize(data, object);
}