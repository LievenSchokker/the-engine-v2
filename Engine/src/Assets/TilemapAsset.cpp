#include "Assets/TilemapAsset.h"
#include "Assets/AssetTypes.h"

#include <fstream>
#include <iostream>
#include <sstream>

TilemapAsset::TilemapAsset()
{
	assetType = AssetType::Tilemap;
}

bool TilemapAsset::load(const std::string& filepath)
{
	unLoad();

	std::ifstream file(filepath);
	if ( !file.is_open() ) {
		std::cerr << "[TilemapAsset] Failed to open file: " << filepath
				  << std::endl;
		return false;
	}

	tiles.clear();
	width = 0;
	height = 0;

	std::string line;
	while ( std::getline(file, line) ) {
		// Skip empty lines
		if ( line.empty() ||
			 (line.find_first_not_of(" \t\r\n") == std::string::npos) ) {
			continue;
		}

		std::vector<int> row;
		std::stringstream ss(line);
		std::string cell;

		while ( std::getline(ss, cell, ',') ) {
			// Trim whitespace
			cell.erase(0, cell.find_first_not_of(" \t"));
			cell.erase(cell.find_last_not_of(" \t") + 1);

			if ( cell.empty() ) {
				continue;
			}

			try {
				int tileId = std::stoi(cell);
				row.push_back(tileId);
			} catch ( const std::exception& e ) {
				std::cerr << "[TilemapAsset] Failed to parse tile value: "
						  << cell << std::endl;
				row.push_back(0);
			}
		}

		if ( !row.empty() ) {
			if ( width == 0 ) {
				width = static_cast<int>(row.size());
			} else if ( static_cast<int>(row.size()) != width ) {
				std::cerr << "[TilemapAsset] Warning: Row " << height
						  << " has inconsistent width. Expected " << width
						  << ", got " << row.size() << std::endl;
				// Pad or truncate to match width
				row.resize(width, 0);
			}

			tiles.insert(tiles.end(), row.begin(), row.end());
			height++;
		}
	}

	file.close();

	if ( width == 0 || height == 0 ) {
		std::cerr << "[TilemapAsset] Invalid tilemap: width or height is 0"
				  << std::endl;
		return false;
	}

	loaded = true;
	return true;
}

bool TilemapAsset::unLoad()
{
	tiles.clear();
	width = 0;
	height = 0;
	loaded = false;
	return true;
}

bool TilemapAsset::isLoaded() const
{
	return loaded;
}

int TilemapAsset::getWidth() const
{
	return width;
}

int TilemapAsset::getHeight() const
{
	return height;
}

int TilemapAsset::getTile(int x, int y) const
{
	if ( x < 0 || x >= width || y < 0 || y >= height ) {
		return 0;
	}
	return tiles[y * width + x];
}

int TilemapAsset::getTile(Vector2 cell) const
{
	return getTile(static_cast<int>(cell.x()), static_cast<int>(cell.y()));
}

bool TilemapAsset::hasTile(int x, int y) const
{
	return getTile(x, y) != 0;
}

const std::vector<int>& TilemapAsset::getTiles() const
{
	return tiles;
}
