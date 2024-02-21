#include "dragonbones_resource.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wrappers/DragonBonesFactory.h"

using namespace godot;

// TODO: 在编辑器中追踪文件变动
//////////////////////////////////////////////////////////////////
//// Resource

void make_data_unuse_atlas(dragonBones::DragonBonesData *p_data, const dragonBones::TextureAtlasData *p_atlas) {
	if (!p_data || !p_atlas) {
		return;
	}

	for (const auto kv : p_data->armatures) {
		const auto armature = kv.second;
		if (!armature) {
			continue;
		}

		for (const auto skin_kv : armature->skins) {
			const auto skin = skin_kv.second;
			if (!skin) {
				continue;
			}

			for (const auto displays_kv : skin->displays) {
				const auto displays = displays_kv.second;
				for (const auto display : displays) {
					if (displays.empty()) {
						continue;
					}

					if (display->type == dragonBones::DisplayType::Image) {
						if (auto image_display = static_cast<dragonBones::ImageDisplayData *>(display)) {
							if (image_display->texture->parent == p_atlas) {
								image_display->texture = nullptr; // 清除对该图集的散图引用
							}
						}
					} else if (display->type == dragonBones::DisplayType::Mesh) {
						if (auto mesh_display = static_cast<dragonBones::MeshDisplayData *>(display)) {
							if (mesh_display->texture->parent == p_atlas) {
								mesh_display->texture = nullptr; // 清除对该图集的散图引用
							}
						}
					}
				}
			}
		}
	}
}

template <bool LOAD_DRAGON_BONES_DATA>
Error DragonBonesResource::load_data(const String &p_file_path, String &p_r_loaded_file, String &p_r_file_md5) {
	if (p_file_path.is_empty()) {
		// 置空
		if (p_r_loaded_file.is_empty()) {
			return OK; // 不需要重复设置
		}

		if constexpr (LOAD_DRAGON_BONES_DATA) {
			if (dragon_bones_data && !dragon_bones_data->name.empty()) {
				factory->removeDragonBonesData(dragon_bones_data->name, true);
				dragon_bones_data = nullptr;
			}
		} else {
			// TextureAtlasData 中的 TextureData 可能仍被已有的 龙骨数据中的 Skin 引用，
			// 确保其不被占用
			make_data_unuse_atlas(dragon_bones_data, texture_atlas_data);

			if (texture_atlas_data && !texture_atlas_data->name.empty()) {
				factory->removeTextureAtlasData(texture_atlas_data->name, true);
				texture_atlas_data = nullptr;
			}
		}

		p_r_loaded_file = "";
		p_r_file_md5 = "";
		return OK;
	}

	auto file = FileAccess::open(p_file_path, FileAccess::READ);
	ERR_FAIL_NULL_V(file, file->get_open_error());
	ERR_FAIL_COND_V(!file->get_length(), ERR_PARSE_ERROR);

	const auto md5 = FileAccess::get_md5(p_file_path);

	bool loaded = false;
	if constexpr (LOAD_DRAGON_BONES_DATA) {
		loaded = dragon_bones_data;
	} else {
		loaded = texture_atlas_data;
	}

	if (loaded && p_file_path == p_r_loaded_file && md5 == p_r_file_md5) {
		// 已经加载，且文件没有变动，直接返回
		return OK;
	}

	// 读取数据
	PackedByteArray raw_data;
	raw_data.resize(file->get_length() + 1);
	file->get_buffer(raw_data.ptrw(), file->get_length());
	raw_data.set(file->get_length(), 0x00);

	if constexpr (LOAD_DRAGON_BONES_DATA) {
		if (dragon_bones_data && !dragon_bones_data->name.empty()) {
			factory->removeDragonBonesData(dragon_bones_data->name, true);
		}
		dragon_bones_data = factory->loadDragonBonesData((const char *)raw_data.ptr());
		ERR_FAIL_NULL_V(dragon_bones_data, ERR_PARSE_ERROR);
	} else {
		// TextureAtlasData 中的 TextureData 可能仍被已有的 龙骨数据中的 Skin 引用，
		// 确保其不被占用
		make_data_unuse_atlas(dragon_bones_data, texture_atlas_data);

		if (texture_atlas_data && !texture_atlas_data->name.empty()) {
			factory->removeTextureAtlasData(texture_atlas_data->name, true);
		}
		texture_atlas_data = factory->loadTextureAtlasData((const char *)raw_data.ptr(), nullptr);
		ERR_FAIL_NULL_V(texture_atlas_data, ERR_PARSE_ERROR);
	}

	p_r_loaded_file = p_file_path;
	p_r_file_md5 = md5;
	return OK;
}

