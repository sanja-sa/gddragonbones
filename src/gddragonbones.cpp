#include "gddragonbones.h"

#include "dragonBones/core/DragonBones.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/class_db.hpp"
using namespace godot;

#define SNAME(n) [] {static const godot::StringName ret{n}; return ret; }()
//////////////////////////////////////////////////////////////////
//// Resource
GDDragonBonesResource::GDDragonBonesResource() {
	p_data_texture_atlas = nullptr;
	p_data_bones = nullptr;
}

GDDragonBonesResource::~GDDragonBonesResource() {
	if (p_data_texture_atlas) {
		memfree(p_data_texture_atlas);
		p_data_texture_atlas = nullptr;
	}

	if (p_data_bones) {
		memfree(p_data_bones);
		p_data_bones = nullptr;
	}
}

char *__load_file(const String &_file_path) {
	auto file = FileAccess::open(_file_path, FileAccess::READ);
	ERR_FAIL_NULL_V(file, nullptr);
	ERR_FAIL_COND_V(!file->get_length(), nullptr);

	// mem
	char *data = (char *)memalloc(file->get_length() + 1);
	ERR_FAIL_COND_V(!data, nullptr);

	file->get_buffer((uint8_t *)data, file->get_length());
	data[file->get_length()] = 0x00;

	file.unref();
	return data;
}

void GDDragonBonesResource::set_def_texture_path(const String &_path) {
	str_default_tex_path = _path;
}

bool GDDragonBonesResource::load_texture_atlas_data(const String &_path) {
	p_data_texture_atlas = __load_file(_path);
	ERR_FAIL_COND_V(!p_data_texture_atlas, false);
	return true;
}

bool GDDragonBonesResource::load_bones_data(const String &_path) {
	p_data_bones = __load_file(_path);
	ERR_FAIL_COND_V(!p_data_bones, false);
	return true;
}

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Plugin module
GDDragonBones::GDDragonBones() {}

GDDragonBones::~GDDragonBones() {
	_cleanup();

	if (p_factory) {
		memdelete(p_factory);
		p_factory = nullptr;
	}
}

void GDDragonBones::_cleanup() {
	b_inited = false;

	if (p_factory)
		p_factory->clear();

	if (p_armature) {
		if (p_armature->is_inside_tree())
			remove_child(p_armature);
		p_armature = nullptr;
	}

	m_res.unref();
}

void GDDragonBones::dispatch_snd_event(const String &_str_type, const dragonBones::EventObject *_p_value) {
	if (Engine::get_singleton()->is_editor_hint())
		return;

	if (_str_type == dragonBones::EventObject::SOUND_EVENT)
		emit_signal("animation_snd_event", String(_p_value->animationState->name.c_str()), String(_p_value->name.c_str()));
}

