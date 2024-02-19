#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/texture2d.hpp"

namespace godot {

class DragonBonesResource : public Resource {
	GDCLASS(DragonBonesResource, Resource)
public:
	static constexpr auto SRC_DBJSON_EXT = "dbjson";
	static constexpr auto SRC_JSON_EXT = "json";
	static constexpr auto SRC_BIN_EXT = "dbbin";
	static constexpr auto SAVED_EXT = "res";

protected:
	static void _bind_methods();

public:
	DragonBonesResource() = default;
	~DragonBonesResource() = default;

	Error load_bones_data(const String &p_path);
	Error load_texture_atlas_data(const String &p_path);

	PackedByteArray texture_atlas_file_raw_data;
	PackedByteArray bones_file_raw_data;

public:
	//  Binding
	String get_bones_data_file() const { return bones_data_file; }
	void set_bones_data_file(const String &p_bones_data_file);

	String get_texture_atlas_json_file() const { return texture_atlas_json_file; }
	void set_texture_atlas_json_file(const String &p_texture_atlas_json_file);

	Ref<Texture2D> get_default_texture() const { return default_texture; }
	void set_default_texture(const Ref<Texture2D> &p_default_texture);

	Ref<Texture2D> default_texture;
	String bones_data_file;
	String texture_atlas_json_file;
};

} //namespace godot