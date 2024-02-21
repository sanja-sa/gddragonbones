#pragma once

#include "dragonBones/model/DragonBonesData.h"
#include "dragonBones/model/TextureAtlasData.h"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/texture2d.hpp"
#include "wrappers/DragonBonesFactory.h"

namespace godot {

class DragonBonesResource : public Resource {
	GDCLASS(DragonBonesResource, Resource)

public:
	static constexpr auto SRC_DBJSON_EXT = "dbjson";
	static constexpr auto SRC_JSON_EXT = "json";
	static constexpr auto SRC_BIN_EXT = "dbbin";
	static constexpr auto SAVED_EXT = "res";

private:
	DragonBonesFactory *const factory{ memnew(DragonBonesFactory) };

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
	DragonBonesResource() = default;
	~DragonBonesResource() { memdelete(factory); }

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