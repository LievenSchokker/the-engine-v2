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
        if (assetPath.empty()) return false;
        return load(assetPath);
    }

    AssetType getAssetType() { return assetType; }

    std::string getAssetpath() { return assetPath; }
    void setAssetpath(const std::string& path) { assetPath = path; }
protected:
    AssetType assetType = AssetType::None;
    std::string assetPath;
};
