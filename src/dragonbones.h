#pragma once

#include "dragonBones/core/DragonBones.h"
#include "dragonbones_resource.h"
#include "wrappers/DragonBonesFactory.h"
#include "wrappers/GDDisplay.h"

namespace godot {

class DragonBones : public GDOwnerNode, public dragonBones::IEventDispatcher {
	GDCLASS(DragonBones, GDOwnerNode)

public:
	// sound IEventDispatcher
	virtual void addDBEventListener(const std::string &type, const std::function<void(dragonBones::EventObject *)> &listener) override {}
	virtual void removeDBEventListener(const std::string &type, const std::function<void(dragonBones::EventObject *)> &listener) override {}
	virtual void dispatchDBEvent(const std::string &type, dragonBones::EventObject *value) override {
		dispatch_snd_event(String(type.c_str()), value);
	}
	virtual bool hasDBEventListener(const std::string &type) const override { return true; }

private:
	dragonBones::DragonBones *p_instance{ nullptr };

	Ref<Texture2D> m_texture_atlas;
	Ref<DragonBonesResource> m_res;
	String str_curr_anim{ "[none]" };
	DragonBonesArmature *p_armature{ nullptr };
	DragonBonesArmature::AnimationCallbackModeProcess m_anim_mode{ DragonBonesArmature::ANIMATION_CALLBACK_MODE_PROCESS_IDLE };
	float f_speed{ 1.0f };
	float f_progress{ 0.0f };
	int c_loop{ -1 };
	bool b_active{ true };
	bool b_playing{ false };
	bool b_debug{ false };
	bool b_inited{ false };
	bool b_try_playing{ false };

	// #ifdef COMPATIBILITY_ENABLED
	bool b_flip_x{ false };
	bool b_flip_y{ false };
	// #endif

	bool b_inherit_child_material{ true };

protected:
	void _notification(int _what);
	static void _bind_methods();

	bool _set(const StringName &_str_name, const Variant &_c_r_value);
	bool _get(const StringName &_str_name, Variant &_r_ret) const;
	void _get_property_list(List<PropertyInfo> *_p_list) const;

public:
	DragonBones() = default;
	~DragonBones() { _cleanup(); }

	void _cleanup();

	// to initial pose current animation
	void _reset();

	virtual void dispatch_event(const String &_str_type, const dragonBones::EventObject *_p_value) override;
	virtual void dispatch_snd_event(const String &_str_type, const dragonBones::EventObject *_p_value) override;

	// setters/getters
	void set_resource(Ref<DragonBonesResource> _p_data);
	Ref<DragonBonesResource> get_resource();

	void set_inherit_material(bool _b_enable);
	bool is_material_inherited() const;

	void set_active(bool _b_active);
	bool is_active() const;

	void set_speed(float _f_speed);
	float get_speed() const;

	void set_texture(const Ref<Texture2D> &_p_texture);
	Ref<Texture2D> get_texture() const;

	void set_animation_process_mode(DragonBonesArmature::AnimationCallbackModeProcess _mode);
	DragonBonesArmature::AnimationCallbackModeProcess get_animation_process_mode() const;

	void advance(float p_delta) {
		if (p_instance) {
			p_instance->advanceTime(p_delta);
		}
	}

	void set_debug(bool _b_debug);
	bool is_debug() const;

	/* deprecated */ void flip_x(bool _b_flip);
	/* deprecated */ bool is_fliped_x() const;
	/* deprecated */ void flip_y(bool _b_flip);
	/* deprecated */ bool is_fliped_y() const;
#ifdef COMPATIBILITY_ENABLED
	/**
		THESE DEPRECATED FUNCTIONS WILL BE REMOVED IN VERSION 3.2.53
	*/
	/* deprecated */ void fade_in(const String &_name_anim, float _time, int _loop, int _layer, const String &_group, DragonBonesArmature::AnimFadeOutMode _fade_out_mode);
	/* deprecated */ void fade_out(const String &_name_anim);
	/* deprecated */ String get_current_animation() const;
	/* deprecated */ String get_current_animation_on_layer(int _layer) const;
	/* deprecated */ float tell();
	/* deprecated */ void seek(float _f_p);
	/* deprecated */ float get_progress() const;
	/* deprecated */ bool has_anim(const String &_str_anim);
	/* deprecated */ bool has_slot(const String &_slot_name) const;
	/* deprecated */ Color get_slot_display_color_multiplier(const String &_slot_name);
	/* deprecated */ void set_slot_display_color_multiplier(const String &_slot_name, const Color &_color);
	/* deprecated */ void set_slot_display_index(const String &_slot_name, int _index = 0);
	/* deprecated */ void set_slot_by_item_name(const String &_slot_name, const String &_item_name);
	/* deprecated */ void set_all_slots_by_item_name(const String &_item_name);
	/* deprecated */ int get_slot_display_index(const String &_slot_name);
	/* deprecated */ int get_total_items_in_slot(const String &_slot_name);
	/* deprecated */ void cycle_next_item_in_slot(const String &_slot_name);
	/* deprecated */ void cycle_previous_item_in_slot(const String &_slot_name);
	/* deprecated */ bool is_playing() const;
	/* deprecated */ void play(bool _b_play = true);
	/* deprecated */ void play_from_time(float _f_time);
	/* deprecated */ void play_from_progress(float _f_progress);
	/* deprecated */ void play_new_animation(const String &_str_anim, int _num_times);
	/* deprecated */ void play_new_animation_from_progress(const String &_str_anim, int _num_times, float _f_progress);
	/* deprecated */ void play_new_animation_from_time(const String &_str_anim, int _num_times, float _f_time);
	/* deprecated */ void stop(bool _b_all = false);
	/* deprecated */ inline void stop_all() { stop(true); }
#endif
	DragonBonesArmature *get_armature();

private:
	const dragonBones::DragonBonesData *get_dragonbones_data() const;
	dragonBones::ArmatureData *get_armature_data(const String &_armature_name);

	void _on_resource_changed();
};

} //namespace godot