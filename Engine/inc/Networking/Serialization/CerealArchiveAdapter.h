#pragma once
#include "IArchive.h"
#include <sstream>
#include <memory>
#include <vector>

namespace cereal
{
class BinaryOutputArchive;
class BinaryInputArchive;
}

class CerealWriteArchive : public IArchive
{
public:
	CerealWriteArchive();
	~CerealWriteArchive();

	void process(uint8_t& value) override;
	void process(int8_t& value) override;
	void process(uint16_t& value) override;
	void process(int16_t& value) override;
	void process(uint32_t& value) override;
	void process(int32_t& value) override;
	void process(uint64_t& value) override;
	void process(int64_t& value) override;
	void process(float& value) override;
	void process(double& value) override;
	void process(bool& value) override;
	void process(std::string& value) override;

	std::vector<uint8_t> getBytes();

private:
	std::ostringstream stream;
	std::unique_ptr<cereal::BinaryOutputArchive> archive;
};

class CerealReadArchive : public IArchive
{
public:
	CerealReadArchive(const uint8_t* data, size_t length);
	~CerealReadArchive();

	void process(uint8_t& value) override;
	void process(int8_t& value) override;
	void process(uint16_t& value) override;
	void process(int16_t& value) override;
	void process(uint32_t& value) override;
	void process(int32_t& value) override;
	void process(uint64_t& value) override;
	void process(int64_t& value) override;
	void process(float& value) override;
	void process(double& value) override;
	void process(bool& value) override;
	void process(std::string& value) override;

private:
	std::istringstream stream;
	std::unique_ptr<cereal::BinaryInputArchive> archive;
};