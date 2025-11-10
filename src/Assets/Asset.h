//
// Created by Lieven Schokker on 10/11/2025.
//

#ifndef GAMEENGINE_ASSET_H
#define GAMEENGINE_ASSET_H
#include <string>

#include "FileTypes/AssetTypes.h"

class Asset
{
	~Asset() = default;

public:
	virtual bool Load(const std::string &filepath) = 0;

	virtual bool UnLoad() = 0;

	virtual bool IsLoaded() const = 0;

	AssetType GetAssetType() { return m_assetType; }

private:
	AssetType m_assetType = AssetType::None;
};

#endif //GAMEENGINE_ASSET_H
