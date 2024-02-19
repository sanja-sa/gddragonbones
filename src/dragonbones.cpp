#include "dragonbones.h"

#include "dragonBones/animation/WorldClock.h"
#include "dragonBones/model/ArmatureData.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/error_macros.hpp"

#include "wrappers/DragonBonesArmature.h"

using namespace godot;

#define SNAME(sn) ([] {static const StringName ret{sn};return ret; }())

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Plugin module

void DragonBones::_cleanup() {
	b_inited = false;

	if (p_armature) {
		if (p_armature->get_parent() == this) {
			remove_child(p_armature);
			p_armature->dispose(true);
		}
		p_armature->queue_free();
		p_armature = nullptr;
	}

	if (p_instance) {
		memdelete(p_instance);
		p_instance = nullptr;
	}

	m_res.unref();
}

void DragonBones::dispatch_sound_event(const String &_str_type, const dragonBones::EventObject *_p_value) {
	using namespace dragonBones;
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	if (_str_type == EventObject::SOUND_EVENT)
		emit_signal("dragon_anim_snd_event", String(_p_value->animationState->name.c_str()), String(_p_value->name.c_str()));
}

void DragonBones::dispatch_event(const String &_str_type, const dragonBones::EventObject *_p_value) {
	using namespace dragonBones;
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	if (_str_type == EventObject::START)
		emit_signal("dragon_anim_start", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::LOOP_COMPLETE)
		emit_signal("dragon_anim_loop_complete", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::COMPLETE)
		emit_signal("dragon_anim_complete", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FRAME_EVENT) {
		int int_val = 0;
		int float_val = 0;
		const char *string_val = "";
		UserData *data = _p_value->getData();
		Armature *armature = _p_value->getArmature();

		if (data != NULL) {
			int_val = _p_value->getData()->getInt(0);
			float_val = _p_value->getData()->getFloat(0);

			if (!data->getStrings().empty()) {
				string_val = _p_value->getData()->getString(0).c_str();
			}
		}

		Dictionary dict = Dictionary();

		dict[SNAME("armature")] = String(armature->getName().c_str());
		dict[SNAME("animation")] = String(_p_value->animationState->name.c_str());
		dict[SNAME("event_name")] = String(_p_value->name.c_str());
		dict[SNAME("int")] = int_val;
		dict[SNAME("float")] = float_val;
		dict[SNAME("string")] = string_val;

		emit_signal("dragon_anim_event", dict);
	} else if (_str_type == EventObject::FADE_IN)
		emit_signal("dragon_fade_in", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_IN_COMPLETE)
		emit_signal("dragon_fade_in_complete", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_OUT)
		emit_signal("dragon_fade_out", String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_OUT_COMPLETE)
		emit_signal("dragon_fade_out_complete", String(_p_value->animationState->name.c_str()));
}

void DragonBones::_on_resource_changed() {
	if (m_res->texture_atlas_file_raw_data.is_empty() || m_res->bones_file_raw_data.is_empty()) {
		return;
	}

	// 重设资源本身
	auto to_set = m_res;
	set_resource({});
	set_resource(to_set);
}

