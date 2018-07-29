#include "GDSlot.h"
#include "GDMesh.h"
#include "GDArmatureDisplay.h"
#include "GDTextureAtlasData.h"
#include "GDTextureData.h"
#include "scene/2d/node_2d.h"

DRAGONBONES_NAMESPACE_BEGIN

#if (VERSION_MAJOR == 3)
    #define MATRIX_TRANSFORM  Transform2D
#else
    #define MATRIX_TRANSFORM  Matrix32
#endif

void GDSlot::_updateZOrder()
{
#if (VERSION_MAJOR == 3)
    _renderDisplay->set_z_index(_zOrder);
#else
    _renderDisplay->set_z(_zOrder);
#endif
}

void GDSlot::_updateVisible()
{
	if(_parent->getVisible())
		_renderDisplay->show();
	else
		_renderDisplay->hide();
}

void GDSlot::_updateBlendMode()
{
	if (_renderDisplay)
	{
        CanvasItem::BlendMode __blend = CanvasItem::BLEND_MODE_MIX;
#if (VERSION_MAJOR == 3)
#else
         GDOwnerNode* __p_owner = _renderDisplay->p_owner;
        if(__p_owner)
            __blend = __p_owner->get_blend_mode();
#endif
        if(!__blend)
        {
            switch (_blendMode)
            {
                case BlendMode::Normal:
                    __blend = CanvasItem::BLEND_MODE_MIX;
                    break;

                case BlendMode::Add:
                    __blend = CanvasItem::BLEND_MODE_ADD;
                    break;

                case BlendMode::Multiply:
                    __blend = CanvasItem::BLEND_MODE_MUL;
                    break;

                case BlendMode::Subtract:
                    __blend = CanvasItem::BLEND_MODE_SUB;
                    break;

                default:
                    break;
            }
        }
        _renderDisplay->set_blend_mode(__blend);
        _renderDisplay->update();
	}
	else if (_childArmature)
	{
		for (const auto slot : _childArmature->getSlots())
		{
			slot->_blendMode = _blendMode;
			slot->_updateBlendMode();
		}
	}
}

void GDSlot::_updateColor()
{
	if (!_renderDisplay) return;

    Color __color(	_colorTransform.redMultiplier,
            _colorTransform.greenMultiplier,
            _colorTransform.blueMultiplier,
            _colorTransform.alphaMultiplier);

   GDOwnerNode* __p_owner = _renderDisplay->p_owner;
   if(__p_owner)
   {
#if (VERSION_MAJOR == 3)
        __color.a *= __p_owner->modulate.a;
#else
       __color.a *= __p_owner->modulate.a * __p_owner->get_opacity();
#endif
       __color.r *= __p_owner->modulate.r;
       __color.g *= __p_owner->modulate.g;
       __color.b *= __p_owner->modulate.b;
   }

   _renderDisplay->set_modulate(__color);
   _renderDisplay->update();
}

void GDSlot::_initDisplay(void* value, bool isRetain)
{
}

void GDSlot::_disposeDisplay(void* value, bool isRelease)
{

}

void GDSlot::_onUpdateDisplay()
{
    _renderDisplay = static_cast<GDDisplay*>(_display != nullptr ? _display : _rawDisplay);
}

void GDSlot::_addDisplay()
{

}

void GDSlot::_replaceDisplay(void* value, bool isArmatureDisplay)
{
}

void GDSlot::_removeDisplay()
{

}

void GDSlot::__get_uv_pt(Point2& _pt, bool _is_rot, float _u, float _v, const Rectangle& _reg, const TextureAtlasData *_p_atlas)
{
    if (_is_rot)
    {
        _pt.x = (_reg.x + (1.f - _v) * _reg.width)/float(_p_atlas->width);
        _pt.y = (_reg.y + _u * _reg.height)/float(_p_atlas->height);
    }
    else
    {
        _pt.x = (_reg.x + _u * _reg.width)/float(_p_atlas->width);
        _pt.y = (_reg.y + _v * _reg.height)/float(_p_atlas->height);
    }
}

