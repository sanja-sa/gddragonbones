#pragma once

#include "dragonBones/event/EventObject.h"
#include "godot_cpp/classes/canvas_item_material.hpp"
#include "godot_cpp/classes/node2d.hpp"

namespace godot {

class GDOwnerNode : public Node2D {
public:
	GDOwnerNode() = default;
	virtual ~GDOwnerNode() = default;

	virtual void dispatch_event(const String &_str_type, const dragonBones::EventObject *_p_value) = 0;
	virtual void dispatch_sound_event(const String &_str_type, const dragonBones::EventObject *_p_value) = 0;
};

class GDDisplay : public GDOwnerNode {
private:
	GDDisplay(const GDDisplay &);

public:
	Ref<Texture2D> texture;
	GDOwnerNode *p_owner{ nullptr };
	bool b_debug{ false };

public:
	GDDisplay() = default;
	virtual ~GDDisplay() = default;

	void set_blend_mode(CanvasItemMaterial::BlendMode p_blend_mode) {
		Ref<CanvasItemMaterial> mat = get_material();
		if (mat.is_null()) {
			if (p_blend_mode == CanvasItemMaterial::BLEND_MODE_MIX) {
				return;
			}
			mat.instantiate();
			set_material(mat);
		}
		mat->set_blend_mode(p_blend_mode);
	}

	virtual void update_modulate(const Color &p_modulate) { set_modulate(p_modulate); }
};

}; //namespace godot
