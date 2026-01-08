///
/// Created by Lieven Schokker on 10/11/2025.
///


#include "Assets/AssetManager.h"
#include "Assets/Asset.h"

AssetManager::~AssetManager() = default;

void AssetManager::add(const std::string & fileName, std::unique_ptr<Asset> asset)
{
	if(get(fileName) == nullptr)
	{
		assets[fileName] = std::move(asset);
	}
}


void AssetManager::remove(const std::string & filePath)
{
	assets.erase(filePath);
}


bool AssetManager::has(const std::string & filePath) const
{
	return assets.contains(filePath);
}


Asset* AssetManager::get(const std::string & filePath)
{
	auto it = assets.find(filePath);

	if(it != assets.end())
	{
		return it->second.get();
	}

	return nullptr;
}


bool AssetManager::load(const std::string & filePath)
{
	Asset* asset = get(filePath);

	if(asset == nullptr)
	{
		return false;
	}

	asset->setAssetpath(filePath);
	if(!asset->isLoaded())
	{
		return asset->load(filePath);
	}

	return true;
}


bool AssetManager::unLoad(const std::string & filePath)
{
	Asset* asset = get(filePath);

	if(asset == nullptr)
	{
		return false;
	}
	asset->setAssetpath("");
	if(asset->isLoaded())
	{
		return asset->unLoad();
	}

	return true;
}