void GDSlot::_updateFrame()
{
        const auto currentVerticesData  = (_deformVertices != nullptr && _display == _meshDisplay) ? _deformVertices->verticesData : nullptr;
        auto currentTextureData = static_cast<GDTextureData*>(_textureData);


        if (_displayIndex >= 0 && _display != nullptr && currentTextureData != nullptr)
        {
            const auto atlas = currentTextureData->getParent();
            const auto& region = currentTextureData->region;
            auto frameDisplay = static_cast<GDMesh*>(_renderDisplay);

                if (currentVerticesData  != nullptr) // Mesh.
                {
                    const auto& deformVertices = _deformVertices->vertices;
                    const auto hasFFD = !deformVertices.empty();

                    const auto data = currentVerticesData->data;
                    const auto intArray = data->intArray;                     
                    const auto floatArray = data->floatArray;
                    const unsigned vertexCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
                    const unsigned triangleCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshTriangleCount];
                    int vertexOffset = intArray[currentVerticesData ->offset + (unsigned)BinaryOffset::MeshFloatOffset];

                    if (vertexOffset < 0)
                    {
                        vertexOffset += 65536; // Fixed out of bouds bug.
                    }

                    const unsigned uvOffset = vertexOffset + (vertexCount<<1);

                    frameDisplay->indices.resize(triangleCount * 3);
                    frameDisplay->verticesColor.resize(vertexCount);
                    frameDisplay->verticesUV.resize(vertexCount);
                    frameDisplay->verticesPos.resize(vertexCount);
                    Point2 __uv;
                    std::size_t iH;
                    float u,v;
                    for (std::size_t i = 0, l = (vertexCount<<1); i < l; i += 2)
                    {
                        iH = i>>1;
                        u = floatArray[uvOffset + i];
                        v = floatArray[uvOffset + i + 1];
                        __get_uv_pt(__uv, currentTextureData->rotated, u, v, region, atlas);
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
                        frameDisplay->verticesColor.write[iH] = Color(1,1,1,1);
                        frameDisplay->verticesUV.write[iH] = __uv;
                        frameDisplay->verticesPos.write[iH] = Point2(floatArray[vertexOffset + i],
						hasFFD * floatArray[vertexOffset + i + 1]);
#else
                        frameDisplay->verticesColor[iH] = Color(1,1,1,1);
                        frameDisplay->verticesUV[iH] = __uv;
                        frameDisplay->verticesPos[iH] = Point2(floatArray[vertexOffset + i],
						hasFFD * floatArray[vertexOffset + i + 1]);
#endif
                    }

                    // setup indicies
                    for (std::size_t i = 0; i < triangleCount * 3; ++i)
                    {
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
                        frameDisplay->indices.write[i] = intArray[currentVerticesData ->offset + (unsigned)BinaryOffset::MeshVertexIndices + i];
#else
                        frameDisplay->indices[i] = intArray[currentVerticesData ->offset + (unsigned)BinaryOffset::MeshVertexIndices + i];		
#endif					
                    }

                    _textureScale = 1.0f;
                    _identityTransform();
                }  else // Normal texture
                {
                    frameDisplay->indices.resize(6);
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
                    frameDisplay->indices.write[0] = 0;
                    frameDisplay->indices.write[1] = 1;
                    frameDisplay->indices.write[2] = 2;
                    frameDisplay->indices.write[3] = 2;
                    frameDisplay->indices.write[4] = 3;
                    frameDisplay->indices.write[5] = 0;
#else
					frameDisplay->indices[0] = 0;
                    frameDisplay->indices[1] = 1;
                    frameDisplay->indices[2] = 2;
                    frameDisplay->indices[3] = 2;
                    frameDisplay->indices[4] = 3;
                    frameDisplay->indices[5] = 0;
#endif
                    frameDisplay->verticesColor.resize(4);
                    frameDisplay->verticesUV.resize(4);
                    frameDisplay->verticesPos.resize(4);

                    const auto scale = currentTextureData->parent->scale * _armature->_armatureData->scale;
                    const auto height = (currentTextureData->rotated ? region.width : region.height) * scale/2.f;
                    const auto width = (currentTextureData->rotated ? region.height : region.width) * scale/2.f;
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
                    frameDisplay->verticesColor.write[0] = Color(1,1,1,1);
                    frameDisplay->verticesColor.write[1] = Color(1,1,1,1);
                    frameDisplay->verticesColor.write[2] = Color(1,1,1,1);
                    frameDisplay->verticesColor.write[3] = Color(1,1,1,1);

                    frameDisplay->verticesPos.write[3] = Vector2(-width, -height);
                    frameDisplay->verticesPos.write[2] = Vector2(width, -height);
                    frameDisplay->verticesPos.write[1] = Vector2(width, height);
                    frameDisplay->verticesPos.write[0] = Vector2(-width, height);

                    __get_uv_pt(frameDisplay->verticesUV.write[0], currentTextureData->rotated, 0, 0, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV.write[1], currentTextureData->rotated, 1.f, 0, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV.write[2], currentTextureData->rotated, 1.f, 1.f, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV.write[3], currentTextureData->rotated, 0, 1.f, region, atlas);
#else
					frameDisplay->verticesColor[0] = Color(1,1,1,1);
                    frameDisplay->verticesColor[1] = Color(1,1,1,1);
                    frameDisplay->verticesColor[2] = Color(1,1,1,1);
                    frameDisplay->verticesColor[3] = Color(1,1,1,1);

                    frameDisplay->verticesPos[3] = Vector2(-width, -height);
                    frameDisplay->verticesPos[2] = Vector2(width, -height);
                    frameDisplay->verticesPos[1] = Vector2(width, height);
                    frameDisplay->verticesPos[0] = Vector2(-width, height);

                    __get_uv_pt(frameDisplay->verticesUV[0], currentTextureData->rotated, 0, 0, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV[1], currentTextureData->rotated, 1.f, 0, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV[2], currentTextureData->rotated, 1.f, 1.f, region, atlas);
                    __get_uv_pt(frameDisplay->verticesUV[3], currentTextureData->rotated, 0, 1.f, region, atlas);
#endif
                    _pivotY = 0;
                    _pivotX = 0;
                    _textureScale = scale;
                    _identityTransform();
                }

                _visibleDirty = true;
                _blendModeDirty = true;
                _colorDirty = true;
                _renderDisplay->update();
                return;
	}
    _renderDisplay->hide();
}

