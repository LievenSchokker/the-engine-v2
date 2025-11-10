//
// Created by Lieven Schokker on 10/11/2025.
//

#ifndef THE_ENGINE_REPO_ASSETMANAGER_H
#define THE_ENGINE_REPO_ASSETMANAGER_H

#include <unordered_set>
#include "../Asset.h"

class AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;


	//Don't Think we want to copy the AssetManager But I am not 100% sure about this
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

	void Add(const std::string &filePath, std::unique_ptr<Asset> asset);

	void Remove(const std::string &filePath);

	bool Has(const std::string &filePath) const;

	Asset *Get(const std::string &filePath);

	bool Load(const std::string &filePath);

	bool Unload(const std::string &filePath);

private:
	std::unordered_map<std::string, std::unique_ptr<Asset> > assets;
};


#endif //THE_ENGINE_REPO_ASSETMANAGER_H
