#include "GDArmatureDisplay.h"

#include "GDDisplay.h"
#include "GDSlot.h"
#include "godot_cpp/classes/atlas_texture.hpp"

DRAGONBONES_NAMESPACE_BEGIN

// godot::Rect2 convert_to_rect2(Rectangle *p_rectangle) { return { p_rectangle->x, p_rectangle->y, p_rectangle->width, p_rectangle->height }; }

// godot::Ref<godot::AtlasTexture> create_atlas_texutre(const godot::Ref<godot::Texture2D> &p_texture, Rectangle *p_rectangle) {
// 	godot::Ref<godot::AtlasTexture> ret;
// 	ret.instantiate();
// 	ret->set_atlas(p_texture);
// 	ret->set_region(convert_to_rect2(p_rectangle));
// 	return ret;
// }

GDArmatureDisplay::GDArmatureDisplay() {
	p_armature = nullptr;
	set_use_parent_material(true);
}

GDArmatureDisplay::~GDArmatureDisplay() {
	p_armature = nullptr;
}

void GDArmatureDisplay::dbInit(Armature *_p_armature) {
	p_armature = _p_armature;
}

void GDArmatureDisplay::dbClear() {
	p_armature = nullptr;
}

void GDArmatureDisplay::dbUpdate() {
}

void GDArmatureDisplay::dispatchDBEvent(const std::string &_type, EventObject *_value) {
	if (p_owner)
		p_owner->dispatch_event(godot::String(_type.c_str()), _value);
}

void GDArmatureDisplay::dispose(bool _disposeProxy) {
	if (p_armature) {
		delete p_armature;
		p_armature = nullptr;
	}
}

void GDArmatureDisplay::add_parent_class(bool _b_debug, const godot::Ref<godot::Texture> &_m_texture_atlas) {
	if (!p_armature)
		return;

	for (Slot *e : p_armature->getSlots()) {
		auto slot = static_cast<GDSlot *>(e);
		if (!slot)
			continue;

		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			add_child(display);
			display->p_owner = p_owner;
			display->b_debug = _b_debug;
			display->texture = _m_texture_atlas; //(create_atlas_texutre(_m_texture_atlas, slot->get_atlas_texture_region()));
		}
	}
}

void GDArmatureDisplay::update_childs(bool _b_color, bool _b_blending) {
	if (!p_armature)
		return;

	auto arr = p_armature->getSlots();
	for (auto item : arr) {
		if (!item)
			continue;

		if (_b_color)
			item->_colorDirty = true;

		if (_b_blending)
			item->invalidUpdate();

		item->update(0);
	}
}

void GDArmatureDisplay::update_material_inheritance(bool _b_inherit_material) {
	if (!p_armature)
		return;

	for (Slot *item : p_armature->getSlots()) {
		if (!item)
			continue;
		if (auto display = static_cast<GDDisplay *>(item->getRawDisplay())) {
			display->set_use_parent_material(_b_inherit_material);
		}
	}
}

void GDArmatureDisplay::update_texture_atlas(const godot::Ref<godot::Texture> &_m_texture_atlas) {
	if (!p_armature)
		return;

	for (Slot *e : p_armature->getSlots()) {
		auto slot = static_cast<GDSlot *>(e);
		if (!slot)
			continue;

		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			display->texture = _m_texture_atlas; //(create_atlas_texutre(_m_texture_atlas, slot->get_atlas_texture_region()));
			display->queue_redraw();
		}
	}
}

void GDArmatureDisplay::set_debug(bool _b_debug) {
	if (!p_armature)
		return;

	for (Slot *item : p_armature->getSlots()) {
		if (!item)
			continue;

		if (auto display = static_cast<GDDisplay *>(item->getRawDisplay())) {
			display->b_debug = _b_debug;
			display->queue_redraw();
		}
	}
}

DRAGONBONES_NAMESPACE_END
