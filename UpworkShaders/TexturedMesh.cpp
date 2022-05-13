#include "TexturedMesh.h"

TexturedMesh::TexturedMesh(Mesh* mesh, Texture* texture) :
    mesh_(mesh),
    texture_(texture)
{
}

Mesh* TexturedMesh::GetMesh() const
{
    return mesh_;
}

Texture* TexturedMesh::GetTexture() const
{
    return texture_;
}

void TexturedMesh::SetMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

void TexturedMesh::SetTexture(Texture* texture)
{
    texture_ = texture;
}
