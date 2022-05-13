#pragma once

#include "Mesh.h"
#include "Texture.h"

class TexturedMesh
{
public:

	TexturedMesh(Mesh* mesh, Texture* texture);
	virtual ~TexturedMesh() = default;

	Mesh* GetMesh() const;
	Texture* GetTexture() const;

    void SetMesh(Mesh* mesh);
    void SetTexture(Texture* texture);

private:

	Mesh* mesh_;
	Texture* texture_;

};
