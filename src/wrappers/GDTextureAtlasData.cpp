#include "GDTextureAtlasData.h"
#include "dragonBones/model/TextureAtlasData.h"

using namespace godot;

GDTextureAtlasData::GDTextureAtlasData() {
	_onClear();
}

GDTextureAtlasData::~GDTextureAtlasData() {
	_onClear();
}

void GDTextureAtlasData::setRenderTexture() {
	// for (const auto &pair : textures) {
	// 	const auto textureData = static_cast<GDTextureData *>(pair.second);
	// 	Rect2i rect(textureData->region.x, textureData->region.y,
	// 			textureData->rotated ? textureData->region.height : textureData->region.width,
	// 			textureData->rotated ? textureData->region.width : textureData->region.height);
	// 	textureData->textureRect = std::move(rect);
	// }
}

dragonBones::TextureData *GDTextureAtlasData::createTexture() const {
	return BaseObject::borrowObject<GDTextureData>();
}