void DragonBones::set_resource(Ref<DragonBonesResource> _p_data) {
	using namespace dragonBones;
	if (m_res == _p_data)
		return;

	Ref<Texture2D> old_texture;
	if (m_res.is_valid())
		old_texture = m_res->default_texture;
	else if (_p_data.is_valid())
		old_texture = _p_data->default_texture;

	if (p_armature) {
		p_armature->stop_all_animations();
	}

	_cleanup();

	static const StringName sn{ "changed" };
	auto cb = callable_mp(this, &DragonBones::_on_resource_changed);
	if (m_res.is_valid() && m_res->is_connected(sn, cb)) {
		m_res->disconnect(sn, cb);
	}

	m_res = _p_data;

	if (m_res.is_null()) {
		// m_texture_atlas.unref();
		notify_property_list_changed();
		return;
	} else if (!m_res->is_connected(sn, cb)) {
		m_res->connect(sn, cb);
	}

	if (m_res->texture_atlas_file_raw_data.is_empty() || m_res->bones_file_raw_data.is_empty()) {
		WARN_PRINT(vformat("DragonBonesResource \"%s\" is invalid, please setup its properties.", m_res));
		return;
	}

	TextureAtlasData *__p_tad = DragonBonesFactory::get_singleton()->loadTextureAtlasData((char *)m_res->texture_atlas_file_raw_data.ptr(), nullptr); // TODO 内存安全
	ERR_FAIL_COND(!__p_tad);
	DragonBonesData *__p_dbd = DragonBonesFactory::get_singleton()->loadDragonBonesData((char *)m_res->bones_file_raw_data.ptr(), ""); // TODO 内存安全
	ERR_FAIL_COND(!__p_dbd);

	// build Armature display
	const std::vector<std::string> &__r_v_m_names = __p_dbd->getArmatureNames();
	ERR_FAIL_COND(!__r_v_m_names.size());

	p_instance = memnew(dragonBones::DragonBones(this));
	DragonBonesFactory::get_singleton()->set_building_dragonbones_instance(p_instance);

	// 仅支持单个骨架
	p_armature = DragonBonesFactory::get_singleton()->buildArmatureDisplay(__r_v_m_names[0].c_str(), __p_dbd->name);

	// add children armature
	p_armature->p_owner = this;

	// To support non-texture atlas; I'd want to look around here
	if (!m_texture_atlas.is_valid() || old_texture != m_res->default_texture)
		m_texture_atlas = m_res->default_texture;

	// correction for old version of DB tad files (Zero width, height)
	if (m_texture_atlas.is_valid()) {
		__p_tad->height = m_texture_atlas->get_height();
		__p_tad->width = m_texture_atlas->get_width();
	}

	// update flip
	p_armature->getArmature()->setFlipX(b_flip_x);
	p_armature->getArmature()->setFlipY(b_flip_y);

	p_armature->setup_recursively(b_debug, m_texture_atlas);
	// add main armature
	add_child(p_armature);

	// p_armature->force_parent_owned(); // ??
	b_inited = true;

	// update color and opacity and blending
	p_armature->update_childs(true, true);

	// update material inheritance
	p_armature->update_material_inheritance(b_inherit_child_material);

	p_armature->getArmature()->advanceTime(0);

	notify_property_list_changed();
	queue_redraw();
}

Ref<DragonBonesResource> DragonBones::get_resource() {
	return m_res;
}

void DragonBones::set_inherit_material(bool _b_enable) {
	b_inherit_child_material = _b_enable;
	if (p_armature)
		p_armature->update_material_inheritance(b_inherit_child_material);
}

bool DragonBones::is_material_inherited() const {
	return b_inherit_child_material;
}

void DragonBones::set_active(bool _b_active) {
	if (b_active == _b_active)
		return;
	b_active = _b_active;
}

bool DragonBones::is_active() const {
	return b_active;
}

void DragonBones::set_debug(bool _b_debug) {
	b_debug = _b_debug;
	if (b_inited)
		p_armature->set_debug(b_debug, true);
}

bool DragonBones::is_debug() const {
	return b_debug;
}

void DragonBones::set_speed(float _f_speed) {
	f_speed = _f_speed;
	if (b_inited)
		p_instance->getClock()->timeScale = _f_speed;
}

float DragonBones::get_speed() const {
	return f_speed;
}

void DragonBones::set_animation_process_mode(DragonBonesArmature::AnimationCallbackModeProcess _mode) {
	if (m_anim_mode == _mode)
		return;
	m_anim_mode = _mode;
}

DragonBonesArmature::AnimationCallbackModeProcess DragonBones::get_animation_process_mode() const {
	return m_anim_mode;
}

void DragonBones::_notification(int _what) {
	switch (_what) {
		case NOTIFICATION_READY: {
			set_resource(get_resource());
		} break;
		case NOTIFICATION_PROCESS: {
			if (b_active && m_anim_mode == DragonBonesArmature::ANIMATION_CALLBACK_MODE_PROCESS_IDLE)
				advance(get_process_delta_time());
		} break;

		case NOTIFICATION_PHYSICS_PROCESS: {
			if (b_active && m_anim_mode == DragonBonesArmature::ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS)
				advance(get_physics_process_delta_time());
		} break;
	}
}

void DragonBones::_reset() {
	ERR_FAIL_NULL(p_armature);
	p_armature->getAnimation()->reset();
}