// 导入器用
Error DragonBonesResource::load_dragon_bones_data(const String &_path) {
	return load_data<true>(_path, dragon_bones_ske_file, dragon_bones_ske_file_md5);
}

// 导入器用
Error DragonBonesResource::load_texture_atlas_data(const String &_path) {
	return load_data<false>(_path, texture_atlas_json_file, texture_atlas_json_file_md5);
}

dragonBones::DragonBones *DragonBonesResource::create_dragon_bones(dragonBones::IEventDispatcher *p_event_manager, DragonBonesArmature *&r_main_armature) {
	ERR_FAIL_NULL_V(dragon_bones_data, nullptr);
	ERR_FAIL_COND_V(dragon_bones_data->armatureNames.size() <= 0, nullptr);

	auto *ret{ memnew(dragonBones::DragonBones(p_event_manager)) };
	factory->set_building_dragonbones_instance(ret);

	auto armature = dragon_bones_data->getArmature(dragon_bones_data->getArmatureNames()[0]);
	auto display = static_cast<dragonBones::ImageDisplayData *>((*armature->skins.begin()->second->getDisplays("Shadow"))[0]);
	UtilityFunctions::printerr("befor: ", display->name.c_str(), " ==  ", display->texture ? display->texture->name.c_str() : "unknown");

	r_main_armature = factory->buildArmatureDisplay(dragon_bones_data->getArmatureNames()[0].c_str(), dragon_bones_data->name);

	return ret;
}

void DragonBonesResource::set_dragon_bones_ske_file(const String &p_dragon_bones_ske_file) {
	String old_md5 = dragon_bones_ske_file_md5;
	auto old_data = dragon_bones_data;
	auto err = load_dragon_bones_data(p_dragon_bones_ske_file);
	if (old_md5 != dragon_bones_ske_file_md5 || old_data != dragon_bones_data) {
		emit_changed();
	}
}

void DragonBonesResource::set_texture_atlas_json_file(const String &p_texture_atlas_json_file) {
	String old_md5 = texture_atlas_json_file_md5;
	auto old_data = texture_atlas_data;
	load_texture_atlas_data(p_texture_atlas_json_file);
	if (old_md5 != texture_atlas_json_file_md5 || old_data != texture_atlas_data) {
		if (default_texture.is_valid() && texture_atlas_data) {
			// 以默认纹理修正图集数据
			texture_atlas_data->height = default_texture->get_height();
			texture_atlas_data->width = default_texture->get_width();
		}
		emit_changed();
	}
}

void DragonBonesResource::set_default_texture(const Ref<Texture2D> &p_default_texture) {
	if (default_texture == p_default_texture) {
		return;
	}
	default_texture = p_default_texture;

	if (default_texture.is_valid() && texture_atlas_data) {
		// 以默认纹理修正图集数据
		texture_atlas_data->height = default_texture->get_height();
		texture_atlas_data->width = default_texture->get_width();
	}

	emit_changed();
}

void DragonBonesResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_dragon_bones_ske_file", "dragon_bones_ske_file"), &DragonBonesResource::set_dragon_bones_ske_file);
	ClassDB::bind_method(D_METHOD("get_dragon_bones_ske_file"), &DragonBonesResource::get_dragon_bones_ske_file);

	ClassDB::bind_method(D_METHOD("set_texture_atlas_json_file", "texture_atlas_json_file"), &DragonBonesResource::set_texture_atlas_json_file);
	ClassDB::bind_method(D_METHOD("get_texture_atlas_json_file"), &DragonBonesResource::get_texture_atlas_json_file);

	ClassDB::bind_method(D_METHOD("set_default_texture", "default_texture"), &DragonBonesResource::set_default_texture);
	ClassDB::bind_method(D_METHOD("get_default_texture"), &DragonBonesResource::get_default_texture);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "dragon_bones_ske_file", PROPERTY_HINT_FILE, "*.json,*dbbin"), "set_dragon_bones_ske_file", "get_dragon_bones_ske_file");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "texture_atlas_json_file", PROPERTY_HINT_FILE, "*.json"), "set_texture_atlas_json_file", "get_texture_atlas_json_file");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "default_texture", PROPERTY_HINT_RESOURCE_TYPE, Texture2D::get_class_static(), PROPERTY_USAGE_DEFAULT, Texture2D::get_class_static()), "set_default_texture", "get_default_texture");
}