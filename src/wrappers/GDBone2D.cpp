#include "GDBone2D.h"

using namespace godot;

void GDBone2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_bone_name"), &GDBone2D::get_bone_name);
	ClassDB::bind_method(D_METHOD("get_bone_position"), &GDBone2D::get_bone_position);
	ClassDB::bind_method(D_METHOD("set_bone_position", "new_position"), &GDBone2D::set_bone_position);
	ClassDB::bind_method(D_METHOD("set_bone_global_position", "new_position"), &GDBone2D::set_bone_global_position);
	ClassDB::bind_method(D_METHOD("get_bone_scale"), &GDBone2D::get_bone_scale);
	ClassDB::bind_method(D_METHOD("set_bone_scale", "new_scale"), &GDBone2D::set_bone_scale);
	ClassDB::bind_method(D_METHOD("set_bone_global_scale", "new_scale"), &GDBone2D::set_bone_global_scale);
	ClassDB::bind_method(D_METHOD("get_bone_rotation"), &GDBone2D::get_bone_rotation);
	ClassDB::bind_method(D_METHOD("set_bone_rotation", "deg_in_rad"), &GDBone2D::set_bone_rotation);
	ClassDB::bind_method(D_METHOD("set_bone_global_rotation", "deg_in_rad"), &GDBone2D::set_bone_global_rotation);
	ClassDB::bind_method(D_METHOD("get_bone_global_position"), &GDBone2D::get_bone_global_position);
	ClassDB::bind_method(D_METHOD("get_bone_global_rotation"), &GDBone2D::get_bone_global_rotation);
	ClassDB::bind_method(D_METHOD("get_bone_global_scale"), &GDBone2D::get_bone_global_scale);
	ClassDB::bind_method(D_METHOD("get_bone_offset_position"), &GDBone2D::get_bone_offset_position);
	ClassDB::bind_method(D_METHOD("get_bone_animation_position"), &GDBone2D::get_bone_animation_position);
	ClassDB::bind_method(D_METHOD("get_bone_origin_position"), &GDBone2D::get_bone_origin_position);
	ClassDB::bind_method(D_METHOD("has_data"), &GDBone2D::has_data);
}

Vector2 GDBone2D::get_bone_global_position() {
	return get_bone_position() +
			static_cast<Node2D *>(get_parent())->get_global_transform().get_origin() +
			get_bone_origin_position();
}

Vector2 GDBone2D::get_bone_global_scale() {
	return get_global_transform().get_scale();
}

float GDBone2D::get_bone_global_rotation() {
	return boneData->global.rotation;
}

void GDBone2D::set_bone_global_position(Vector2 new_pos) {
	set_bone_position(
			new_pos -
			static_cast<Node2D *>(get_parent())->get_global_transform().get_origin() -
			get_bone_origin_position());
}

void GDBone2D::set_bone_global_scale(Vector2 scale) {
	boneData->global.scaleX = scale.x;
	boneData->global.scaleY = scale.y;

	boneData->invalidUpdate();
}

void GDBone2D::set_bone_global_rotation(float rotation) {
	boneData->global.rotation = rotation;
	boneData->update(0);
	boneData->invalidUpdate();
}

Vector2 GDBone2D::get_bone_origin_position() {
	float _x = boneData->origin->x * get_global_transform().get_scale().x;
	float _y = boneData->origin->y * get_global_transform().get_scale().y;

	return Vector2(_x, _y);
}

Vector2 GDBone2D::get_bone_animation_position() {
	float _x = boneData->animationPose.x;
	float _y = boneData->animationPose.y;

	return Vector2(_x, _y);
}

Vector2 GDBone2D::get_bone_offset_position() {
	float _x = boneData->offset.x;
	float _y = boneData->offset.y;

	return Vector2(_x, _y);
}

void GDBone2D::set_data(dragonBones::Bone *new_bone) {
	this->boneData = new_bone;
}

String GDBone2D::get_bone_name() {
	if (boneData == nullptr)
		return "";
	return String(boneData->getName().c_str());
}

Vector2 GDBone2D::get_bone_position() {
	if (boneData == nullptr)
		return Vector2();

	return Vector2(boneData->animationPose.x * get_global_transform().get_scale().x,
			boneData->animationPose.y * get_global_transform().get_scale().y);
}

void GDBone2D::set_bone_position(Vector2 new_pos) {
	if (boneData == nullptr)
		return;

	boneData->animationPose.x = new_pos.x / get_global_transform().get_scale().x;
	boneData->animationPose.y = new_pos.y / get_global_transform().get_scale().y;

	boneData->invalidUpdate();
}

Vector2 GDBone2D::get_bone_scale() {
	if (boneData == nullptr)
		return Vector2();

	return Vector2(boneData->animationPose.scaleX, boneData->animationPose.scaleY);
}

void GDBone2D::set_bone_scale(Vector2 scale) {
	if (boneData == nullptr)
		return;

	boneData->animationPose.scaleX = scale.x;
	boneData->animationPose.scaleY = scale.y;

	boneData->invalidUpdate();
}

float GDBone2D::get_bone_rotation() {
	if (boneData == nullptr)
		return 0.;

	return boneData->animationPose.rotation;
}

void GDBone2D::set_bone_rotation(float rotation) {
	if (boneData == nullptr)
		return;

	boneData->animationPose.rotation = rotation;
	boneData->update(0);
	boneData->invalidUpdate();
}

bool GDBone2D::has_data() {
	return this->boneData != nullptr;
}