const dragonBones::DragonBonesData *DragonBones::get_dragonbones_data() const {
	const dragonBones::DragonBonesData *data = this->p_armature->getArmature()->getArmatureData()->getParent();
	return data;
}

dragonBones::ArmatureData *DragonBones::get_armature_data(const String &_armature_name) {
	auto it = get_dragonbones_data()->armatures.find(_armature_name.ascii().get_data());

	if (it == get_dragonbones_data()->armatures.end()) {
		return nullptr;
	}

	return it->second;
}

DragonBonesArmature *DragonBones::get_armature() {
	return p_armature;
}

void DragonBones::flip_x(bool _b_flip) {
	b_flip_x = _b_flip;
	if (p_armature) {
		p_armature->flip_x(_b_flip);
	}
}

bool DragonBones::is_fliped_x() const {
	if (p_armature) {
		return p_armature->is_flipped_x();
	}
	return b_flip_x;
}

void DragonBones::flip_y(bool _b_flip) {
	b_flip_y = _b_flip;
	if (!p_armature) {
		p_armature->flip_y(_b_flip);
	}
}

bool DragonBones::is_fliped_y() const {
	if (p_armature) {
		return p_armature->is_flipped_y();
	}
	return b_flip_y;
}

#ifdef COMPATIBILITY_ENABLED
void DragonBones::fade_in(const String &_name_anim, float _time, int _loop, int _layer, const String &_group, DragonBonesArmature::AnimFadeOutMode _fade_out_mode) {
	// setup speed
	p_factory->set_speed(f_speed);
	ERR_FAIL_NULL(p_armature);
	p_armature->fade_in(_name_anim, _time, _loop, _layer, _group, _fade_out_mode);
	b_playing = true;
}

void DragonBones::fade_out(const String &_name_anim) {
	ERR_FAIL_NULL(p_armature);
	if (!p_armature->getAnimation()->isPlaying() || !p_armature->getAnimation()->hasAnimation(_name_anim.ascii().get_data()))
		return;

	p_armature->stop(_name_anim.ascii().get_data(), true);

	b_playing = false;
}

void DragonBones::set_slot_display_index(const String &_slot_name, int _index) {
	ERR_FAIL_NULL(p_armature);
	p_armature->set_slot_display_index(_slot_name, _index);
}

void DragonBones::set_slot_by_item_name(const String &_slot_name, const String &_item_name) {
	ERR_FAIL_NULL(p_armature);
	p_armature->set_slot_by_item_name(_slot_name, _item_name);
}

void DragonBones::set_all_slots_by_item_name(const String &_item_name) {
	ERR_FAIL_NULL(p_armature);
	p_armature->set_all_slots_by_item_name(_item_name);
}

int DragonBones::get_slot_display_index(const String &_slot_name) {
	ERR_FAIL_NULL_V(p_armature, -1);
	return p_armature->get_slot_display_index(_slot_name);
}

int DragonBones::get_total_items_in_slot(const String &_slot_name) {
	ERR_FAIL_NULL_V(p_armature, -1);
	return p_armature->get_total_items_in_slot(_slot_name);
}

bool DragonBones::has_slot(const String &_slot_name) const {
	ERR_FAIL_NULL_V(p_armature, false);
	return p_armature->has_slot(_slot_name);
}

void DragonBones::cycle_next_item_in_slot(const String &_slot_name) {
	ERR_FAIL_NULL(p_armature);
	p_armature->cycle_next_item_in_slot(_slot_name);
}

void DragonBones::cycle_previous_item_in_slot(const String &_slot_name) {
	ERR_FAIL_NULL(p_armature);
	p_armature->cycle_next_item_in_slot(_slot_name);
}

Color DragonBones::get_slot_display_color_multiplier(const String &_slot_name) {
	ERR_FAIL_NULL_V((p_armature), (Color(-1.0, -1.0, -1.0, -1.0)));
	return p_armature->get_slot_display_color_multiplier(_slot_name);
}

void DragonBones::set_slot_display_color_multiplier(const String &_slot_name, const Color &_color) {
	ERR_FAIL_NULL(p_armature);
	p_armature->set_slot_display_color_multiplier(_slot_name, _color);
}

