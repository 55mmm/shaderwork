#pragma once

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <string>

class SceneLoader
{
public:
    const aiScene* LoadScene(const std::string& file);
private:
    Assimp::Importer mImporter;
};