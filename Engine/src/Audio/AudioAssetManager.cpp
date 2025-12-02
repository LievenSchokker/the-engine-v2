#include "Audio/AudioAssetManager.h"
#include "Audio/IAudioBackend.h"

#include <iostream>
#include <map>

struct MusicAsset
{
    MusicHandle handle = -1;
    int usageCount = 0;
};

struct SoundAsset
{
    SoundHandle handle = -1;
    int usageCount = 0;
};

SoundHandle AudioAssetManager::loadSound(const std::string& path)
{
    static std::map<std::string, SoundAsset> soundMap;

    // Check if already loaded
    auto it = soundMap.find(path);
    if (it != soundMap.end())
    {
        it->second.usageCount++;
        return it->second.handle;
    }

    if (!backend)
        return -1;

    SoundHandle handle = backend->loadSound(path);
    if (handle == -1)
    {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return -1;
    }

    soundMap[path] = {handle, 1};
    return handle;
}

MusicHandle AudioAssetManager::loadMusic(const std::string& path)
{
    static std::map<std::string, MusicAsset> musicMap;

    // Check if already loaded
    auto it = musicMap.find(path);
    if (it != musicMap.end())
    {
        it->second.usageCount++;
        return it->second.handle;
    }

    if (!backend)
        return -1;

    MusicHandle handle = backend->loadMusic(path);
    if (handle == -1)
    {
        std::cerr << "Failed to load music: " << path << std::endl;
        return -1;
    }

    musicMap[path] = {handle, 1};
    return handle;
}

void AudioAssetManager::releaseSound(SoundHandle handle)
{
    static std::map<std::string, SoundAsset>& soundMap = *new std::map<std::string, SoundAsset>();

    for (auto it = soundMap.begin(); it != soundMap.end(); ++it)
    {
        if (it->second.handle == handle)
        {
            it->second.usageCount--;
            if (it->second.usageCount <= 0)
            {
                backend->unloadSound(handle);
                soundMap.erase(it);
            }
            return;
        }
    }
}

void AudioAssetManager::releaseMusic(MusicHandle handle)
{
    static std::map<std::string, MusicAsset>& musicMap = *new std::map<std::string, MusicAsset>();

    for (auto it = musicMap.begin(); it != musicMap.end(); ++it)
    {
        if (it->second.handle == handle)
        {
            it->second.usageCount--;
            if (it->second.usageCount <= 0)
            {
                backend->unloadMusic(handle);
                musicMap.erase(it);
            }
            return;
        }
    }
}

void AudioAssetManager::unloadUnused()
{
    // Could iterate over all assets and remove unused
    // For simplicity, leave to releaseSound/releaseMusic
}

void AudioAssetManager::clear()
{
    static std::map<std::string, SoundAsset>& soundMap = *new std::map<std::string, SoundAsset>();
    static std::map<std::string, MusicAsset>& musicMap = *new std::map<std::string, MusicAsset>();

    for (auto& [_, asset] : soundMap)
        backend->unloadSound(asset.handle);
    soundMap.clear();

    for (auto& [_, asset] : musicMap)
        backend->unloadMusic(asset.handle);
    musicMap.clear();
}
