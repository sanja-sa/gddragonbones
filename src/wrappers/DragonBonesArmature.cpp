#include "DragonBonesArmature.h"

#include "GDDisplay.h"
#include "GDMesh.h"
#include "dragonBones/DragonBonesHeaders.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace dragonBones;

DragonBonesArmature::DragonBonesArmature() {
	set_use_parent_material(true);
}

DragonBonesArmature::~DragonBonesArmature() {
	_bones.clear();
	_slots.clear();
	dispose(true);
}

void DragonBonesArmature::_bind_methods() {
	// TODO:: 属性
	ClassDB::bind_method(D_METHOD("has_animation", "animation_name"), &DragonBonesArmature::has_animation);
	ClassDB::bind_method(D_METHOD("get_animations"), &DragonBonesArmature::get_animations);

	ClassDB::bind_method(D_METHOD("is_playing"), &DragonBonesArmature::is_playing);
	ClassDB::bind_method(D_METHOD("play", "animation_name", "loop_count"), &DragonBonesArmature::play);

	ClassDB::bind_method(D_METHOD("play_from_time", "animation_name", "f_time", "loop_count"), &DragonBonesArmature::play_from_time);
	ClassDB::bind_method(D_METHOD("play_from_progress", "animation_name", "f_progress", "loop_count"), &DragonBonesArmature::play_from_progress);

	ClassDB::bind_method(D_METHOD("stop", "animation_name", "b_reset"), &DragonBonesArmature::stop);
	ClassDB::bind_method(D_METHOD("stop_all_animations", "b_reset"), &DragonBonesArmature::stop_all_animations);

	ClassDB::bind_method(D_METHOD("fade_in"), &DragonBonesArmature::fade_in);

	ClassDB::bind_method(D_METHOD("has_slot", "slot_name"), &DragonBonesArmature::has_slot);
	ClassDB::bind_method(D_METHOD("get_slot", "slot_name"), &DragonBonesArmature::get_slot);
	ClassDB::bind_method(D_METHOD("get_slots"), &DragonBonesArmature::get_slots);

	ClassDB::bind_method(D_METHOD("reset", "recurisively"), &DragonBonesArmature::reset);

	ClassDB::bind_method(D_METHOD("set_flip_x", "is_flipped"), &DragonBonesArmature::flip_x);
	ClassDB::bind_method(D_METHOD("is_flipped_x"), &DragonBonesArmature::is_flipped_x);

	ClassDB::bind_method(D_METHOD("set_flip_y", "is_flipped"), &DragonBonesArmature::flip_y);
	ClassDB::bind_method(D_METHOD("is_flipped_y"), &DragonBonesArmature::is_flipped_y);

	ClassDB::bind_method(D_METHOD("set_debug", "debug"), &DragonBonesArmature::set_debug);
	ClassDB::bind_method(D_METHOD("is_debug"), &DragonBonesArmature::is_debug);

	ClassDB::bind_method(D_METHOD("get_ik_constraints"), &DragonBonesArmature::get_ik_constraints);
	ClassDB::bind_method(D_METHOD("set_ik_constraint", "constraint_name", "new_position"), &DragonBonesArmature::set_ik_constraint);
	ClassDB::bind_method(D_METHOD("set_ik_constraint_bend_positive", "constraint_name", "is_positive"), &DragonBonesArmature::set_ik_constraint_bend_positive);

	ClassDB::bind_method(D_METHOD("get_bones"), &DragonBonesArmature::get_bones);
	ClassDB::bind_method(D_METHOD("get_bone", "bone_name"), &DragonBonesArmature::get_bone);

	// Enum
	BIND_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS);
	BIND_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_IDLE);
	BIND_CONSTANT(ANIMATION_CALLBACK_MODE_PROCESS_MANUAL);

	BIND_CONSTANT(FADE_OUT_NONE);
	BIND_CONSTANT(FADE_OUT_SAME_LAYER);
	BIND_CONSTANT(FADE_OUT_SAME_GROUP);
	BIND_CONSTANT(FADE_OUT_SAME_LAYER_AND_GROUP);
	BIND_CONSTANT(FADE_OUT_ALL);
	BIND_CONSTANT(FADE_OUT_SINGLE);
}

template <typename Func, typename std::enable_if<std::is_invocable_v<Func, DragonBonesArmature *>>::type *_dummy>
void DragonBonesArmature::for_each_armature(Func &&p_action) {
	for (auto slot : getArmature()->getSlots()) {
		if (slot->getDisplayList().size() == 0)
			continue;
		auto display = slot->getDisplayList()[slot->getDisplayIndex()];
		if (display.second == dragonBones::DisplayType::Armature) {
			dragonBones::Armature *armature = static_cast<dragonBones::Armature *>(display.first);
			DragonBonesArmature *convertedDisplay = static_cast<DragonBonesArmature *>(armature->getDisplay());
			if constexpr (std::is_invocable_r_v<bool, Func, DragonBonesArmature *>) {
				if (p_action(convertedDisplay)) {
					break;
				}
			} else {
				p_action(convertedDisplay);
			}
		}
	}
}

