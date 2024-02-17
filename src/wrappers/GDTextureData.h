#pragma once

#include "godot_cpp/variant/rect2i.hpp"
#include <dragonBones/DragonBonesHeaders.h>

DRAGONBONES_NAMESPACE_BEGIN

class GDTextureData : public TextureData {
	BIND_CLASS_TYPE_B(GDTextureData);

public:
	godot::Rect2i textureRect{};

public:
	GDTextureData() {
		_onClear();
	}

	virtual ~GDTextureData() {
		_onClear();
	}

	virtual void _onClear() override {
		TextureData::_onClear();
	}
};

DRAGONBONES_NAMESPACE_END
