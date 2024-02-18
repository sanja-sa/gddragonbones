#pragma once

#include "dragonBones/model/TextureAtlasData.h"
namespace godot {

class GDTextureAtlasData : public dragonBones::TextureAtlasData {
	BIND_CLASS_TYPE_B(GDTextureAtlasData);

public:
	GDTextureAtlasData();
	~GDTextureAtlasData();

	virtual dragonBones::TextureData *createTexture() const override;

	void setRenderTexture();
};

} //namespace godot