void DragonBonesArmature::set_debug(bool _b_debug, bool p_recursively) {
	if (!p_armature)
		return;

	b_debug = _b_debug;
	for (Slot *slot : p_armature->getSlots()) {
		if (!slot)
			continue;

		if (p_recursively) {
			for_each_armature([_b_debug](DragonBonesArmature *p_child_armature) {
				p_child_armature->set_debug(_b_debug, true);
			});
		}

		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			display->b_debug = _b_debug;
			display->queue_redraw();
		}
	}
}

bool DragonBonesArmature::has_animation(const String &_animation_name) {
	if (p_armature == nullptr || !getAnimation()) {
		return false;
	}

	return getArmature()->getArmatureData()->getAnimation(_animation_name.ascii().get_data()) != nullptr;
}

Array DragonBonesArmature::get_animations() {
	Array animations{};

	const ArmatureData *data = p_armature->getArmatureData();

	for (std::string animation_name : data->getAnimationNames()) {
		animations.push_back(String(animation_name.c_str()));
	}

	return animations;
}

String DragonBonesArmature::get_current_animation() const {
	if (!getAnimation())
		return {};
	return getAnimation()->getLastAnimationName().c_str();
}

String DragonBonesArmature::get_current_animation_on_layer(int _layer) const {
	if (!getAnimation())
		return {};

	std::vector<AnimationState *> states = p_armature->getAnimation()->getStates();

	for (AnimationState *state : states) {
		if (state->layer == _layer) {
			return state->getName().c_str();
		}
	}

	return {};
}

String DragonBonesArmature::get_current_animation_in_group(const String &_group_name) const {
	if (!getAnimation())
		return {};

	std::vector<AnimationState *> states = getAnimation()->getStates();

	for (AnimationState *state : states) {
		if (state->group == _group_name.ascii().get_data()) {
			return state->getName().c_str();
		}
	}

	return {};
}

float DragonBonesArmature::tell_animation(const String &_animation_name) {
	if (has_animation(_animation_name)) {
		AnimationState *animation_state = getAnimation()->getState(_animation_name.ascii().get_data());
		if (animation_state)
			return animation_state->getCurrentTime() / animation_state->_duration;
	}
	return 0.0f;
}

void DragonBonesArmature::seek_animation(const String &_animation_name, float progress) {
	if (has_animation(_animation_name)) {
		stop(_animation_name, true);
		auto current_progress = Math::fmod(progress, 1.0f);
		if (current_progress == 0 && progress != 0)
			current_progress = 1.0f;
		p_armature->getAnimation()->gotoAndStopByProgress(_animation_name.ascii().get_data(), current_progress < 0 ? 1. + current_progress : current_progress);
	}
}

bool DragonBonesArmature::is_playing() const {
	return getAnimation()->isPlaying();
}

void DragonBonesArmature::play(const String &_animation_name, int loop) {
	if (has_animation(_animation_name)) {
		getAnimation()->play(_animation_name.ascii().get_data(), loop);
	}
}

void DragonBonesArmature::play_from_time(const String &_animation_name, float _f_time, int loop) {
	if (has_animation(_animation_name)) {
		play(_animation_name, loop);
		getAnimation()->gotoAndPlayByTime(_animation_name.ascii().get_data(), _f_time);
	}
}

void DragonBonesArmature::play_from_progress(const String &_animation_name, float f_progress, int loop) {
	if (has_animation(_animation_name)) {
		play(_animation_name, loop);
		getAnimation()->gotoAndPlayByProgress(_animation_name.ascii().get_data(), f_progress);
	}
}

void DragonBonesArmature::stop(const String &_animation_name, bool b_reset) {
	if (getAnimation()) {
		getAnimation()->stop(_animation_name.ascii().get_data());

		if (b_reset) {
			reset();
		}
	}
}

void DragonBonesArmature::stop_all_animations(bool b_children, bool b_reset) {
	if (getAnimation()) {
		getAnimation()->stop("");
	}

	if (b_reset) {
		reset();
	}

	if (b_children) {
		for_each_armature([b_reset](DragonBonesArmature *p_child_armature) {
			p_child_armature->stop_all_animations(true, b_reset);
		});
	}
}

