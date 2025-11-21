///
/// Created by Lieven Schokker on 10/11/2025.
///

#pragma once


#include <string>
#include "AssetTypes.h"


class Asset
{
public:
	virtual ~Asset() = default;

	virtual bool load(const std::string & filepath) = 0;

	virtual bool unLoad() = 0;

	virtual bool isLoaded() const = 0;

	AssetType getAssetType() { return assetType; }

protected:
	AssetType assetType = AssetType::None;
};
