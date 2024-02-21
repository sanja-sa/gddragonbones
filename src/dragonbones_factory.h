#pragma once

#include "dragonBones/factory/BaseFactory.h"

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/texture2d.hpp"

namespace godot {

class DragonBonesFactory : public Resource, private dragonBones::BaseFactory {
	GDCLASS(DragonBonesFactory, Resource)

protected:
	// dragonBones::BaseFactory 成员
	void set_building_dragon_bones(dragonBones::DragonBones *p_building_instance) { _dragonBones = p_building_instance; }

	dragonBones::DragonBonesData *loadDragonBonesData(const char *_p_data_loaded, const std::string &name = "");
	dragonBones::TextureAtlasData *loadTextureAtlasData(const char *_p_data_loaded, Ref<Texture> *_p_atlasTexture, const std::string &name = "", float scale = 1.0f);
	class DragonBonesArmature *buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName, const std::string &skinName = "", const std::string &textureAtlasName = "") const;

	virtual dragonBones::TextureAtlasData *_buildTextureAtlasData(dragonBones::TextureAtlasData *textureAtlasData, void *textureAtlas) const override;
	virtual dragonBones::Armature *_buildArmature(const dragonBones::BuildArmaturePackage &dataPackage) const override;
	virtual dragonBones::Slot *_buildSlot(const dragonBones::BuildArmaturePackage &dataPackage, const dragonBones::SlotData *slotData, dragonBones::Armature *armature) const override;
	virtual dragonBones::Armature *_buildChildArmature(const dragonBones::BuildArmaturePackage *dataPackage, dragonBones::Slot *slot, dragonBones::DisplayData *displayData) const override;
	virtual void _buildBones(const dragonBones::BuildArmaturePackage &dataPackage, dragonBones::Armature *armature) const override;

public:
	static constexpr auto SRC_DBJSON_EXT = "dbjson";
	static constexpr auto SRC_JSON_EXT = "json";
	static constexpr auto SRC_BIN_EXT = "dbbin";
	static constexpr auto SAVED_EXT = "res";

private:
	String dragon_bones_ske_file_md5{};
	String texture_atlas_json_file_md5{};

	dragonBones::DragonBonesData *dragon_bones_data{ nullptr };
	dragonBones::TextureAtlasData *texture_atlas_data{ nullptr };

	// Tip: 私有模板，不需要在头文件中定义
	template <bool LOAD_DRAGON_BONES_DATA>
	Error load_data(const String &p_file_path, String &r_loaded_file, String &p_r_file_md5);

protected:
	static void _bind_methods();

public:
	DragonBonesFactory() = default;

	Error load_dragon_bones_data(const String &p_path);
	Error load_texture_atlas_data(const String &p_path);

	bool can_create_dragon_bones_instance() const { return dragon_bones_data && texture_atlas_data; }

	dragonBones::DragonBones *create_dragon_bones(dragonBones::IEventDispatcher *p_event_manager, DragonBonesArmature *&r_main_armature);

public:
	//  Binding
	String get_dragon_bones_ske_file() const { return dragon_bones_ske_file; }
	void set_dragon_bones_ske_file(const String &p_dragon_bones_ske_file);

	String get_texture_atlas_json_file() const { return texture_atlas_json_file; }
	void set_texture_atlas_json_file(const String &p_texture_atlas_json_file);

	Ref<Texture2D> get_default_texture() const { return default_texture; }
	void set_default_texture(const Ref<Texture2D> &p_default_texture);

private:
	Ref<Texture2D> default_texture;
	String dragon_bones_ske_file;
	String texture_atlas_json_file;
};

} //namespace godot