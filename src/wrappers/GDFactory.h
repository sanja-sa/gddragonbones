#pragma once

#include <string>
#include <vector>

#include "GDArmatureDisplay.h"
#include "GDSlot.h"
#include "GDTextureData.h"
#include "dragonBones/factory/BaseFactory.h"

namespace godot {

typedef std::function<void(EventObject *)> Func_t;
class GDFactory : public BaseFactory, public IEventDispatcher {
	DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(GDFactory)

protected:
	DragonBones *_dragonBonesInstance;
	GDOwnerNode *p_owner;

	mutable std::vector<std::unique_ptr<Slot_GD>> _wrapperSlots;
	mutable std::vector<std::unique_ptr<GDTextureData>> _wrapperTexturesData;

public:
	GDFactory(GDOwnerNode *_p_owner);
	~GDFactory();

public:
	DragonBonesData *loadDragonBonesData(const char *_p_data_loaded, const std::string &name = "");
	TextureAtlasData *loadTextureAtlasData(const char *_p_data_loaded, Ref<Texture> *_p_atlasTexture, const std::string &name = "", float scale = 1.0f);
	GDArmatureDisplay *buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName = "", const std::string &skinName = "", const std::string &textureAtlasName = "") const;

	virtual void clear(bool disposeData = true) override;
	void update(float lastUpdate);
	void set_speed(float _f_speed);

	// sound IEventDispatcher
	virtual void addDBEventListener(const std::string &type, const Func_t &listener) override {}
	virtual void removeDBEventListener(const std::string &type, const Func_t &listener) override {}
	virtual void dispatchDBEvent(const std::string &type, EventObject *value) override;
	virtual bool hasDBEventListener(const std::string &type) const override { return true; }

protected:
	TextureAtlasData *_buildTextureAtlasData(TextureAtlasData *textureAtlasData, void *textureAtlas) const override;
	Armature *_buildArmature(const BuildArmaturePackage &dataPackage) const override;
	Slot *_buildSlot(const BuildArmaturePackage &dataPackage, const SlotData *slotData, Armature *armature) const override;
	Armature *_buildChildArmature(const BuildArmaturePackage *dataPackage, Slot *slot, DisplayData *displayData) const override;
	void _buildBones(const BuildArmaturePackage &dataPackage, Armature *armature) const override;
};

} //namespace godot