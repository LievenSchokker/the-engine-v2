#pragma once


#include "IArchive.h"
#include <memory>
#include <vector>
#include <cstdint>


class CerealWriteArchive;
class CerealReadArchive;


using WriteArchive = CerealWriteArchive;
using ReadArchive = CerealReadArchive;


std::unique_ptr<IArchive> createWriteArchive();
std::unique_ptr<IArchive> createReadArchive(const uint8_t* data, size_t length);

std::vector<uint8_t> getArchiveBytes(IArchive* archive);