void GDSlot::_updateMesh()
{
    const auto scale = _armature->_armatureData->scale;
    const auto textureData = static_cast<GDTextureData*>(_textureData);
    const auto& deformVertices = _deformVertices->vertices;
    const auto hasFFD = !deformVertices.empty();
    const auto& bones = _deformVertices->bones;
    const auto verticesData = _deformVertices->verticesData;
    const auto weightData = verticesData->weight;
    const auto meshDisplay = static_cast<GDMesh*>(_renderDisplay);

    if (!textureData)
	{
		return;
	}

    if (!meshDisplay->indices.size())
    {
        _armature->invalidUpdate("", true);
        return;
    }

	if (weightData != nullptr)
	{
		const auto data = verticesData->data;
		const auto intArray = data->intArray;
		const auto floatArray = data->floatArray;
		const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
		int weightFloatOffset = intArray[weightData->offset + (unsigned)BinaryOffset::WeigthFloatOffset];

		if (weightFloatOffset < 0)
		{
			weightFloatOffset += 65536;
		}

		for (
			std::size_t i = 0, iD = 0, iB = weightData->offset + (unsigned)BinaryOffset::WeigthBoneIndices + weightData->bones.size(), iV = (std::size_t)weightFloatOffset, iF = 0;
			i < vertexCount;
			++i
			)
		{
			const auto boneCount = (std::size_t)intArray[iB++];
			auto xG = 0.0f, yG = 0.0f;
			for (std::size_t j = 0; j < boneCount; ++j)
			{
				const auto boneIndex = (unsigned)intArray[iB++];
				const auto bone = bones[boneIndex];
				if (bone != nullptr)
				{
					const auto& matrix = bone->globalTransformMatrix;
					const auto weight = floatArray[iV++];
					auto xL = floatArray[iV++] * scale;
                    auto yL = floatArray[iV++] * scale;

					if (hasFFD)
					{
                        xL += deformVertices[iF++];
                        yL += deformVertices[iF++];
					}

					xG += (matrix.a * xL + matrix.c * yL + matrix.tx) * weight;
					yG += (matrix.b * xL + matrix.d * yL + matrix.ty) * weight;
				}
			}
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
            meshDisplay->verticesPos.write[i] = Vector2(xG, yG);
#else
			meshDisplay->verticesPos[i] = Vector2(xG, yG);
#endif
		}
	}
	else if (hasFFD)
	{        
		const auto data = verticesData->data;
		const auto intArray = data->intArray;
		const auto floatArray = data->floatArray;
		const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
		int vertexOffset = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshFloatOffset];

		if (vertexOffset < 0)
		{
			vertexOffset += 65536;
		}
        for (std::size_t i = 0, l = (vertexCount << 1); i < l; i += 2)
		{
			const auto iH = (i >> 1);
            const auto xG = floatArray[vertexOffset + i] * scale + deformVertices[i];
            const auto yG = floatArray[vertexOffset + i + 1] * scale + deformVertices[i + 1];
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
            meshDisplay->verticesPos.write[iH] = Vector2(xG, -yG);
#else
			meshDisplay->verticesPos[iH] = Vector2(xG, -yG);
#endif
		}
	}

    _renderDisplay->update();
}

