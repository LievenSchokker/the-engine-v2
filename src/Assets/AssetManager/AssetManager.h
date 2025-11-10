//
// Created by Lieven Schokker on 10/11/2025.
//

#ifndef THE_ENGINE_REPO_ASSETMANAGER_H
#define THE_ENGINE_REPO_ASSETMANAGER_H

#include <unordered_set>
#include "Assets/Asset.h"

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

	void Add(const std::string &fileName, std::unique_ptr<Asset> asset);

	void Remove(const std::string &fileName);

	bool Has(const std::string &fileName) const;

	Asset *Get(const std::string &fileName);

	bool Load(const std::string &fileName);

	bool Unload(const std::string &fileName);

private:
	std::unordered_map<std::string, std::unique_ptr<Asset> > assets;
};


#endif //THE_ENGINE_REPO_ASSETMANAGER_H
