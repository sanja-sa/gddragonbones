#pragma once

#include "dragonBones/core/DragonBones.h"
#include "godot_cpp/classes/resource.hpp"
#include "wrappers/GDArmatureDisplay.h"
#include "wrappers/GDDisplay.h"
#include "wrappers/GDFactory.h"

// Resource class
class GDDragonBonesResource : public godot::Resource {
	GDCLASS(GDDragonBonesResource, Resource);

protected:
	static void _bind_methods() {}

public:
	GDDragonBonesResource();
	~GDDragonBonesResource();

	void set_def_texture_path(const godot::String &_path);
	bool load_texture_atlas_data(const godot::String &_path);
	bool load_bones_data(const godot::String &_path);

	godot::String str_default_tex_path;
	char *p_data_texture_atlas;
	char *p_data_bones;
};

class GDDragonBones : public dragonBones::GDOwnerNode {
	GDCLASS(GDDragonBones, dragonBones::GDOwnerNode)

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
		FADE_OUT_SINGLE
	};

private:
	dragonBones::GDFactory *p_factory = memnew(dragonBones::GDFactory(this));

	godot::Ref<godot::Texture2D> m_texture_atlas;
	godot::Ref<GDDragonBonesResource> m_res;
	godot::String str_curr_anim{ "[none]" };
	dragonBones::GDArmatureDisplay *p_armature = nullptr;
	AnimationCallbackModeProcess m_anim_mode = ANIMATION_CALLBACK_MODE_PROCESS_IDLE;
	float f_speed = 1.0f;
	float f_progress = 1.0f;
	int c_loop = -1;
	// bool b_processing = false;
	bool b_active = true;
	bool b_playing = false;
	bool b_debug = false;
	bool b_inited = false;
	bool b_try_playing = false;
	bool b_flip_x = false;
	bool b_flip_y = false;
	bool b_inherit_child_material = true;

	auto to_c_str(const godot::StringName &p_sn) const { return godot::String(p_sn).ascii().get_data(); }
	auto get_curr_anim_c_str() const { return str_curr_anim.ascii().get_data(); }

protected:
	void _notification(int _what);
	static void _bind_methods();

	bool _set(const godot::StringName &p_name, const godot::Variant &p_property);
	bool _get(const godot::StringName &p_name, godot::Variant &r_property) const;
	void _get_property_list(godot::List<godot::PropertyInfo> *p_list) const;

public:
	GDDragonBones();
	~GDDragonBones();

	void _cleanup();

	// to initial pose current animation
	void _reset();
	// void _set_process(bool _b_process, bool _b_force = false);

	void dispatch_event(const godot::String &_str_type, const dragonBones::EventObject *_p_value) override;
	void dispatch_snd_event(const godot::String &_str_type, const dragonBones::EventObject *_p_value) override;

	// setters/getters
	void set_resource(godot::Ref<GDDragonBonesResource> _p_data);
	godot::Ref<GDDragonBonesResource> get_resource();

	void set_inherit_material(bool _b_enable);
	bool is_material_inherited() const;

	// #if (VERSION_MAJOR == 3)
	// #else
	// 	void set_opacity(float _f_opacity);
	// 	float get_opacity() const;

	// 	void set_blend_mode(godot::CanvasItem::BlendMode _blend_mode);
	// 	godot::CanvasItem::BlendMode get_blend_mode() const;

	// 	void set_modulate(const godot::Color &_p_color);
	// 	godot::Color get_modulate() const;
	// #endif

	void fade_in(const godot::String &_name_anim, float _time, int _loop, int _layer, const godot::String &_group, GDDragonBones::AnimFadeOutMode _fade_out_mode);
	void fade_out(const godot::String &_name_anim);

	void set_active(bool _b_active);
	bool is_active() const;

	void set_debug(bool _b_debug);
	bool is_debug() const;

	void set_speed(float _f_speed);
	float get_speed() const;

	void set_texture(const godot::Ref<godot::Texture2D> &_p_texture);
	godot::Ref<godot::Texture2D> get_texture() const;

	void set_current_animation(const godot::String &p_anim);
	godot::String get_current_animation() const;

	float tell() const;
	void seek(float _f_p);
	float get_progress() const;

	void set_animation_process_mode(AnimationCallbackModeProcess _mode);
	AnimationCallbackModeProcess get_animation_process_mode() const;

	void play(bool _b_play = true);
	void play_from_time(float _f_time);
	void play_from_progress(float _f_progress);

	void advance(float p_delta) {
		p_factory->update(p_delta);
	}

	void flip_x(bool _b_flip);
	bool is_fliped_x() const;

	void flip_y(bool _b_flip);
	bool is_fliped_y() const;

	bool is_playing() const;

	void set_loop(bool p_loop);
	bool is_loop() const { return c_loop; }

	bool has_anim(const godot::StringName &_str_anim) const;
	void stop(bool _b_all = false);
	inline void stop_all() { stop(true); }
};

VARIANT_ENUM_CAST(GDDragonBones::AnimationCallbackModeProcess);
VARIANT_ENUM_CAST(GDDragonBones::AnimFadeOutMode);
