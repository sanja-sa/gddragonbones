#ifndef GDTEXTUREATLAS_H
#define GDTEXTUREATLAS_H

#include <dragonBones/DragonBonesHeaders.h>
#include "scene/resources/texture.h"

DRAGONBONES_NAMESPACE_BEGIN

class GDTextureAtlasData : public TextureAtlasData
{
	BIND_CLASS_TYPE_B(GDTextureAtlasData);

public:
	GDTextureAtlasData();
	~GDTextureAtlasData();

    virtual TextureData* createTexture() const override;

    void setRenderTexture();
};

DRAGONBONES_NAMESPACE_END

#endif // GDTEXTUREATLAS_H
