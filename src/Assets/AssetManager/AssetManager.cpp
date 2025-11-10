//
// Created by Lieven Schokker on 10/11/2025.
//

#include "AssetManager.h"

void AssetManager::Add(const std::string &fileName, std::unique_ptr<Asset> asset)
{
	assets[fileName] = std::move(asset);
}

void AssetManager::Remove(const std::string& fileName)
{
	assets.erase(fileName);
}

bool AssetManager::Has(const std::string& fileName) const
{
	return assets.contains(fileName);
}

Asset* AssetManager::Get(const std::string&fileName)
{
	auto it = assets.find(fileName);
	if (it != assets.end()) {
		return it->second.get();
	}
	return nullptr;
}

bool AssetManager::Load(const std::string &fileName)
{
	Asset* asset = Get(fileName);

	if (asset == nullptr)
	{
		return false;
	}

	if(asset->IsLoaded() == false)
	{
		return asset->Load(fileName);
	}
	return true;
}

bool AssetManager::Unload(const std::string &fileName)
{
	Asset* asset = Get(fileName);

	if (asset == nullptr)
	{
		return false;
	}

	if(asset->IsLoaded() == false)
	{
		return asset->UnLoad();
	}
	return true;
}

