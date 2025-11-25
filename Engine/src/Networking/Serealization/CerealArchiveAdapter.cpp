// Serialization/CerealArchive.cpp
#include "Networking/Serialization/CerealArchiveAdapter.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <sstream>
#include <string>


CerealWriteArchive::CerealWriteArchive()
    : stream(std::ios::binary)
    , archive(std::make_unique<cereal::BinaryOutputArchive>(stream))
{}

CerealWriteArchive::~CerealWriteArchive() = default;

void CerealWriteArchive::process(uint8_t& value) { (*archive)(value); }
void CerealWriteArchive::process(int8_t& value) { (*archive)(value); }
void CerealWriteArchive::process(uint16_t& value) { (*archive)(value); }
void CerealWriteArchive::process(int16_t& value) { (*archive)(value); }
void CerealWriteArchive::process(uint32_t& value) { (*archive)(value); }
void CerealWriteArchive::process(int32_t& value) { (*archive)(value); }
void CerealWriteArchive::process(uint64_t& value) { (*archive)(value); }
void CerealWriteArchive::process(int64_t& value) { (*archive)(value); }
void CerealWriteArchive::process(float& value) { (*archive)(value); }
void CerealWriteArchive::process(double& value) { (*archive)(value); }
void CerealWriteArchive::process(bool& value) { (*archive)(value); }
void CerealWriteArchive::process(std::string& value) { (*archive)(value); }

std::vector<uint8_t> CerealWriteArchive::getBytes()
{
    archive.reset();
    std::string data = stream.str();
    return std::vector<uint8_t>(data.begin(), data.end());
}


CerealReadArchive::CerealReadArchive(const uint8_t* data, size_t length)
    : stream(std::string(reinterpret_cast<const char*>(data), length), std::ios::binary)
    , archive(std::make_unique<cereal::BinaryInputArchive>(stream))
{}

CerealReadArchive::~CerealReadArchive() = default;

void CerealReadArchive::process(uint8_t& value) { (*archive)(value); }
void CerealReadArchive::process(int8_t& value) { (*archive)(value); }
void CerealReadArchive::process(uint16_t& value) { (*archive)(value); }
void CerealReadArchive::process(int16_t& value) { (*archive)(value); }
void CerealReadArchive::process(uint32_t& value) { (*archive)(value); }
void CerealReadArchive::process(int32_t& value) { (*archive)(value); }
void CerealReadArchive::process(uint64_t& value) { (*archive)(value); }
void CerealReadArchive::process(int64_t& value) { (*archive)(value); }
void CerealReadArchive::process(float& value) { (*archive)(value); }
void CerealReadArchive::process(double& value) { (*archive)(value); }
void CerealReadArchive::process(bool& value) { (*archive)(value); }
void CerealReadArchive::process(std::string& value) { (*archive)(value); }