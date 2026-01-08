#pragma once
#include "Networking/Serialization/ISerializable.h"
#include "Networking/Serialization/Serialization.h"

/**
 * @struct SpritesheetDefinition
 * @brief Describes the grid layout of a spritesheet for frame-based animation.
 *
 * Defines how frames are arranged in a spritesheet image using a grid system.
 * Frames are read left-to-right, top-to-bottom (row-major order).
 */
struct SpritesheetDefinition : ISerializable
{

    SpritesheetDefinition() = default;
    SpritesheetDefinition(int row, int colum, int frameWid, int frameHeig)
        : rows(row), columns(colum), frameWidth(frameWid), frameHeight(frameHeig) {}

    /**
     * @brief Number of rows in the spritesheet grid.
     */
    int rows = 0;

    /**
     * @brief Number of columns in the spritesheet grid.
     */
    int columns = 0;

    /**
     * @brief Width of each frame in pixels.
     */
    int frameWidth = 0;

    /**
     * @brief Height of each frame in pixels.
     */
    int frameHeight = 0;

    /**
     * @brief Gets the total number of frames in the spritesheet.
     * @return Total frames (rows * columns)
     */
    int getTotalFrames() const
    {
        return rows * columns;
    }

    void serialize(WriteArchive& archive) const override
    {
        int32_t r = rows;
        int32_t c = columns;
        int32_t fw = frameWidth;
        int32_t fh = frameHeight;

        archive.process(r);
        archive.process(c);
        archive.process(fw);
        archive.process(fh);
    }

    void deserialize(ReadArchive& archive) override
    {
        int32_t r, c, fw, fh;

        archive.process(r);
        archive.process(c);
        archive.process(fw);
        archive.process(fh);

        rows = r;
        columns = c;
        frameWidth = fw;
        frameHeight = fh;
    }
};