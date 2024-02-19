#include "dragonbones_resource.h"
#include "godot_cpp/classes/file_access.hpp"

using namespace godot;

//////////////////////////////////////////////////////////////////
//// Resource

Error __load_file(const String &_file_path, PackedByteArray &r_raw_data) {
	auto __p_f = FileAccess::open(_file_path, FileAccess::READ);
	ERR_FAIL_NULL_V(__p_f, __p_f->get_open_error());
	ERR_FAIL_COND_V(!__p_f->get_length(), ERR_PARSE_ERROR);

	r_raw_data.resize(__p_f->get_length() + 1);
	__p_f->get_buffer(r_raw_data.ptrw(), __p_f->get_length());

	r_raw_data.set(__p_f->get_length(), 0x00);
	return OK;
}

Error DragonBonesResource::load_bones_data(const String &_path) {
	bones_data_file = _path;
	auto err = __load_file(_path, bones_file_raw_data);
	ERR_FAIL_COND_V(err != OK, err);
	return err;
}

Error DragonBonesResource::load_texture_atlas_data(const String &_path) {
	texture_atlas_json_file = _path;
	auto err = __load_file(_path, texture_atlas_file_raw_data);
	ERR_FAIL_COND_V(err != OK, err);
	return err;
}

void DragonBonesResource::set_bones_data_file(const String &p_bones_data_file) {
	load_bones_data(p_bones_data_file);
	emit_changed();
}

void DragonBonesResource::set_texture_atlas_json_file(const String &p_texture_atlas_json_file) {
	load_texture_atlas_data(p_texture_atlas_json_file);
	emit_changed();
}

void DragonBonesResource::set_default_texture(const Ref<Texture2D> &p_default_texture) {
	default_texture = p_default_texture;
	emit_changed();
}

void DragonBonesResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_bones_data_file", "bones_data_file"), &DragonBonesResource::set_bones_data_file);
	ClassDB::bind_method(D_METHOD("get_bones_data_file"), &DragonBonesResource::get_bones_data_file);

	ClassDB::bind_method(D_METHOD("set_texture_atlas_json_file", "texture_atlas_json_file"), &DragonBonesResource::set_texture_atlas_json_file);
	ClassDB::bind_method(D_METHOD("get_texture_atlas_json_file"), &DragonBonesResource::get_texture_atlas_json_file);

	ClassDB::bind_method(D_METHOD("set_default_texture", "default_texture"), &DragonBonesResource::set_default_texture);
	ClassDB::bind_method(D_METHOD("get_default_texture"), &DragonBonesResource::get_default_texture);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "bone_data_file", PROPERTY_HINT_FILE, "*.json,*dbbin"), "set_bones_data_file", "get_bones_data_file");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "texture_atlas_json_file", PROPERTY_HINT_FILE, "*.json"), "set_texture_atlas_json_file", "get_texture_atlas_json_file");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "default_texture", PROPERTY_HINT_RESOURCE_TYPE, Texture2D::get_class_static(), PROPERTY_USAGE_DEFAULT, Texture2D::get_class_static()), "set_default_texture", "get_default_texture");
}