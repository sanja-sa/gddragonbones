#ifndef GDTEXTUREDATA_H
#define GDTEXTUREDATA_H

#include <memory>
#include <dragonBones/DragonBonesHeaders.h>
#include "core/math/math_2d.h"
#include "scene/resources/texture.h"

DRAGONBONES_NAMESPACE_BEGIN


class GDTextureData : public TextureData
{
	BIND_CLASS_TYPE_B(GDTextureData);

public:	
    Rect2i				textureRect;

public:
	GDTextureData()
	{
		_onClear();
	}

	virtual ~GDTextureData()
	{
		_onClear();
	}

	void _onClear() override
	{
		TextureData::_onClear();
	}
};

DRAGONBONES_NAMESPACE_END

#endif // GDTEXTUREDATA_H
                                