void DragonBones::play(bool _b_play) {
	b_playing = _b_play;
	if (!_b_play) {
		stop();
		return;
	}

	// setup speed
	p_factory->set_speed(f_speed);
	if (p_armature && p_armature->has_animation(str_curr_anim)) {
		p_armature->play(str_curr_anim, c_loop);
		b_try_playing = false;
	} else {
		// not finded animation stop playing
		b_try_playing = true;
		str_curr_anim = "[none]";
		stop();
	}
}

void DragonBones::play_from_time(float _f_time) {
	play();
	if (b_playing && p_armature)
		p_armature->getAnimation()->gotoAndPlayByTime(str_curr_anim.ascii().get_data(), _f_time, c_loop);
}

void DragonBones::play_from_progress(float _f_progress) {
	play();
	if (b_playing && p_armature)
		p_armature->getAnimation()->gotoAndPlayByProgress(str_curr_anim.ascii().get_data(), CLAMP(_f_progress, 0, 1.f), c_loop);
}

void DragonBones::play_new_animation_from_progress(const String &_str_anim, int _num_times, float _f_progress) {
	stop_all();
	_set("playback/curr_animation", _str_anim);
	_set("playback/loop", _num_times);
	play(true);

	play_from_progress(_f_progress);
}

void DragonBones::play_new_animation_from_time(const String &_str_anim, int _num_times, float _f_time) {
	stop_all();
	_set("playback/curr_animation", _str_anim);
	_set("playback/loop", _num_times);
	play(true);

	play_from_time(_f_time);
}

void DragonBones::play_new_animation(const String &_str_anim, int _num_times) {
	stop_all();
	_set("playback/curr_animation", _str_anim);
	_set("playback/loop", _num_times);
	play(true);
}

bool DragonBones::has_anim(const String &_str_anim) {
	ERR_FAIL_NULL_V(p_armature, false);
	return p_armature->has_animation(_str_anim);
}

void DragonBones::stop(bool _b_all) {
	if (!b_inited)
		return;

	b_playing = false;

	if (p_armature && p_armature->getAnimation()->isPlaying())
		p_armature->stop(_b_all ? std::move(String("")) : str_curr_anim, true);
}

float DragonBones::tell() {
	ERR_FAIL_NULL_V(p_armature, 0.0f);
	return p_armature->tell_animation(str_curr_anim);
}

void DragonBones::seek(float _f_p) {
	ERR_FAIL_NULL(p_armature);
	b_playing = false;
	f_progress = _f_p;
	p_armature->seek_animation(str_curr_anim, _f_p);
}

float DragonBones::get_progress() const {
	if (p_armature) {
		auto anim = p_armature->getArmature()->getAnimation();
		if (anim) {
			if (auto state = anim->getState(str_curr_anim.ascii().get_data())) {
				return state->getCurrentTime() / state->getTotalTime();
			}
		}
	}
	return f_progress;
}

bool DragonBones::is_playing() const {
	if (p_armature) {
		return p_armature->is_playing();
	}
	return false;
}

String DragonBones::get_current_animation() const {
	if (p_armature) {
		return p_armature->get_current_animation();
	}
	return {};
}

String DragonBones::get_current_animation_on_layer(int _layer) const {
	if (p_armature) {
		return p_armature->get_current_animation_on_layer(_layer);
	}
	return {};
}
#endif

void DragonBones::set_texture(const Ref<Texture2D> &_p_texture) {
	if (_p_texture.is_valid() && m_texture_atlas.is_valid() && (_p_texture == m_texture_atlas || m_texture_atlas->get_height() != _p_texture->get_height() || m_texture_atlas->get_width() != _p_texture->get_width()))
		return;

	m_texture_atlas = _p_texture;

	if (p_armature) {
		p_armature->update_texture_atlas(m_texture_atlas);
		queue_redraw();
	}
}

Ref<Texture2D> DragonBones::get_texture() const {
	return m_texture_atlas;
}