void GDDragonBones::dispatch_event(const String &_str_type, const dragonBones::EventObject *_p_value) {
	using EventObject = dragonBones::EventObject;
	if (Engine::get_singleton()->is_editor_hint())
		return;

	if (_str_type == EventObject::START)
		emit_signal(SNAME("animation_start"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::LOOP_COMPLETE)
		emit_signal(SNAME("animation_loop_completed"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::COMPLETE)
		emit_signal(SNAME("animation_completed"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FRAME_EVENT)
		emit_signal(SNAME("animation_event"), String(_p_value->animationState->name.c_str()), String(_p_value->name.c_str()));
	else if (_str_type == EventObject::FADE_IN)
		emit_signal(SNAME("fade_in"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_IN_COMPLETE)
		emit_signal(SNAME("fade_in_completed"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_OUT)
		emit_signal(SNAME("fade_out"), String(_p_value->animationState->name.c_str()));
	else if (_str_type == EventObject::FADE_OUT_COMPLETE)
		emit_signal(SNAME("fade_out_completed"), String(_p_value->animationState->name.c_str()));
}

void GDDragonBones::set_resource(Ref<GDDragonBonesResource> _p_data) {
	using namespace dragonBones;

	String __old_texture_path = "";
	if (m_res.is_valid())
		__old_texture_path = m_res->str_default_tex_path;
	else if (_p_data.is_valid())
		__old_texture_path = _p_data->str_default_tex_path;

	if (m_res == _p_data)
		return;

	stop();
	_cleanup();

	m_res = _p_data;
	if (m_res.is_null()) {
		m_texture_atlas = Ref<Texture>();
		ERR_PRINT("Null resources");
		notify_property_list_changed();
		return;
	}

	ERR_FAIL_COND(!m_res->p_data_texture_atlas);
	ERR_FAIL_COND(!m_res->p_data_bones);

	TextureAtlasData *__p_tad = p_factory->loadTextureAtlasData(m_res->p_data_texture_atlas, nullptr);
	ERR_FAIL_COND(!__p_tad);
	DragonBonesData *__p_dbd = p_factory->loadDragonBonesData(m_res->p_data_bones);
	ERR_FAIL_COND(!__p_dbd);

	// build Armature display
	const std::vector<std::string> &__r_v_m_names = __p_dbd->getArmatureNames();
	ERR_FAIL_COND(!__r_v_m_names.size());

	p_armature = static_cast<GDArmatureDisplay *>(p_factory->buildArmatureDisplay(__r_v_m_names[0].c_str()));
	// add children armature
	p_armature->p_owner = this;

	if (!m_texture_atlas.is_valid() || __old_texture_path != m_res->str_default_tex_path)
		m_texture_atlas = godot::ResourceLoader::get_singleton()->load(m_res->str_default_tex_path);

	// correction for old version of DB tad files (Zero width, height)
	if (m_texture_atlas.is_valid()) {
		__p_tad->height = m_texture_atlas->get_height();
		__p_tad->width = m_texture_atlas->get_width();
	}

	p_armature->add_parent_class(b_debug, m_texture_atlas);
	// add main armature
	add_child(p_armature);

	b_inited = true;

	// update color and opacity and blending
	p_armature->update_childs(true, true);

	// update material inheritance
	p_armature->update_material_inheritance(b_inherit_child_material);

	// update flip
	p_armature->getArmature()->setFlipX(b_flip_x);
	p_armature->getArmature()->setFlipY(b_flip_y);
	p_armature->getArmature()->advanceTime(0);

	notify_property_list_changed();
	queue_redraw();
}

Ref<GDDragonBonesResource> GDDragonBones::get_resource() {
	return m_res;
}

void GDDragonBones::set_inherit_material(bool _b_enable) {
	b_inherit_child_material = _b_enable;
	if (p_armature)
		p_armature->update_material_inheritance(b_inherit_child_material);
}

bool GDDragonBones::is_material_inherited() const {
	return b_inherit_child_material;
}

// #if (VERSION_MAJOR == 3)
// #else
// void GDDragonBones::set_opacity(float _f_opacity) {
// 	GDOwnerNode::set_opacity(_f_opacity);
// 	if (p_armature)
// 		p_armature->update_childs(true);
// }

// float GDDragonBones::get_opacity() const {
// #ifdef TOOLS_ENABLED
// 	if (p_armature)
// 		p_armature->update_childs(true);
// #endif
// 	return GDOwnerNode::get_opacity();
// }

// void GDDragonBones::set_blend_mode(godot::CanvasItemMaterial::BlendMode _blend_mode) {
// 	GDOwnerNode::set_blend_mode(_blend_mode);
// 	if (p_armature)
// 		p_armature->update_childs(false, true);
// }

// CanvasItem::BlendMode GDDragonBones::get_blend_mode() const {
// #ifdef TOOLS_ENABLED
// 	if (p_armature)
// 		p_armature->update_childs(false, true);
// #endif
// 	return GDOwnerNode::get_blend_mode();
// }

// void GDDragonBones::set_modulate(const Color &_p_color) {
// 	modulate = _p_color;
// 	if (p_armature)
// 		p_armature->update_childs(true);
// }

// Color GDDragonBones::get_modulate() const {
// 	return modulate;
// }
// #endif

void GDDragonBones::fade_in(const String &_name_anim, float _time, int _loop, int _layer, const String &_group, GDDragonBones::AnimFadeOutMode _fade_out_mode) {
	// setup speed
	p_factory->set_speed(f_speed);
	if (has_anim(_name_anim)) {
		p_armature->getAnimation()->fadeIn(_name_anim.ascii().get_data(), _time, _loop, _layer, _group.ascii().get_data(), (dragonBones::AnimationFadeOutMode)_fade_out_mode);
		if (!b_playing) {
			b_playing = true;
			// _set_process(true);
		}
	}
}

void GDDragonBones::fade_out(const String &_name_anim) {
	if (!b_inited)
		return;

	if (!p_armature->getAnimation()->isPlaying() || !p_armature->getAnimation()->hasAnimation(_name_anim.ascii().get_data()))
		return;

	p_armature->getAnimation()->stop(_name_anim.ascii().get_data());

	if (p_armature->getAnimation()->isPlaying())
		return;

	// _set_process(false);
	b_playing = false;

	_reset();
}

void GDDragonBones::set_active(bool _b_active) {
	if (b_active == _b_active)
		return;
	b_active = _b_active;
	// _set_process(b_processing, true);
}

bool GDDragonBones::is_active() const {
	return b_active;
}

void GDDragonBones::set_debug(bool _b_debug) {
	b_debug = _b_debug;
	if (b_inited)
		p_armature->set_debug(b_debug);
}

bool GDDragonBones::is_debug() const {
	return b_debug;
}

void GDDragonBones::flip_x(bool _b_flip) {
	b_flip_x = _b_flip;
	if (!p_armature)
		return;
	p_armature->getArmature()->setFlipX(_b_flip);
	p_armature->getArmature()->advanceTime(0);
}

bool GDDragonBones::is_fliped_x() const {
	return b_flip_x;
}

void GDDragonBones::flip_y(bool _b_flip) {
	b_flip_y = _b_flip;
	if (!p_armature)
		return;
	p_armature->getArmature()->setFlipY(_b_flip);
	p_armature->getArmature()->advanceTime(0);
}

bool GDDragonBones::is_fliped_y() const {
	return b_flip_y;
}

void GDDragonBones::set_speed(float _f_speed) {
	f_speed = _f_speed;
	if (b_inited)
		p_factory->set_speed(_f_speed);
}

float GDDragonBones::get_speed() const {
	return f_speed;
}

void GDDragonBones::set_animation_process_mode(AnimationCallbackModeProcess _mode) {
	if (m_anim_mode == _mode) {
		return;
	}
	// bool __pr = b_processing;
	// if (__pr)
	// 	_set_process(false);
	m_anim_mode = _mode;
	// if (__pr)
	// 	_set_process(true);
}

GDDragonBones::AnimationCallbackModeProcess GDDragonBones::get_animation_process_mode() const {
	return m_anim_mode;
}

void GDDragonBones::_notification(int _what) {
	switch (_what) {
		case NOTIFICATION_READY: {
			set_process(true);
			set_physics_process(true);
			if (b_playing && b_inited) {
				play();
			}
		} break;
		case NOTIFICATION_PROCESS: {
			if (m_anim_mode == ANIMATION_CALLBACK_MODE_PROCESS_IDLE && b_active) {
				advance(get_process_delta_time());
			}
		} break;

		case NOTIFICATION_PHYSICS_PROCESS: {
			if (m_anim_mode == ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS && b_active) {
				advance(get_physics_process_delta_time());
			}
		} break;
	}
}

void GDDragonBones::_reset() {
	p_armature->getAnimation()->reset();
}

void GDDragonBones::play(bool _b_play) {
	b_playing = _b_play;
	if (!_b_play) {
		stop();
		return;
	}

	// setup speed
	p_factory->set_speed(f_speed);
	if (has_anim(str_curr_anim)) {
		p_armature->getAnimation()->play(get_curr_anim_c_str(), c_loop);
		// _set_process(true);
		b_try_playing = false;
	}

	else // not finded animation stop playing
	{
		b_try_playing = true;
		str_curr_anim = SNAME("[none]");
		stop();
	}
}

void GDDragonBones::play_from_time(float _f_time) {
	play();
	if (b_playing)
		p_armature->getAnimation()->gotoAndPlayByTime(get_curr_anim_c_str(), _f_time, c_loop);
}

void GDDragonBones::play_from_progress(float _f_progress) {
	play();
	if (b_playing)
		p_armature->getAnimation()->gotoAndPlayByProgress(get_curr_anim_c_str(), CLAMP(_f_progress, 0, 1.f), c_loop);
}

void GDDragonBones::set_loop(bool p_loop) {
	c_loop = p_loop;
	if (b_inited && b_playing) {
		_reset();
		play();
	}
}

bool GDDragonBones::has_anim(const StringName &_str_anim) const {
	return p_armature->getAnimation()->hasAnimation(to_c_str(_str_anim));
}

void GDDragonBones::stop(bool _b_all) {
	if (!b_inited)
		return;

	// _set_process(false);
	b_playing = false;

	if (p_armature->getAnimation()->isPlaying())
		p_armature->getAnimation()->stop(_b_all ? "" : get_curr_anim_c_str());

	_reset();
}

float GDDragonBones::tell() const {
	if (b_inited && has_anim(str_curr_anim)) {
		dragonBones::AnimationState *__p_state = p_armature->getAnimation()->getState(get_curr_anim_c_str());
		if (__p_state)
			return __p_state->getCurrentTime() / __p_state->_duration;
	}
	return 0;
}

void GDDragonBones::seek(float _f_p) {
	if (b_inited && has_anim(str_curr_anim)) {
		f_progress = _f_p;
		stop();
		auto __c_p = Math::fmod(_f_p, 1.0f);
		if (__c_p == 0 && _f_p != 0)
			__c_p = 1.0f;
		p_armature->getAnimation()->gotoAndStopByProgress(get_curr_anim_c_str(), __c_p < 0 ? 1. + __c_p : __c_p);
	}
}

float GDDragonBones::get_progress() const {
	return f_progress;
}

bool GDDragonBones::is_playing() const {
	return b_inited && b_playing && p_armature->getAnimation()->isPlaying();
}

void GDDragonBones::set_current_animation(const godot::String &p_anim) {
	if (str_curr_anim == p_anim) {
		return;
	}

	str_curr_anim = p_anim;
	if (b_inited) {
		if (str_curr_anim == SNAME("[none]")) {
			stop();
		} else if (has_anim(str_curr_anim)) {
			if (b_playing || b_try_playing) {
				play();
			} else {
				p_armature->getAnimation()->gotoAndStopByProgress(get_curr_anim_c_str());
			}
		}
	}
}

String GDDragonBones::get_current_animation() const {
	if (!b_inited || !p_armature->getAnimation())
		return String("");
	return String(p_armature->getAnimation()->getLastAnimationName().c_str());
}

// void GDDragonBones::_set_process(bool _b_process, bool _b_force) {
// 	if (b_processing == _b_process && !_b_force)
// 		return;

// 	// set_process(m_anim_mode == ANIMATION_CALLBACK_MODE_PROCESS_IDLE && _b_process && b_active);
// 	// set_physics_process(m_anim_mode == ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS && _b_process && b_active);

// 	// b_processing = _b_process;
// }

void GDDragonBones::set_texture(const Ref<Texture2D> &_p_texture) {
	if (_p_texture.is_valid() && m_texture_atlas.is_valid() && (_p_texture == m_texture_atlas || m_texture_atlas->get_height() != _p_texture->get_height() || m_texture_atlas->get_width() != _p_texture->get_width()))
		return;

	m_texture_atlas = _p_texture;

	// #ifdef DEBUG_ENABLED
	// 	if (m_texture_atlas.is_valid()) {
	// 		m_texture_atlas->set_flags(m_texture_atlas->get_flags()); //remove repeat from texture, it looks bad in sprites
	// 		//        m_texture_atlas->connect(CoreStringNames::get_singleton()->changed, this, SceneStringNames::get_singleton()->update);
	// 	}
	// #endif

	if (p_armature) {
		p_armature->update_texture_atlas(m_texture_atlas);
		queue_redraw();
	}
}

Ref<Texture2D> GDDragonBones::get_texture() const {
	return m_texture_atlas;
}

bool GDDragonBones::_set(const StringName &p_name, const Variant &p_property) {
	if (p_name == SNAME("playback/current_animation")) {
		if (str_curr_anim == p_property) {
			return false;
		}
		set_current_animation(p_property);
		return true;
	} else if (p_name == SNAME("playback/loop")) {
		set_loop(p_property);
		return true;
	} else if (p_name == SNAME("playback/progress")) {
		seek(p_property);
		return true;
	}
	return false;
}

bool GDDragonBones::_get(const StringName &p_name, Variant &r_property) const {
	if (p_name == SNAME("playback/current_animation")) {
		r_property = str_curr_anim;
		return true;
	} else if (p_name == SNAME("playback/loop")) {
		r_property = c_loop;
		return true;
	} else if (p_name == SNAME("playback/progress")) {
		r_property = get_progress();
		return true;
	}
	return false;
}

void GDDragonBones::_get_property_list(List<PropertyInfo> *_p_list) const {
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

	_p_list->push_back(PropertyInfo(Variant::STRING, "playback/current_animation", PROPERTY_HINT_ENUM, __str_hint));
	_p_list->push_back(PropertyInfo(Variant::INT, "playback/loop", PROPERTY_HINT_RANGE, "-1,100,1"));
	_p_list->push_back(PropertyInfo(Variant::FLOAT, "playback/progress", PROPERTY_HINT_RANGE, "-100,100,0.010"));
}

void GDDragonBones::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &GDDragonBones::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &GDDragonBones::get_texture);

	ClassDB::bind_method(D_METHOD("set_resource", "dragonbones"), &GDDragonBones::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &GDDragonBones::get_resource);

	ClassDB::bind_method(D_METHOD("set_inherit_material"), &GDDragonBones::set_inherit_material);
	ClassDB::bind_method(D_METHOD("is_material_inherited"), &GDDragonBones::is_material_inherited);

	ClassDB::bind_method(D_METHOD("fade_in", "animation_name", "time", "loop", "layer", "group", "fade_out_mode"), &GDDragonBones::fade_in);
	ClassDB::bind_method(D_METHOD("fade_out", "animation_name"), &GDDragonBones::fade_out);

	ClassDB::bind_method(D_METHOD("stop"), &GDDragonBones::stop);
	ClassDB::bind_method(D_METHOD("stop_all"), &GDDragonBones::stop_all);
	ClassDB::bind_method(D_METHOD("reset"), &GDDragonBones::_reset);
	ClassDB::bind_method(D_METHOD("play"), &GDDragonBones::play);
	ClassDB::bind_method(D_METHOD("play_from_time"), &GDDragonBones::play_from_time);
	ClassDB::bind_method(D_METHOD("play_from_progress"), &GDDragonBones::play_from_progress);

	ClassDB::bind_method(D_METHOD("has", "name"), &GDDragonBones::has_anim);
	ClassDB::bind_method(D_METHOD("is_playing"), &GDDragonBones::is_playing);

	ClassDB::bind_method(D_METHOD("set_current_animation", "anim_name"), &GDDragonBones::set_current_animation);
	ClassDB::bind_method(D_METHOD("get_current_animation"), &GDDragonBones::get_current_animation);

	ClassDB::bind_method(D_METHOD("seek", "pos"), &GDDragonBones::seek);
	ClassDB::bind_method(D_METHOD("tell"), &GDDragonBones::tell);
	ClassDB::bind_method(D_METHOD("get_progress"), &GDDragonBones::get_progress);

	ClassDB::bind_method(D_METHOD("set_active", "active"), &GDDragonBones::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &GDDragonBones::is_active);

	ClassDB::bind_method(D_METHOD("set_debug", "debug"), &GDDragonBones::set_debug);
	ClassDB::bind_method(D_METHOD("is_debug"), &GDDragonBones::is_debug);

	ClassDB::bind_method(D_METHOD("flip_x", "enable_flip"), &GDDragonBones::flip_x);
	ClassDB::bind_method(D_METHOD("is_fliped_x"), &GDDragonBones::is_fliped_x);
	ClassDB::bind_method(D_METHOD("flip_y", "enable_flip"), &GDDragonBones::flip_y);
	ClassDB::bind_method(D_METHOD("is_fliped_y"), &GDDragonBones::is_fliped_y);

	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &GDDragonBones::set_speed);
	ClassDB::bind_method(D_METHOD("get_speed"), &GDDragonBones::get_speed);

	ClassDB::bind_method(D_METHOD("set_animation_process_mode", "mode"), &GDDragonBones::set_animation_process_mode);
	ClassDB::bind_method(D_METHOD("get_animation_process_mode"), &GDDragonBones::get_animation_process_mode);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug"), "set_debug", "is_debug");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flipX"), "flip_x", "is_fliped_x");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flipY"), "flip_y", "is_fliped_y");

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "GDDragonBonesResource"), "set_resource", "get_resource");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "playback/process_mode", PROPERTY_HINT_ENUM, "Physics,Idle,Manual"), "set_animation_process_mode", "get_animation_process_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "playback/speed", PROPERTY_HINT_RANGE, "0,10,0.01"), "set_speed", "get_speed");
	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "playback/progress", PROPERTY_HINT_RANGE, "-100,100,0.010"), "seek", "get_progress");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "playback/play"), "play", "is_playing");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "childs use this material"), "set_inherit_material", "is_material_inherited");

	ADD_SIGNAL(MethodInfo("animation_start", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("animation_completed", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("animation_event", PropertyInfo(Variant::STRING, "anim"), PropertyInfo(Variant::STRING, "event")));
	ADD_SIGNAL(MethodInfo("animation_loop_completed", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("animation_snd_event", PropertyInfo(Variant::STRING, "anim"), PropertyInfo(Variant::STRING, "event")));
	ADD_SIGNAL(MethodInfo("fade_in", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("fade_in_completed", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("fade_out", PropertyInfo(Variant::STRING, "anim")));
	ADD_SIGNAL(MethodInfo("fade_out_completed", PropertyInfo(Variant::STRING, "anim")));

	BIND_ENUM_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS);
	BIND_ENUM_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_IDLE);
	BIND_ENUM_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_MANUAL);

	BIND_ENUM_CONSTANT(FADE_OUT_NONE);
	BIND_ENUM_CONSTANT(FADE_OUT_SAME_LAYER);
	BIND_ENUM_CONSTANT(FADE_OUT_SAME_GROUP);
	BIND_ENUM_CONSTANT(FADE_OUT_SAME_LAYER_AND_GROUP);
	BIND_ENUM_CONSTANT(FADE_OUT_ALL);
	BIND_ENUM_CONSTANT(FADE_OUT_SINGLE);
}