void DragonBonesArmature::fade_in(const String &_animation_name, float _time, int _loop, int _layer, const String &_group, DragonBonesArmature::AnimFadeOutMode _fade_out_mode) {
	if (has_animation(_animation_name)) {
		getAnimation()->fadeIn(_animation_name.ascii().get_data(), _time, _loop, _layer, _group.ascii().get_data(), (AnimationFadeOutMode)_fade_out_mode);
	}
}

void DragonBonesArmature::reset(bool p_recursively) {
	if (getAnimation()) {
		getAnimation()->reset();
	}

	if (p_recursively) {
		for_each_armature([](DragonBonesArmature *p_child_armature) {
			p_child_armature->reset(true);
		});
	}
}

bool DragonBonesArmature::has_slot(const String &_slot_name) const {
	return getArmature()->getSlot(_slot_name.ascii().get_data()) != nullptr;
}

Dictionary DragonBonesArmature::get_slots() {
	Dictionary slots{};

	for (auto &slot : _slots) {
		slots[slot.first.c_str()] = slot.second;
	}

	return slots;
}

Ref<DragonBonesSlot> DragonBonesArmature::get_slot(const String &_slot_name) {
	return _slots[_slot_name.ascii().get_data()];
}

void DragonBonesArmature::set_slot_display_index(const String &_slot_name, int _index) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return;
	}

	getSlot(_slot_name)->setDisplayIndex(_index);
}

void DragonBonesArmature::set_slot_by_item_name(const String &_slot_name, const String &_item_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return;
	}

	const std::vector<DisplayData *> *rawData = getSlot(_slot_name)->getRawDisplayDatas();

	// we only want to update the slot if there's a choice
	if (rawData->size() > 1) {
		const char *desired_item = _item_name.ascii().get_data();
		std::string NONE_STRING("none");

		if (NONE_STRING.compare(desired_item) == 0) {
			getSlot(_slot_name)->setDisplayIndex(-1);
		}

		for (int i = 0; i < rawData->size(); i++) {
			DisplayData *display_data = rawData->at(i);

			if (display_data->name.compare(desired_item) == 0) {
				getSlot(_slot_name)->setDisplayIndex(i);
				return;
			}
		}
	} else {
		WARN_PRINT("Slot " + _slot_name + " has only 1 item; refusing to set slot");
		return;
	}

	WARN_PRINT("Slot " + _slot_name + " has no item called \"" + _item_name);
}

void DragonBonesArmature::set_all_slots_by_item_name(const String &_item_name) {
	for (Slot *slot : getArmature()->getSlots()) {
		set_slot_by_item_name(String(slot->getName().c_str()), _item_name);
	}
}

int DragonBonesArmature::get_slot_display_index(const String &_slot_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return -1;
	}
	return p_armature->getSlot(_slot_name.ascii().get_data())->getDisplayIndex();
}

int DragonBonesArmature::get_total_items_in_slot(const String &_slot_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return -1;
	}
	return p_armature->getSlot(_slot_name.ascii().get_data())->getDisplayList().size();
}

void DragonBonesArmature::cycle_next_item_in_slot(const String &_slot_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return;
	}

	int current_slot = get_slot_display_index(_slot_name);
	current_slot++;

	set_slot_display_index(_slot_name, current_slot < get_total_items_in_slot(_slot_name) ? current_slot : -1);
}

void DragonBonesArmature::cycle_previous_item_in_slot(const String &_slot_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return;
	}

	int current_slot = get_slot_display_index(_slot_name);
	current_slot--;

	set_slot_display_index(_slot_name, current_slot >= -1 ? current_slot : get_total_items_in_slot(_slot_name) - 1);
}

Color DragonBonesArmature::get_slot_display_color_multiplier(const String &_slot_name) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return Color(-1, -1, -1, -1);
	}
	ColorTransform transform(p_armature->getSlot(_slot_name.ascii().get_data())->_colorTransform);

	Color return_color;
	return_color.r = transform.redMultiplier;
	return_color.g = transform.greenMultiplier;
	return_color.b = transform.blueMultiplier;
	return_color.a = transform.alphaMultiplier;
	return return_color;
}

void DragonBonesArmature::set_slot_display_color_multiplier(const String &_slot_name, const Color &_color) {
	if (!has_slot(_slot_name)) {
		WARN_PRINT("Slot " + _slot_name + " doesn't exist");
		return;
	}

	ColorTransform _new_color;
	_new_color.redMultiplier = _color.r;
	_new_color.greenMultiplier = _color.g;
	_new_color.blueMultiplier = _color.b;
	_new_color.alphaMultiplier = _color.a;

	p_armature->getSlot(_slot_name.ascii().get_data())->_setColor(_new_color);
}

void DragonBonesArmature::flip_x(bool _b_flip) {
	getArmature()->setFlipX(_b_flip);
	getArmature()->advanceTime(0);
}

