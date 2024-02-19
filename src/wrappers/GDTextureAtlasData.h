#pragma once

#include "dragonBones/model/TextureAtlasData.h"
namespace godot {
class GDTextureData : public dragonBones::TextureData {
	BIND_CLASS_TYPE_B(GDTextureData);

public:
	GDTextureData() {
		_onClear();
	}

	virtual ~GDTextureData() {
		_onClear();
	}
};

class GDTextureAtlasData : public dragonBones::TextureAtlasData {
	BIND_CLASS_TYPE_B(GDTextureAtlasData);

public:
	GDTextureAtlasData();
	~GDTextureAtlasData();

	virtual dragonBones::TextureData *createTexture() const override;

	void setRenderTexture();
};

} //namespace godot