#include "register_types.h"

#include "dragonbones.h"
#include "editor/dragonbones_editor_plugin.h"

#include "godot_cpp/classes/editor_plugin_registration.hpp"

#include "resource_format_loader_dragonbones.h"
#include "wrappers/GDMesh.h"

using namespace godot;

void initialize_dragonbones_module(godot::ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_INTERNAL_CLASS(DragonBonesImportPlugin);
		GDREGISTER_INTERNAL_CLASS(DragonBonesEditorPlugin);
		EditorPlugins::add_by_type<DragonBonesEditorPlugin>();
	}

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_INTERNAL_CLASS(GDMesh);

	GDREGISTER_CLASS(DragonBones);
	GDREGISTER_ABSTRACT_CLASS(DragonBonesBone);
	GDREGISTER_ABSTRACT_CLASS(DragonBonesSlot);
	GDREGISTER_ABSTRACT_CLASS(DragonBonesArmature);

	GDREGISTER_CLASS(DragonBonesResource);

	if (!DragonBonesFactory::get_singleton()) {
		memnew(DragonBonesFactory);
	}
}

void uninitialize_dragonbones_module(godot::ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::remove_by_type<DragonBonesEditorPlugin>();
	}
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	auto factory = DragonBonesFactory::get_singleton();
	if (factory) {
		memdelete(factory);
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
