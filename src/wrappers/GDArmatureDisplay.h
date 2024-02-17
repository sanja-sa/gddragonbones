#pragma once

#include "GDDisplay.h"
#include "GDTextureAtlasData.h"
#include <dragonBones/DragonBonesHeaders.h>

DRAGONBONES_NAMESPACE_BEGIN

class GDArmatureDisplay : public GDDisplay, virtual public IArmatureProxy {
	// GDCLASS(GDArmatureDisplay, GDDisplay);

private:
	GDArmatureDisplay(const GDArmatureDisplay &);

protected:
	Armature *p_armature;

public:
	GDArmatureDisplay();
	~GDArmatureDisplay();

	static GDArmatureDisplay *create() {
		return memnew(GDArmatureDisplay);
	}

	void addEvent(const std::string &_type, const std::function<void(EventObject *)> &_callback);
	void removeEvent(const std::string &_type);

	virtual bool hasDBEventListener(const std::string &_type) const override { return true; }
	virtual void addDBEventListener(const std::string &_type, const std::function<void(EventObject *)> &_listener) override {}
	virtual void removeDBEventListener(const std::string &_type, const std::function<void(EventObject *)> &_listener) override {}
	virtual void dispatchDBEvent(const std::string &_type, EventObject *_value) override;

	virtual void dbInit(Armature *_p_armature) override;
	virtual void dbClear() override;
	virtual void dbUpdate() override;

	virtual void dispose(bool disposeProxy) override;

	virtual Armature *getArmature() const override { return p_armature; }
	virtual Animation *getAnimation() const override { return p_armature->getAnimation(); }

	void add_parent_class(bool _b_debug, const godot::Ref<godot::Texture> &_m_texture_atla);
	void update_childs(bool _b_color, bool _b_blending = false);
	void update_texture_atlas(const godot::Ref<godot::Texture> &_m_texture_atlas);
	void update_material_inheritance(bool _b_inherit_material);

	void set_debug(bool _b_debug);
};

DRAGONBONES_NAMESPACE_END
