// #include "resource_format_loader_dragonbones.h"

// #include "dragonbones_resource.h"

// using namespace godot;

// Variant ResourceFormatLoaderDragonBones::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
// 	Ref<DragonBonesResource> ret(memnew(DragonBonesResource));

// 	String __str_path_base = p_path.get_basename();

// 	__str_path_base = __str_path_base.erase(__str_path_base.length() - strlen("_ske"), strlen("_ske"));

// 	// texture path
// 	ret->set_def_texture_path(__str_path_base + "_tex.png");

// 	// loading atlas data
// 	ERR_FAIL_COND_V(!ret->load_texture_atlas_data(__str_path_base + "_tex.dbjson"), {});

// 	// loading bones data
// 	ERR_FAIL_COND_V(!ret->load_bones_data(p_path), {});

// 	ret->set_path(p_path);
// 	return ret;
// }

// PackedStringArray ResourceFormatLoaderDragonBones::_get_recognized_extensions() const {
// 	PackedStringArray ret;
// 	ret.push_back(DragonBonesResource::SRC_JSON_EXT);
// 	ret.push_back(DragonBonesResource::SRC_BIN_EXT);
// 	ret.push_back(DragonBonesResource::SAVED_EXT);
// 	return ret;
// }

// bool ResourceFormatLoaderDragonBones::_handles_type(const StringName &p_type) const {
// 	return p_type == DragonBonesResource::get_class_static() ||
// 			ClassDB::is_parent_class(DragonBonesResource::get_class_static(), p_type);
// }

// String ResourceFormatLoaderDragonBones::_get_resource_type(const String &p_path) const {
// 	String el = p_path.get_extension().to_lower();
// 	if ((el == DragonBonesResource::SRC_JSON_EXT || el == DragonBonesResource::SRC_BIN_EXT) &&
// 			p_path.get_basename().to_lower().ends_with("_ske")) {
// 		return DragonBonesResource::get_class_static();
// 	}

// 	return "";
// }