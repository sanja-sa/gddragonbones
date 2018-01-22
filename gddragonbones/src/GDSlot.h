#ifndef GDSLOT_H
#define GDSLOT_H

#include <memory>
#include "scene/2d/node_2d.h"
#include <dragonBones/DragonBonesHeaders.h>
#include "GDDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

class GDSlot : public Slot
{
	BIND_CLASS_TYPE_A(GDSlot);

private:
    float       _textureScale;
    GDDisplay*  _renderDisplay;

public:
	virtual void _updateVisible() override;
	virtual void _updateBlendMode() override;
	virtual void _updateColor() override;

protected:
    virtual void _initDisplay(void* value, bool isRetain) override;
    virtual void _disposeDisplay(void* value, bool isRelease) override;
	virtual void _onUpdateDisplay() override;
	virtual void _addDisplay() override;
	virtual void _replaceDisplay(void* value, bool isArmatureDisplay) override;
	virtual void _removeDisplay() override;
	virtual void _updateZOrder() override;

	virtual void _updateFrame() override;
	virtual void _updateMesh() override;
    virtual void _updateTransform() override;
    virtual void _identityTransform() override;

	virtual void _onClear() override;


    static void __get_uv_pt(Point2& _pt, bool _is_rot, float _u, float _v, const Rectangle& _reg, const TextureAtlasData *_p_atlas);
};

DRAGONBONES_NAMESPACE_END

#endif // GDSLOT_H
