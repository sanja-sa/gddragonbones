#pragma once

#include "dragonBones/model/TextureAtlasData.h"
namespace godot {
class GDTextureData : public dragonBones::TextureData {
	BIND_CLASS_TYPE_B(GDTextureData);

public:
	GDTextureData() { _onClear(); }
	virtual ~GDTextureData() { _onClear(); }
};

class GDTextureAtlasData : public dragonBones::TextureAtlasData {
	BIND_CLASS_TYPE_B(GDTextureAtlasData);

public:
	GDTextureAtlasData() { _onClear(); }
	virtual ~GDTextureAtlasData() { _onClear(); }

	virtual dragonBones::TextureData *createTexture() const override {
		return BaseObject::borrowObject<GDTextureData>();
	}

	void setRenderTexture() {
		// for (const auto &pair : textures) {
		// 	const auto textureData = static_cast<GDTextureData *>(pair.second);
		// 	Rect2i rect(textureData->region.x, textureData->region.y,
		// 			textureData->rotated ? textureData->region.height : textureData->region.width,
		// 			textureData->rotated ? textureData->region.width : textureData->region.height);
		// 	textureData->textureRect = std::move(rect);
		// }
	}
};

} //namespace godot