void GDSlot::_identityTransform()
{
    auto matrix = MATRIX_TRANSFORM();
    matrix.scale(Size2(_textureScale, _textureScale));
    _renderDisplay->set_transform(matrix);
    _renderDisplay->update();
}

void GDSlot::_updateTransform()
{
   Vector2 pos = Vector2(0,0);
   if (((void*)_renderDisplay) == _rawDisplay || ((void*)_renderDisplay) == _meshDisplay)
   {
     pos.x = globalTransformMatrix.tx - (globalTransformMatrix.a * _pivotX + globalTransformMatrix.c * _pivotY);
     pos.y = globalTransformMatrix.ty - (globalTransformMatrix.b * _pivotX + globalTransformMatrix.d * _pivotY);
   }
   else if (_childArmature)
   {
     pos.x = globalTransformMatrix.tx;
     pos.y = globalTransformMatrix.ty;
   }
   else
   {
     Vector2 anchorPoint(1.f, 1.f);

     pos.x = globalTransformMatrix.tx - (globalTransformMatrix.a * anchorPoint.x - globalTransformMatrix.c * anchorPoint.y);
     pos.y = globalTransformMatrix.ty - (globalTransformMatrix.b * anchorPoint.x - globalTransformMatrix.d * anchorPoint.y);
   }

   auto matrix = MATRIX_TRANSFORM(
                    globalTransformMatrix.a * _textureScale,
                    globalTransformMatrix.b * _textureScale,
                    -globalTransformMatrix.c * _textureScale,
                    -globalTransformMatrix.d * _textureScale,
                    pos.x * _textureScale,
                    pos.y * _textureScale
                    );

   matrix.scale(Size2(1, 1));

   _renderDisplay->set_transform(matrix);
   _renderDisplay->update();
}

void GDSlot::_onClear()
{
	Slot::_onClear();

	_textureScale = 1.0f;
    _renderDisplay = nullptr;
	if (_textureData)
	{
		delete _textureData;
		_textureData = nullptr;
	}
}

DRAGONBONES_NAMESPACE_END
