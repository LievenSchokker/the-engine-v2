//
// Created by Lieven Schokker on 10/11/2025.
//

#include "AssetManager.h"

void AssetManager::Add(const std::string &fileName, std::unique_ptr<Asset> asset)
{
	if (Get(fileName) == nullptr)
	{
		assets[fileName] = std::move(asset);
	}
}

void AssetManager::Remove(const std::string& filePath)
{
	assets.erase(filePath);
}

bool AssetManager::Has(const std::string& filePath) const
{
	return assets.contains(filePath);
}

Asset* AssetManager::Get(const std::string&filePath)
{
	auto it = assets.find(filePath);
	if (it != assets.end()) {
		return it->second.get();
	}
	return nullptr;
}

bool AssetManager::Load(const std::string &filePath)
{
	Asset* asset = Get(filePath);

	if (asset == nullptr)
	{
		return false;
	}

	if(asset->IsLoaded() == false)
	{
		return asset->Load(filePath);
	}
	return true;
}

bool AssetManager::Unload(const std::string &filePath)
{
	Asset* asset = Get(filePath);

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

