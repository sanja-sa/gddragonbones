#ifndef GDTEXTUREDATA_H
#define GDTEXTUREDATA_H

#include <memory>
#include <dragonBones/DragonBonesHeaders.h>
#include "scene/resources/texture.h"
#include "core/version.h"

#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
#include <core/math/transform_2d.h>
#include <core/math/vector2.h>
#include <core/math/rect2.h>

#else
#include "core/math/math_2d.h"

#endif

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
                                
