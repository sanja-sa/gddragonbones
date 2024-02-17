#pragma once

#include <string>
#include <vector>

#include <dragonBones/DragonBonesHeaders.h>

#include "GDArmatureDisplay.h"
#include "GDSlot.h"
#include "GDTextureData.h"

DRAGONBONES_NAMESPACE_BEGIN

typedef std::function<void(EventObject *)> Func_t;
class GDFactory : public BaseFactory, public IEventDispatcher {
	DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(GDFactory)

protected:
	DragonBones *_dragonBonesInstance;
	GDOwnerNode *p_owner;

	// mutable std::vector<std::unique_ptr<GDSlot>> _wrapperSlots;
	// mutable std::vector<std::unique_ptr<GDTextureData>> _wrapperTexturesData;

public:
	GDFactory(GDOwnerNode *_p_owner);
	~GDFactory();

public:
	DragonBonesData *loadDragonBonesData(const char *_p_data_loaded, const std::string &name = "");
	TextureAtlasData *loadTextureAtlasData(const char *_p_data_loaded, godot::Ref<godot::Texture> *_p_atlasTexture, const std::string &name = "", float scale = 1.0f);
	GDArmatureDisplay *buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName = "", const std::string &skinName = "", const std::string &textureAtlasName = "") const;

	void update(float lastUpdate);
	void set_speed(float _f_speed);

	// sound IEventDispatcher
	virtual void addDBEventListener(const std::string &type, const Func_t &listener) override;
	virtual void removeDBEventListener(const std::string &type, const Func_t &listener) override;
	virtual void dispatchDBEvent(const std::string &type, EventObject *value) override;
	virtual bool hasDBEventListener(const std::string &type) const override;

protected:
	virtual TextureAtlasData *_buildTextureAtlasData(TextureAtlasData *textureAtlasData, void *textureAtlas) const override;
	virtual Armature *_buildArmature(const BuildArmaturePackage &dataPackage) const override;
	virtual Slot *_buildSlot(const BuildArmaturePackage &dataPackage, const SlotData *slotData, Armature *armature) const override;
};

DRAGONBONES_NAMESPACE_END
