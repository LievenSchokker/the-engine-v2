#pragma once


#include "AssetTypes.h"

#include <string>

class Asset
{
public:
    virtual ~Asset() = default;

    virtual bool load(const std::string& filepath) = 0;

    virtual bool unLoad() = 0;

    virtual bool isLoaded() const = 0;

    virtual bool reload()
    {
        if (filepath.empty()) return false;
        return load(filepath);
    }

    AssetType getAssetType() { return assetType; }

    std::string getAssetpath() { return filepath; }

protected:
    AssetType assetType = AssetType::None;
    std::string filepath;
};
