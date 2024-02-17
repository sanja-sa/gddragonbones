#pragma once

#include "dragonBones/event/EventObject.h"
#include "godot_cpp/classes/canvas_item.hpp"
#include "godot_cpp/classes/canvas_item_material.hpp"
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/sprite2d.hpp"

DRAGONBONES_NAMESPACE_BEGIN

class GDOwnerNode : public godot::Sprite2D {
	// GDCLASS(GDOwnerNode, godot::Node2D);

	// protected:
	// 	static void _bind_methods() {}

public:
	virtual ~GDOwnerNode() {}

	virtual void dispatch_event(const godot::String &_str_type, const dragonBones::EventObject *_p_value) = 0;
	virtual void dispatch_snd_event(const godot::String &_str_type, const dragonBones::EventObject *_p_value) = 0;
};

class GDDisplay : public godot::Node2D {
	// GDCLASS(GDDisplay, Node2D);

	// protected:
	// 	static void _bind_methods() {}

private:
	// GDDisplay(const GDDisplay &);

public:
	godot::Ref<godot::Texture> texture;
	// godot::Ref<godot::CanvasItemMaterial> p_canvas_mat;

	// godot::Color modulate;
	GDOwnerNode *p_owner;
	bool b_debug;

public:
	GDDisplay() {
		p_owner = nullptr;
		b_debug = false;
		// p_canvas_mat.instantiate();
	}
	virtual ~GDDisplay() {}

	void set_blend_mode(godot::CanvasItemMaterial::BlendMode _blend) {
		// p_canvas_mat->set_blend_mode(_blend);
		// set_material(p_canvas_mat);
		if (auto mat = Object::cast_to<godot::CanvasItemMaterial>(get_material().ptr())) {
			mat->set_blend_mode(_blend);
		}
	}
};

DRAGONBONES_NAMESPACE_END