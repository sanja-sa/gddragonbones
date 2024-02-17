#pragma once

#include "GDDisplay.h"
#include "dragonBones/core/DragonBones.h"
#include "godot_cpp/variant/vector2.hpp"
#include "wrappers/GDTextureData.h"

DRAGONBONES_NAMESPACE_BEGIN

class GDSlot : public Slot {
	BIND_CLASS_TYPE_A(GDSlot);

private:
	float _textureScale;
	GDDisplay *_renderDisplay;

public:
	inline GDTextureData *get_texture_data() const { return static_cast<GDTextureData *>(_textureData); }

	inline Rectangle *get_atlas_texture_region() const {
		return get_texture_data()->getRegion();
	}

	virtual void _updateVisible() override;
	virtual void _updateBlendMode() override;
	virtual void _updateColor() override;

protected:
	virtual void _initDisplay(void *value, bool isRetain) override;
	virtual void _disposeDisplay(void *value, bool isRelease) override;
	virtual void _onUpdateDisplay() override;
	virtual void _addDisplay() override;
	virtual void _replaceDisplay(void *value, bool isArmatureDisplay) override;
	virtual void _removeDisplay() override;
	virtual void _updateZOrder() override;

	virtual void _updateFrame() override;
	virtual void _updateMesh() override;
	virtual void _updateTransform() override;
	virtual void _identityTransform() override;

	virtual void _onClear() override;

	static void __get_uv_pt(godot::Point2 &_pt, bool _is_rot, float _u, float _v, const Rectangle &_reg, const TextureAtlasData *_p_atlas);
};

DRAGONBONES_NAMESPACE_END
