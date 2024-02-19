#include "DragonBonesFactory.h"

#include "DragonBonesArmature.h"
#include "GDMesh.h"
#include "GDSlot.h"
#include "GDTextureAtlasData.h"
#include "dragonBones/DragonBonesHeaders.h"

using namespace godot;
using namespace dragonBones;

DragonBonesFactory *DragonBonesFactory::singleton = ::new DragonBonesFactory; //{ nullptr };

DragonBonesFactory::DragonBonesFactory() {
	ERR_FAIL_COND_MSG(singleton, "Reinstantiate DragonBonesFactory");
	singleton = this;
}

DragonBonesFactory::~DragonBonesFactory() {
	clear();
	if (singleton == this) {
		singleton = nullptr;
	}
}

DragonBonesData *DragonBonesFactory::loadDragonBonesData(const char *_p_data_loaded, const std::string &name) {
	if (!name.empty()) {
		const auto existedData = getDragonBonesData(name);

		if (existedData)
			return existedData;
	}
	return parseDragonBonesData(_p_data_loaded, name, 1.0f);
}

TextureAtlasData *DragonBonesFactory::loadTextureAtlasData(const char *_p_data_loaded, Ref<Texture> *_p_atlasTexture, const std::string &name, float scale) {
	return BaseFactory::parseTextureAtlasData(_p_data_loaded, _p_atlasTexture, name, scale);
}

DragonBonesArmature *DragonBonesFactory::buildArmatureDisplay(const std::string &armatureName, const std::string &dragonBonesName, const std::string &skinName, const std::string &textureAtlasName) const {
	const auto armature = buildArmature(armatureName, dragonBonesName, skinName, textureAtlasName);

	if (armature != nullptr) {
		_dragonBones->getClock()->add(armature);
		return static_cast<DragonBonesArmature *>(armature->getDisplay());
	}
	return nullptr;
}

TextureAtlasData *DragonBonesFactory::_buildTextureAtlasData(TextureAtlasData *textureAtlasData, void *textureAtlas) const {
	auto textureAtlasData_ = static_cast<GDTextureAtlasData *>(textureAtlasData);

	if (textureAtlasData != nullptr)
		textureAtlasData_->setRenderTexture();
	else
		textureAtlasData_ = BaseObject::borrowObject<GDTextureAtlasData>();
	return textureAtlasData_;
}

Armature *DragonBonesFactory::_buildArmature(const BuildArmaturePackage &dataPackage) const {
	const auto armature = BaseObject::borrowObject<Armature>();
	const auto armatureDisplay = memnew(DragonBonesArmature);
	armature->init(dataPackage.armature, armatureDisplay, armatureDisplay, _dragonBones);
	armatureDisplay->set_name(armature->getName().c_str());
	return armature;
}

Slot *DragonBonesFactory::_buildSlot(const BuildArmaturePackage &dataPackage, const SlotData *slotData, Armature *armature) const {
	auto slot = BaseObject::borrowObject<Slot_GD>();
	auto wrapperDisplay = memnew(GDMesh);

	slot->init(slotData, armature, wrapperDisplay, wrapperDisplay);
	slot->update(0);

	GDSlot *tree_slot = memnew(GDSlot(slot));
	// tree_slot->set_slot(slot);

	const auto proxy = static_cast<DragonBonesArmature *>(slot->getArmature()->getDisplay());
	proxy->add_slot(slot->getName(), tree_slot);

	return slot;
}

Armature *DragonBonesFactory::_buildChildArmature(const BuildArmaturePackage *dataPackage, Slot *slot, DisplayData *displayData) const {
	auto childDisplayName = slot->_slotData->name;

	const auto proxy = static_cast<DragonBonesArmature *>(slot->getArmature()->getDisplay());

	DragonBonesArmature *childArmature = nullptr;

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

void DragonBonesFactory::_buildBones(const BuildArmaturePackage &dataPackage, Armature *armature) const {
	for (const auto boneData : dataPackage.armature->sortedBones) {
		const auto bone = BaseObject::borrowObject<Bone>();
		bone->init(boneData, armature);

		GDBone2D *new_bone = memnew(GDBone2D(bone));
		new_bone->set_name(bone->getName().c_str());
		DragonBonesArmature *display = static_cast<DragonBonesArmature *>(armature->getDisplay());
		display->add_bone(bone->getName(), new_bone);
	}

	for (const auto &pair : dataPackage.armature->constraints) {
		// TODO more constraint type.
		const auto constraint = BaseObject::borrowObject<IKConstraint>();
		constraint->init(pair.second, armature);
		armature->_addConstraint(constraint);
	}
}

void DragonBonesFactory::clear(bool disposeData) {
	BaseFactory::clear(disposeData);
}