bool DragonBones::_set(const StringName &_str_name, const Variant &_c_r_value) {
	String name = _str_name;

	if (name == "playback/curr_animation") {
		if (str_curr_anim == _c_r_value)
			return false;

		str_curr_anim = _c_r_value;
		if (b_inited) {
			if (str_curr_anim == "[none]") {
				p_armature->stop("");
			} else if (p_armature->has_animation(str_curr_anim)) {
				if (b_playing || b_try_playing) {
					p_armature->play(str_curr_anim);
				} else {
					p_armature->getAnimation()->gotoAndStopByProgress(str_curr_anim.ascii().get_data());
				}
			}
		}
		return true;
	} else if (name == "playback/loop") {
		c_loop = _c_r_value;
		if (b_inited && b_playing) {
			_reset();
			p_armature->play(str_curr_anim);
		}
		return true;
	} else if (name == "playback/progress") {
		p_armature->seek_animation(str_curr_anim, _c_r_value);
		return true;
	}

	return false;
}

bool DragonBones::_get(const StringName &_str_name, Variant &_r_ret) const {
	String __name = _str_name;

	if (__name == "playback/curr_animation") {
		_r_ret = str_curr_anim;
		return true;
	} else if (__name == "playback/loop") {
		_r_ret = c_loop;
		return true;
	} else if (__name == "playback/progress") {
		_r_ret = f_progress;
		return true;
	}
	return false;
}

void DragonBones::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &DragonBones::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &DragonBones::get_texture);

	ClassDB::bind_method(D_METHOD("set_resource", "dragonbones"), &DragonBones::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &DragonBones::get_resource);

	ClassDB::bind_method(D_METHOD("set_inherit_material"), &DragonBones::set_inherit_material);
	ClassDB::bind_method(D_METHOD("is_material_inherited"), &DragonBones::is_material_inherited);

	ClassDB::bind_method(D_METHOD("flip_x", "enable_flip"), &DragonBones::flip_x);
	ClassDB::bind_method(D_METHOD("is_fliped_x"), &DragonBones::is_fliped_x);
	ClassDB::bind_method(D_METHOD("flip_y", "enable_flip"), &DragonBones::flip_y);
	ClassDB::bind_method(D_METHOD("is_fliped_y"), &DragonBones::is_fliped_y);

#ifdef COMPATIBILITY_ENABLED
	/*
		All these functions act upon the base armature / display; a structure is being formed to make them available for all displays and armatures
	*/
	ClassDB::bind_method(D_METHOD("fade_in", "anim_name", "time", "loop", "layer", "group", "fade_out_mode"), &DragonBones::fade_in);
	ClassDB::bind_method(D_METHOD("fade_out", "anim_name"), &DragonBones::fade_out);

	ClassDB::bind_method(D_METHOD("stop"), &DragonBones::stop);
	ClassDB::bind_method(D_METHOD("stop_all"), &DragonBones::stop_all);
	ClassDB::bind_method(D_METHOD("reset"), &DragonBones::_reset);
	ClassDB::bind_method(D_METHOD("has_slot"), &DragonBones::has_slot);
	ClassDB::bind_method(D_METHOD("set_slot_by_item_name"), &DragonBones::set_slot_by_item_name);
	ClassDB::bind_method(D_METHOD("set_all_slots_by_item_name"), &DragonBones::set_all_slots_by_item_name);
	ClassDB::bind_method(D_METHOD("set_slot_display_index"), &DragonBones::set_slot_display_index);
	ClassDB::bind_method(D_METHOD("get_slot_display_index"), &DragonBones::get_slot_display_index);
	ClassDB::bind_method(D_METHOD("get_total_items_in_slot"), &DragonBones::get_total_items_in_slot);
	ClassDB::bind_method(D_METHOD("set_slot_display_color_multiplier"), &DragonBones::set_slot_display_color_multiplier);
	ClassDB::bind_method(D_METHOD("get_slot_display_color_multiplier"), &DragonBones::get_slot_display_color_multiplier);
	ClassDB::bind_method(D_METHOD("cycle_next_item_in_slot"), &DragonBones::cycle_next_item_in_slot);
	ClassDB::bind_method(D_METHOD("cycle_previous_item_in_slot"), &DragonBones::cycle_previous_item_in_slot);

	ClassDB::bind_method(D_METHOD("play"), &DragonBones::play);
	ClassDB::bind_method(D_METHOD("play_from_time"), &DragonBones::play_from_time);
	ClassDB::bind_method(D_METHOD("play_from_progress"), &DragonBones::play_from_progress);
	ClassDB::bind_method(D_METHOD("play_new_animation"), &DragonBones::play_new_animation);
	ClassDB::bind_method(D_METHOD("play_new_animation_from_progress"), &DragonBones::play_new_animation_from_progress);
	ClassDB::bind_method(D_METHOD("play_new_animation_from_time"), &DragonBones::play_new_animation_from_time);

	ClassDB::bind_method(D_METHOD("seek", "pos"), &DragonBones::seek);
	ClassDB::bind_method(D_METHOD("tell"), &DragonBones::tell);
	ClassDB::bind_method(D_METHOD("get_progress"), &DragonBones::get_progress);

	ClassDB::bind_method(D_METHOD("has", "name"), &DragonBones::has_anim);
	ClassDB::bind_method(D_METHOD("is_playing"), &DragonBones::is_playing);

	ClassDB::bind_method(D_METHOD("get_current_animation"), &DragonBones::get_current_animation);
	ClassDB::bind_method(D_METHOD("get_current_animation_on_layer"), &DragonBones::get_current_animation_on_layer);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "playback/progress", PROPERTY_HINT_RANGE, "-100,100,0.010"), "seek", "get_progress");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "playback/play"), "play", "is_playing");