bool DragonBonesArmature::is_flipped_x() const {
	return getArmature()->getFlipX();
}

void DragonBonesArmature::flip_y(bool _b_flip) {
	getArmature()->setFlipY(_b_flip);
	getArmature()->advanceTime(0);
}

bool DragonBonesArmature::is_flipped_y() const {
	return getArmature()->getFlipY();
}

Dictionary DragonBonesArmature::get_ik_constraints() {
	Dictionary dict;

	for (auto &constraint : getArmature()->getArmatureData()->constraints) {
		dict[String(constraint.first.c_str())] = Vector2(constraint.second->target->transform.x, constraint.second->target->transform.y);
	}

	return dict;
}

void DragonBonesArmature::set_ik_constraint(const String &name, Vector2 position) {
	for (dragonBones::Constraint *constraint : getArmature()->_constraints) {
		if (constraint->getName() == name.ascii().get_data()) {
			dragonBones::BoneData *target = const_cast<BoneData *>(constraint->_constraintData->target);
			target->transform.x = position.x;
			target->transform.y = position.y;

			constraint->_constraintData->setTarget(target);
			constraint->update();
			getArmature()->invalidUpdate(target->name, true);
		}
	}
}

void DragonBonesArmature::set_ik_constraint_bend_positive(const String &name, bool bend_positive) {
	for (dragonBones::Constraint *constraint : getArmature()->_constraints) {
		if (constraint->getName() == name.ascii().get_data()) {
			dragonBones::BoneData *target = const_cast<BoneData *>(constraint->_constraintData->target);

			static_cast<IKConstraint *>(constraint)->_bendPositive = bend_positive;
			constraint->update();
			getArmature()->invalidUpdate(target->name, true);
		}
	}
}

Dictionary DragonBonesArmature::get_bones() {
	Dictionary bones{};

	for (auto &bone : _bones) {
		bones[bone.first.c_str()] = bone.second;
	}

	return bones;
}

Ref<DragonBonesBone> DragonBonesArmature::get_bone(const String &name) {
	return _bones[name.ascii().get_data()];
}

Slot *DragonBonesArmature::getSlot(const std::string &name) const {
	return p_armature->getSlot(name);
}

void DragonBonesArmature::add_bone(std::string name, const Ref<DragonBonesBone> &new_bone) {
	_bones.insert(std::make_pair(name, new_bone));
}

void DragonBonesArmature::add_slot(std::string name, const Ref<DragonBonesSlot> &new_slot) {
	_slots.insert(std::make_pair(name, new_slot));
}

void DragonBonesArmature::dbInit(Armature *_p_armature) {
	p_armature = _p_armature;
}

void DragonBonesArmature::dbClear() {
	p_armature = nullptr;
}

void DragonBonesArmature::dbUpdate() {
}

void DragonBonesArmature::dispose(bool _disposeProxy) {
	if (p_armature) {
		p_armature->dispose();
		p_armature = nullptr;
	}
}

void DragonBonesArmature::setup_recursively(bool _b_debug, const Ref<Texture> &_m_texture_atlas) {
	if (!p_armature)
		return;

	b_debug = _b_debug;
	for (Slot *slot : p_armature->getSlots()) {
		if (!slot)
			continue;

		for_each_armature([&_m_texture_atlas, this](DragonBonesArmature *p_child_armature) {
			p_child_armature->p_owner = p_owner;
			p_child_armature->setup_recursively(b_debug, _m_texture_atlas);
		});

		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			add_child(display);
			display->p_owner = this;
			display->b_debug = _b_debug;
			display->texture = _m_texture_atlas;
		}
	}
}

void DragonBonesArmature::update_childs(bool _b_color, bool _b_blending) {
	if (!p_armature)
		return;

	for (Slot *slot : p_armature->getSlots()) {
		if (!slot)
			continue;

		if (_b_color)
			slot->_colorDirty = true;

		if (_b_blending)
			slot->invalidUpdate();

		slot->update(0);
	}
}

void DragonBonesArmature::update_material_inheritance(bool _b_inherit_material) {
	if (!p_armature)
		return;

	for (Slot *slot : p_armature->getSlots()) {
		if (!slot)
			continue;

		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			display->set_use_parent_material(_b_inherit_material);
		}
	}
}

void DragonBonesArmature::update_texture_atlas(const Ref<Texture> &_m_texture_atlas) {
	if (!p_armature)
		return;

	for (Slot *slot : p_armature->getSlots()) {
		if (!slot)
			continue;
		if (auto display = static_cast<GDDisplay *>(slot->getRawDisplay())) {
			display->texture = _m_texture_atlas;
			display->queue_redraw();
		}
	}
}
