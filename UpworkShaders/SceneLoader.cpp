#include "SceneLoader.h"

#include "Settings.h"

#include <stdexcept>

const aiScene* SceneLoader::LoadScene(const std::string& file)
{
    std::string path = Settings::resourcesPathPrefix + "models/";
    path += file;

    const aiScene* scene = mImporter.ReadFile(
        path.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_SortByPType |
        aiProcessPreset_TargetRealtime_MaxQuality
    );

    if (!scene)
        throw std::runtime_error(
            std::string("Failed to load \"") +
            path +
            "\"."
        );

    return scene;
}
