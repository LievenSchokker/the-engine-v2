#pragma once

#include "Scene/Scene.h"

#include <memory>
#include <string>
#include <vector>

/**
 * @class SaveSceneSystem
 * @brief Handles saving and loading scenes to/from files.
 *
 * Supports both single scene files (.scene) and multi-scene files (.scenes).
 * Uses the Scene's serialization methods to persist scene data,
 * including all GameObjects and their parent-child relationships.
 *
 * File naming conventions:
 *   - Single scene:    "levelName.scene"
 *   - Multiple scenes: "projectName.scenes"
 *
 * File format:
 *   - Header: magic number (4 bytes) + version (4 bytes) + scene count (4 bytes)
 *   - Body: serialized scene data
 */
class SaveSceneSystem
{
public:
	/// File extension for single scene files
	static constexpr const char* SCENE_EXTENSION = ".scene";

	/// File extension for multi-scene files
	static constexpr const char* SCENES_EXTENSION = ".scenes";

	/// Current file format version
	static constexpr uint32_t FILE_VERSION = 1;

	/// Magic number to identify scene files ("SCNE")
	static constexpr uint32_t SCENE_HEADER_NUMBER = 0x454E4353;

	/**
	 * @brief Saves a single scene to a file.
	 * @param scene The scene to save.
	 * @param filePath The file path to save to.
	 * @return true if save was successful, false otherwise.
	 */
	static bool saveScene(const Scene& scene, const std::string& filePath);

	/**
	 * @brief Saves multiple scenes to a single file.
	 * @param scenes Vector of scenes to save.
	 * @param filePath The file path to save to.
	 * @return true if save was successful, false otherwise.
	 */
	static bool saveScenes(const std::vector<const Scene*>& scenes,
	                       const std::string& filePath);

	/**
	 * @brief Loads a single scene from a file.
	 * @param filePath The file path to load from.
	 * @return A unique_ptr to the loaded scene, or nullptr on failure.
	 */
	static std::unique_ptr<Scene> loadScene(const std::string& filePath);

	/**
	 * @brief Loads all scenes from a multi-scene file.
	 * @param filePath The file path to load from.
	 * @return Vector of loaded scenes (may be empty on failure).
	 */
	static std::vector<std::unique_ptr<Scene>> loadScenes(
		const std::string& filePath);

	/**
	 * @brief Checks if a file appears to be a valid scene file.
	 * @param filePath The file path to check.
	 * @return true if file has valid scene header, false otherwise.
	 */
	static bool isValidSceneFile(const std::string& filePath);

	/**
	 * @brief Checks if a scene file exists at the given path.
	 * @param filePath The file path to check.
	 * @return true if file exists, false otherwise.
	 */
	static bool exists(const std::string& filePath);

	/**
	 * @brief Gets the number of scenes stored in a file.
	 * @param filePath The file path to check.
	 * @return Number of scenes, or 0 if file is invalid.
	 */
	static uint32_t getSceneCount(const std::string& filePath);

private:
	/**
* @brief Generates a file path with the appropriate extension.
* @param baseName Base name without extension (e.g., "level1").
* @param multiScene true for .scenes extension, false for .scene.
* @return Complete file path with extension.
*/
	static std::string makeFilePath(const std::string& baseName,
									bool multiScene = false);

	/**
	 * @brief Generates a file path using the scene's name.
	 * @param scene The scene to generate a path for.
	 * @param directory Optional directory prefix.
	 * @return Complete file path with extension.
	 */
	static std::string makeFilePath(const Scene& scene,
									const std::string& directory = "");
	
	static bool writeHeader(WriteArchive& archive, uint32_t sceneCount);
	static bool readHeader(ReadArchive& archive, uint32_t& sceneCount);
	static bool writeToFile(const std::vector<std::byte>& bytes,
	                        const std::string& filePath);
	static std::vector<std::byte> readFromFile(const std::string& filePath);
};