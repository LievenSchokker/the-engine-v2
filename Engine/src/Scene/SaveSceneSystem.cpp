#include "Scene/SaveSceneSystem.h"

#include "Networking/Serialization/Serialization.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

bool SaveSceneSystem::saveScene(const Scene& scene, const std::string& filePath)
{
	const std::vector scenes = {&scene};
	return saveScenes(scenes, filePath);
}

bool SaveSceneSystem::saveScenes(const std::vector<const Scene*>& scenes,
                                 const std::string& filePath)
{
	if (scenes.empty())
	{
		std::cerr << "[SaveSceneSystem] No scenes to save" << std::endl;
		return false;
	}

	WriteArchive archive;

	// Write file header
	if (!writeHeader(archive, static_cast<uint32_t>(scenes.size())))
	{
		return false;
	}

	for (const Scene* scene : scenes)
	{
		if (scene == nullptr)
		{
			return false;
		}
		scene->serialize(archive);
	}

	return writeToFile(archive.getBytes(), filePath);
}

std::unique_ptr<Scene> SaveSceneSystem::loadScene(const std::string& filePath)
{
	auto scenes = loadScenes(filePath);

	if (scenes.empty())
	{
		return nullptr;
	}

	if (scenes.size() > 1)
	{
		std::cerr <<
			"[SaveSceneSystem] Warning: File contains multiple scenes, "
			<< "only first scene loaded. Use loadScenes() to load all."
			<< std::endl;
	}

	return std::move(scenes[0]);
}

std::vector<std::unique_ptr<Scene>> SaveSceneSystem::loadScenes(
	const std::string& filePath)
{
	std::vector<std::unique_ptr<Scene>> result;

	std::vector<std::byte> bytes = readFromFile(filePath);
	if (bytes.empty())
	{
		return result;
	}

	ReadArchive archive(bytes.data(), bytes.size());

	uint32_t sceneCount = 0;
	if (!readHeader(archive, sceneCount))
	{
		std::cerr << "[SaveSceneSystem] Invalid file header: " << filePath <<
			std::endl;
		return result;
	}

	result.reserve(sceneCount);
	for (uint32_t i = 0; i < sceneCount; ++i)
	{
		auto scene = std::make_unique<Scene>("loading");
		scene->deserialize(archive);
		result.push_back(std::move(scene));
	}

	return result;
}


std::string SaveSceneSystem::makeFilePath(const std::string& baseName,
                                          const bool multiScene)
{
	std::string path = baseName;
	const std::string sceneExt = SCENE_EXTENSION;
	const std::string scenesExt = SCENES_EXTENSION;

	if (path.size() > sceneExt.size() &&
	    path.compare(path.size() - sceneExt.size(), sceneExt.size(),
	                 sceneExt) == 0)
	{
		path = path.substr(0, path.size() - sceneExt.size());
	}
	else if (path.size() > scenesExt.size() &&
	         path.compare(path.size() - scenesExt.size(), scenesExt.size(),
	                      scenesExt) == 0)
	{
		path = path.substr(0, path.size() - scenesExt.size());
	}

	return path + (multiScene ? SCENES_EXTENSION : SCENE_EXTENSION);
}

std::string SaveSceneSystem::makeFilePath(const Scene& scene,
                                          const std::string& directory)
{
	std::string sceneName = scene.getName();

	std::ranges::replace(sceneName, ' ', '_');
	std::ranges::replace(sceneName, '/', '_');
	std::ranges::replace(sceneName, '\\', '_');

	sceneName.erase(
		std::ranges::remove_if(sceneName,
		                       [](const char c)
		                       {
			                       return c == ':' || c == '*' || c == '?' ||
			                              c == '"' || c == '<' || c == '>' || c
			                              == '|';
		                       }).begin(),
		sceneName.end());

	if (sceneName.empty())
	{
		sceneName = "unnamed_scene";
	}

	std::string path;
	if (!directory.empty())
	{
		path = directory;
		if (path.back() != '/' && path.back() != '\\')
		{
			path += '/';
		}
	}

	return path + sceneName + SCENE_EXTENSION;
}

bool SaveSceneSystem::isValidSceneFile(const std::string& filePath)
{
	if (!std::filesystem::exists(filePath))
	{
		return false;
	}

	std::vector<std::byte> bytes = readFromFile(filePath);
	if (bytes.size() < 12)
	{
		return false;
	}

	ReadArchive archive(bytes.data(), bytes.size());
	uint32_t sceneCount = 0;
	return readHeader(archive, sceneCount);
}

bool SaveSceneSystem::exists(const std::string& filePath)
{
	return std::filesystem::exists(filePath);
}

uint32_t SaveSceneSystem::getSceneCount(const std::string& filePath)
{
	std::vector<std::byte> bytes = readFromFile(filePath);
	if (bytes.size() < 12)
	{
		return 0;
	}

	ReadArchive archive(bytes.data(), bytes.size());
	uint32_t sceneCount = 0;
	if (!readHeader(archive, sceneCount))
	{
		return 0;
	}
	return sceneCount;
}

bool SaveSceneSystem::writeHeader(WriteArchive& archive, uint32_t sceneCount)
{
	uint32_t magic = SCENE_HEADER_NUMBER;
	uint32_t version = FILE_VERSION;

	archive.process(magic);
	archive.process(version);
	archive.process(sceneCount);

	return true;
}

bool SaveSceneSystem::readHeader(ReadArchive& archive, uint32_t& sceneCount)
{
	uint32_t magic = 0;
	uint32_t version = 0;

	archive.process(magic);
	archive.process(version);
	archive.process(sceneCount);

	if (magic != SCENE_HEADER_NUMBER)
	{
		return false;
	}

	if (version > FILE_VERSION)
	{
		return false;
	}

	return true;
}

bool SaveSceneSystem::writeToFile(const std::vector<std::byte>& bytes,
                                  const std::string& filePath)
{
	std::filesystem::path path(filePath);
	if (path.has_parent_path())
	{
		std::error_code ec;
		std::filesystem::create_directories(path.parent_path(), ec);
		if (ec)
		{
			return false;
		}
	}

	std::ofstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(reinterpret_cast<const char*>(bytes.data()),
	           static_cast<std::streamsize>(bytes.size()));

	if (!file.good())
	{
		return false;
	}

	file.close();
	return true;
}

std::vector<std::byte> SaveSceneSystem::readFromFile(
	const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return {};
	}

	std::streamsize size = file.tellg();
	if (size <= 0)
	{
		return {};
	}

	file.seekg(0, std::ios::beg);

	std::vector<std::byte> bytes(static_cast<size_t>(size));
	file.read(reinterpret_cast<char*>(bytes.data()), size);

	if (!file.good())
	{
		return {};
	}

	file.close();
	return bytes;
}