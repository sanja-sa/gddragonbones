#pragma once

#include "dragonBones/model/TextureAtlasData.h"
#include "godot_cpp/variant/rect2i.hpp"
namespace godot {

class GDTextureData : public dragonBones::TextureData {
	BIND_CLASS_TYPE_B(GDTextureData);

public:
	Rect2i textureRect; // ?? TODO: 有啥用？

public:
	GDTextureData() {
		_onClear();
	}

	virtual ~GDTextureData() {
		_onClear();
	}

	void _onClear() override {
		TextureData::_onClear();
	}
};

} //namespace godot