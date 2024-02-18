#include "GDFactory.h"

#include "GDArmatureDisplay.h"
#include "GDMesh.h"
#include "GDSlot.h"
#include "GDTextureAtlasData.h"
#include "GDTextureData.h"
#include "dragonBones/DragonBonesHeaders.h"

using namespace godot;
using namespace dragonBones;

GDFactory::GDFactory(GDOwnerNode *_p_owner) {
	p_owner = _p_owner;
	_dragonBonesInstance = memnew(DragonBones(this));
	_dragonBones = _dragonBonesInstance;
}

GDFactory::~GDFactory() {
	clear();

	if (_dragonBonesInstance) {
		memdelete(_dragonBonesInstance);
		_dragonBonesInstance = nullptr;
	}
}

DragonBonesData *GDFactory::loadDragonBonesData(const char *_p_data_loaded, const std::string &name) {
	if (!name.empty()) {
		const auto existedData = getDragonBonesData(name);

		if (existedData)
			return existedData;
	}
	return parseDragonBonesData(_p_data_loaded, name, 1.0f);
}

TextureAtlasData *GDFactory::loadTextureAtlasData(const char *_p_data_loaded, Ref<Texture> *_p_atlasTexture, const std::string &name, float scale) {
	return static_cast<GDTextureAtlasData *>(BaseFactory::parseTextureAtlasData(_p_data_loaded, _p_atlasTexture, name, scale));
}

GDArmatureDisplay *GDFactory::buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName, const std::string &skinName, const std::string &textureAtlasName) const {
	const auto armature = buildArmature(armatureName, dragonBonesName, skinName, textureAtlasName);

	if (armature != nullptr) {
		_dragonBones->getClock()->add(armature);
		return static_cast<GDArmatureDisplay *>(armature->getDisplay());
	}
	return nullptr;
}

void GDFactory::update(float lastUpdate) {
	_dragonBonesInstance->advanceTime(lastUpdate);
}

void GDFactory::set_speed(float _f_speed) {
	_dragonBonesInstance->getClock()->timeScale = _f_speed;
}

TextureAtlasData *GDFactory::_buildTextureAtlasData(TextureAtlasData *textureAtlasData, void *textureAtlas) const {
	auto textureAtlasData_ = static_cast<GDTextureAtlasData *>(textureAtlasData);

	if (textureAtlasData != nullptr)
		textureAtlasData_->setRenderTexture();
	else
		textureAtlasData_ = BaseObject::borrowObject<GDTextureAtlasData>();
	return textureAtlasData_;
}

Armature *GDFactory::_buildArmature(const BuildArmaturePackage &dataPackage) const {
	const auto armature = BaseObject::borrowObject<Armature>();
	const auto armatureDisplay = GDArmatureDisplay::create();
	armature->init(dataPackage.armature, armatureDisplay, armatureDisplay, _dragonBones);
	armatureDisplay->set_name(armature->getName().c_str());
	return armature;
}

Slot *GDFactory::_buildSlot(const BuildArmaturePackage &dataPackage, const SlotData *slotData, Armature *armature) const {
	auto slot = BaseObject::borrowObject<Slot_GD>();
	auto wrapperDisplay = GDMesh::create();
	_wrapperSlots.push_back(std::unique_ptr<Slot_GD>(slot));

	slot->init(slotData, armature, wrapperDisplay, wrapperDisplay);
	slot->update(0);

	GDSlot *tree_slot = memnew(GDSlot);
	tree_slot->set_slot(slot);

	const auto proxy = static_cast<GDArmatureDisplay *>(slot->getArmature()->getDisplay());
	proxy->add_slot(slot->getName(), tree_slot);

	return slot;
}

void GDFactory::dispatchDBEvent(const std::string &type, EventObject *value) {
	p_owner->dispatch_snd_event(String(type.c_str()), value);
}

Armature *GDFactory::_buildChildArmature(const BuildArmaturePackage *dataPackage, Slot *slot, DisplayData *displayData) const {
	auto childDisplayName = slot->_slotData->name;

	const auto proxy = static_cast<GDArmatureDisplay *>(slot->getArmature()->getDisplay());

	GDArmatureDisplay *childArmature = nullptr;

	if (dataPackage != nullptr) {
		childArmature = buildArmatureDisplay(displayData->path, dataPackage->dataName);
	} else {
		childArmature = buildArmatureDisplay(displayData->path, displayData->getParent()->parent->parent->name);
	}

	if (childArmature == nullptr) {
		ERR_PRINT("Child armature is null");
		return nullptr;
	}

	childArmature->set_z_index(slot->_zOrder);
	childArmature->getArmature()->setFlipY(true);
	childArmature->hide();
	proxy->add_child(childArmature);

	return childArmature->getArmature();
}

void GDFactory::_buildBones(const BuildArmaturePackage &dataPackage, Armature *armature) const {
	for (const auto boneData : dataPackage.armature->sortedBones) {
		const auto bone = BaseObject::borrowObject<Bone>();
		bone->init(boneData, armature);

		GDBone2D *new_bone = GDBone2D::create();
		new_bone->set_data(bone);
		new_bone->set_name(bone->getName().c_str());
		GDArmatureDisplay *display = static_cast<GDArmatureDisplay *>(armature->getDisplay());
		display->add_bone(bone->getName(), new_bone);
	}

	for (const auto &pair : dataPackage.armature->constraints) {
		// TODO more constraint type.
		const auto constraint = BaseObject::borrowObject<IKConstraint>();
		constraint->init(pair.second, armature);
		armature->_addConstraint(constraint);
	}
}

void GDFactory::clear(bool disposeData) {
	_wrapperSlots.clear();

	BaseFactory::clear();
}
