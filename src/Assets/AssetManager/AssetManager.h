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
	void add(const std::string& id, std::unique_ptr<Asset> asset);
	Asset* get(const std::string& id);
	void load(const std::string& id);
	void unload(const std::string& id);

private:
	std::unordered_map<std::string, std::unique_ptr<Asset>> assets;

};


#endif //THE_ENGINE_REPO_ASSETMANAGER_H