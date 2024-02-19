#pragma once

#include <string>

#include "DragonBonesArmature.h"
#include "GDSlot.h"
#include "dragonBones/factory/BaseFactory.h"

namespace godot {

class DragonBonesFactory : public dragonBones::BaseFactory {
	DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(DragonBonesFactory)

private:
	static DragonBonesFactory *singleton; // TODO 实例化

protected:
public:
	DragonBonesFactory();
	~DragonBonesFactory();

	static DragonBonesFactory *get_singleton() { return singleton; }

public:
	void set_building_dragonbones_instance(dragonBones::DragonBones *p_building_instance) { _dragonBones = p_building_instance; }

	dragonBones::DragonBonesData *loadDragonBonesData(const char *_p_data_loaded, const std::string &name);
	dragonBones::TextureAtlasData *loadTextureAtlasData(const char *_p_data_loaded, Ref<Texture> *_p_atlasTexture, const std::string &name = "", float scale = 1.0f);
	DragonBonesArmature *buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName, const std::string &skinName = "", const std::string &textureAtlasName = "") const;

	virtual void clear(bool disposeData = true) override;

protected:
	dragonBones::TextureAtlasData *_buildTextureAtlasData(dragonBones::TextureAtlasData *textureAtlasData, void *textureAtlas) const override;
	dragonBones::Armature *_buildArmature(const dragonBones::BuildArmaturePackage &dataPackage) const override;
	dragonBones::Slot *_buildSlot(const dragonBones::BuildArmaturePackage &dataPackage, const dragonBones::SlotData *slotData, dragonBones::Armature *armature) const override;
	dragonBones::Armature *_buildChildArmature(const dragonBones::BuildArmaturePackage *dataPackage, dragonBones::Slot *slot, dragonBones::DisplayData *displayData) const override;
	void _buildBones(const dragonBones::BuildArmaturePackage &dataPackage, dragonBones::Armature *armature) const override;
};

} //namespace godot