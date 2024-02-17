#include "register_types.h"

#include "gddragonbones.h"
#include "godot_cpp/classes/resource_format_loader.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "wrappers/GDMesh.h"

using namespace godot;

class ResourceFormatLoaderGDDragonBones : public godot::ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderGDDragonBones, godot::ResourceFormatLoader)
protected:
	static void _bind_methods() {}

public:
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override {
		Ref<GDDragonBonesResource> ret(memnew(GDDragonBonesResource));

		String __str_path_base = p_path.get_basename();

		__str_path_base = __str_path_base.erase(__str_path_base.length() - strlen("_ske"), strlen("_ske"));

		// texture path
		ret->set_def_texture_path(__str_path_base + "_tex.png");

		// loading atlas data
		ERR_FAIL_COND_V(!ret->load_texture_atlas_data(__str_path_base + "_tex.dbjson"), {});

		// loading bones data
		ERR_FAIL_COND_V(!ret->load_bones_data(p_path), {});

		ret->set_path(p_path);
		return ret;
	}

	virtual PackedStringArray _get_recognized_extensions() const override {
		PackedStringArray ret;
		ret.push_back("dbbin");
		ret.push_back("dbjson");
		return ret;
	}

	virtual bool _handles_type(const StringName &p_type) const override {
		return p_type == GDDragonBonesResource::get_class_static() ||
				ClassDB::is_parent_class(GDDragonBonesResource::get_class_static(), p_type);
	}

	virtual String _get_resource_type(const String &p_path) const override {
		String el = p_path.get_extension().to_lower();
		if ((el == "dbjson" || el == "dbbin") && p_path.get_basename().to_lower().ends_with("_ske")) {
			return GDDragonBonesResource::get_class_static();
		}

		return "";
	}
};

static Ref<ResourceFormatLoaderGDDragonBones> resource_loader_GDDragonBones;

void initialize_dragonbones_module(godot::ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_INTERNAL_CLASS(ResourceFormatLoaderGDDragonBones);
		resource_loader_GDDragonBones.instantiate();
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_GDDragonBones, true);
	}
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_INTERNAL_CLASS(dragonBones::GDMesh);
	GDREGISTER_CLASS(GDDragonBones);
	GDREGISTER_CLASS(GDDragonBonesResource);
}

void uninitialize_dragonbones_module(godot::ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		if (resource_loader_GDDragonBones.is_valid()) {
			ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_GDDragonBones);
			resource_loader_GDDragonBones.unref();
		}
	}
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_dragonbones_module);
	init_obj.register_terminator(uninitialize_dragonbones_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
