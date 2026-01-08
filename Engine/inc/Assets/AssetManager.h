#pragma once


#include <memory>
#include <unordered_map>
#include <string>
#include "Asset.h"


class AssetManager
{
public:
	AssetManager() = default;

	~AssetManager();

	void add(const std::string & filePath, std::unique_ptr<Asset> asset);

	void remove(const std::string & filePath);

	bool has(const std::string & filePath) const;

	Asset* get(const std::string & filePath);

	bool load(const std::string & filePath);

	bool unLoad(const std::string & filePath);

private:
	std::unordered_map<std::string, std::unique_ptr<Asset>> assets;
};


