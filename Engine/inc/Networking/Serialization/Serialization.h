#pragma once


#include "CerealReadArchive.h"
#include "CerealWriteArchive.h"

/**
 * @file Serialization.h
 * @brief Provides serialization type aliases for the networking layer.
 *
 * These aliases exist to create a single point of change for the entire codebase's
 * serialization strategy. All network code uses WriteArchive and ReadArchive rather
 * than the concrete Cereal implementations directly.
 *
 * This indirection enables swapping serialization backends
 * without modifying any calling code. Only this file would need updates.
 *
 */

/**
 * @typedef WriteArchive
 * @brief The serialization archive used throughout the networking layer.
 *
 * Aliased separately from ReadArchive because write and read implementations
 * could theoretically diverge.
 *
 */
using WriteArchive = CerealWriteArchive;

/**
 * @typedef ReadArchive
 * @brief The deserialization archive used throughout the networking layer.
 *
 *
 */
using ReadArchive = CerealReadArchive;