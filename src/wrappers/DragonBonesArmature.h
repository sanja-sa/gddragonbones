#pragma once

#include "DragonBonesBone.h"
#include "GDDisplay.h"
#include "dragonBones/armature/Armature.h"
#include "dragonBones/armature/IArmatureProxy.h"
#include "dragonBones/model/DisplayData.h"
#include "wrappers/DragonBonesSlot.h"

namespace godot {

class DragonBonesArmature : public GDDisplay, virtual public dragonBones::IArmatureProxy {
	GDCLASS(DragonBonesArmature, Node2D)
public:
	enum AnimationCallbackModeProcess {
		ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS = 0,
		ANIMATION_CALLBACK_MODE_PROCESS_IDLE = 1,
		ANIMATION_CALLBACK_MODE_PROCESS_MANUAL = 2,
	};

	enum AnimFadeOutMode {
		FADE_OUT_NONE,
		FADE_OUT_SAME_LAYER,
		FADE_OUT_SAME_GROUP,
		FADE_OUT_SAME_LAYER_AND_GROUP,
		FADE_OUT_ALL,
		FADE_OUT_SINGLE,
	};

private:
	DragonBonesArmature(const DragonBonesArmature &);

protected:
	dragonBones::Armature *p_armature{ nullptr };
	std::map<std::string, Ref<DragonBonesBone>> _bones;
	std::map<std::string, Ref<DragonBonesSlot>> _slots;

public:
	DragonBonesArmature();
	~DragonBonesArmature();

	virtual void update_modulate(const Color &p_modulate) override {
		set_modulate(p_modulate);
		update_childs(true);
	}

	virtual void dispatch_event(const String &_str_type, const dragonBones::EventObject *_p_value) override {
		if (p_owner) {
			p_owner->dispatch_event(_str_type, _p_value);
		}
	}

	virtual void dispatch_sound_event(const String &_str_type, const dragonBones::EventObject *_p_value) override {
		if (p_owner) {
			p_owner->dispatch_sound_event(_str_type, _p_value);
		}
	}

	dragonBones::Slot *getSlot(const std::string &name) const;

	void add_bone(std::string name, const Ref<DragonBonesBone> &new_bone);
	void add_slot(std::string name, const Ref<DragonBonesSlot> &new_slot);
	void addEvent(const std::string &_type, const std::function<void(dragonBones::EventObject *)> &_callback);
	void removeEvent(const std::string &_type);

	virtual bool hasDBEventListener(const std::string &_type) const override { return true; }
	virtual void addDBEventListener(const std::string &_type, const std::function<void(dragonBones::EventObject *)> &_listener) override {}
	virtual void removeDBEventListener(const std::string &_type, const std::function<void(dragonBones::EventObject *)> &_listener) override {}
	virtual void dispatchDBEvent(const std::string &_type, dragonBones::EventObject *_value) override {
		this->dispatch_event(String(_type.c_str()), _value);
	}

	void dbInit(dragonBones::Armature *_p_armature) override;
	void dbClear() override;
	void dbUpdate() override;

	void dispose(bool disposeProxy) override;

	virtual dragonBones::Armature *getArmature() const override { return p_armature; }
	virtual dragonBones::Animation *getAnimation() const override { return p_armature->getAnimation(); }

	void setup_recursively(bool _b_debug, const Ref<Texture> &_m_texture_atlas);
	void update_childs(bool _b_color, bool _b_blending = false);
	void update_texture_atlas(const Ref<Texture> &_m_texture_atlas);
	void update_material_inheritance(bool _b_inherit_material);

	//
	dragonBones::Slot *getSlot(const String &p_name) const { return p_armature->getSlot(p_name.ascii().get_data()); }

public:
	/* METHOD BINDINGS */
	static void _bind_methods();

	bool is_frozen();
	void freeze();
	void thaw();
	void set_debug(bool _b_debug, bool p_recursively = false);
	bool is_debug() const { return b_debug; }

	bool has_animation(const String &_animation_name);
	Array get_animations();

	String get_current_animation() const;
	String get_current_animation_on_layer(int _layer) const;
	String get_current_animation_in_group(const String &_group_name) const;

	float tell_animation(const String &_animation_name);
	void seek_animation(const String &_animation_name, float progress);

	bool is_playing() const;
	void play(const String &_animation_name, int loop = -1);
	void play_from_time(const String &_animation_name, float _f_time, int loop = -1);
	void play_from_progress(const String &_animation_name, float f_progress, int loop = -1);
	void stop(const String &_animation_name, bool b_reset = false);
	void stop_all_animations(bool b_children = false, bool b_reset = false);
	void fade_in(const String &_animation_name, float _time,
			int _loop, int _layer, const String &_group, DragonBonesArmature::AnimFadeOutMode _fade_out_mode);

	void reset();

	bool has_slot(const String &_slot_name) const;
	Dictionary get_slots();
	Ref<DragonBonesSlot> get_slot(const String &_slot_name);
	void set_slot_display_index(const String &_slot_name, int _index);
	void set_slot_by_item_name(const String &_slot_name, const String &_item_name);
	void set_all_slots_by_item_name(const String &_item_name);
	int get_slot_display_index(const String &_slot_name);
	int get_total_items_in_slot(const String &_slot_name);
	void cycle_next_item_in_slot(const String &_slot_name);
	void cycle_previous_item_in_slot(const String &_slot_name);
	Color get_slot_display_color_multiplier(const String &_slot_name);
	void set_slot_display_color_multiplier(const String &_slot_name, const Color &_color);

	void flip_x(bool _b_flip);
	bool is_flipped_x() const;
	void flip_y(bool _b_flip);
	bool is_flipped_y() const;

	Dictionary get_ik_constraints();
	void set_ik_constraint(const String &name, Vector2 position);
	void set_ik_constraint_bend_positive(const String &name, bool bend_positive);

	Dictionary get_bones();
	Ref<DragonBonesBone> get_bone(const String &name);
};

} //namespace godot

VARIANT_ENUM_CAST(godot::DragonBonesArmature::AnimationCallbackModeProcess);
VARIANT_ENUM_CAST(godot::DragonBonesArmature::AnimFadeOutMode);