#endif
	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &DragonBones::set_speed);
	ClassDB::bind_method(D_METHOD("get_speed"), &DragonBones::get_speed);

	ClassDB::bind_method(D_METHOD("get_armature"), &DragonBones::get_armature);
	/*
		END OF BASE ARMATURE FUNCTIONS
	*/

	/*
		Let's get into the business of playing with child armatures.
	*/

	ClassDB::bind_method(D_METHOD("set_active", "active"), &DragonBones::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &DragonBones::is_active);

	ClassDB::bind_method(D_METHOD("set_debug", "debug"), &DragonBones::set_debug);
	ClassDB::bind_method(D_METHOD("is_debug"), &DragonBones::is_debug);

	ClassDB::bind_method(D_METHOD("set_animation_process_mode", "mode"), &DragonBones::set_animation_process_mode);
	ClassDB::bind_method(D_METHOD("get_animation_process_mode"), &DragonBones::get_animation_process_mode);

	// This is how we set top level properties
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug"), "set_debug", "is_debug");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flipX"), "flip_x", "is_fliped_x");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flipY"), "flip_y", "is_fliped_y");

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "DragonBonesResource"), "set_resource", "get_resource");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "playback/process_mode", PROPERTY_HINT_ENUM, "Physics,Idle,Manual"), "set_animation_process_mode", "get_animation_process_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "playback/speed", PROPERTY_HINT_RANGE, "-10,10,0.01"), "set_speed", "get_speed");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "childs_use_this_material"), "set_inherit_material", "is_material_inherited");

	ADD_SIGNAL(MethodInfo("dragon_anim_start", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_anim_complete", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_anim_event", PropertyInfo(Variant::DICTIONARY, "event")));
	ADD_SIGNAL(MethodInfo("dragon_anim_loop_complete", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_anim_snd_event", PropertyInfo(Variant::STRING, "anim"), PropertyInfo(Variant::STRING, "ev")));
	ADD_SIGNAL(MethodInfo("dragon_fade_in", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_fade_in_complete", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_fade_out", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("dragon_fade_out_complete", PropertyInfo(Variant::STRING, "anim")));
}

void DragonBones::_get_property_list(List<PropertyInfo> *_p_list) const {
	List<String> __l_names;

	if (b_inited && p_armature->getAnimation()) {
		auto __names = p_armature->getAnimation()->getAnimationNames();
		auto __it = __names.cbegin();
		while (__it != __names.cend()) {
			__l_names.push_back(__it->c_str());
			++__it;
		}
	}

	__l_names.sort();
	__l_names.push_front("[none]");
	String __str_hint;
	for (List<String>::Element *__p_E = __l_names.front(); __p_E; __p_E = __p_E->next()) {
		if (__p_E != __l_names.front())
			__str_hint += ",";
		__str_hint += __p_E->get();
	}

	_p_list->push_back(PropertyInfo(Variant::STRING, "playback/curr_animation", PROPERTY_HINT_ENUM, __str_hint));
	_p_list->push_back(PropertyInfo(Variant::INT, "playback/loop", PROPERTY_HINT_RANGE, "-1,100,1"));
}
