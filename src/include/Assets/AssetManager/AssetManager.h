///
/// Created by Lieven Schokker on 10/11/2025.
///

#pragma once


#include <unordered_map>
#include <unordered_set>


class Asset;
class AssetManager
{
public:
	AssetManager() = default;

	~AssetManager() = default;

	void add(const std::string & filePath, std::unique_ptr<Asset> asset);

	void remove(const std::string & filePath);

	bool has(const std::string & filePath) const;

	Asset* get(const std::string & filePath);

	bool load(const std::string & filePath);

	bool unload(const std::string & filePath);

private:
	std::unordered_map<std::string, std::unique_ptr<Asset> > assets;
};


