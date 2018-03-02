#ifndef GDARMATUREDESPLAY_H
#define GDARMATUREDESPLAY_H

#include <dragonBones/DragonBonesHeaders.h>
#include "GDDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

class GDArmatureDisplay : public GDDisplay, virtual public IArmatureProxy
{
#if (VERSION_MAJOR == 3)
    GDCLASS(GDArmatureDisplay, GDDisplay);
#else
    OBJ_TYPE(GDArmatureDisplay, GDDisplay);
#endif

   private:
       GDArmatureDisplay(const GDArmatureDisplay&);

protected:
    Armature*           p_armature;

public:
	GDArmatureDisplay();
	~GDArmatureDisplay();

    static GDArmatureDisplay* create()
    {
        return memnew(GDArmatureDisplay);
    }

    void addEvent(const std::string& _type, const std::function<void(EventObject*)>& _callback);
    void removeEvent(const std::string& _type);

    bool hasDBEventListener(const std::string& _type) const override { return true; }
    void addDBEventListener(const std::string& _type, const std::function<void(EventObject*)>& _listener) {}
    void removeDBEventListener(const std::string& _type, const std::function<void(EventObject*)>& _listener) {}
    void dispatchDBEvent(const std::string& _type, EventObject* _value);

    void dbInit(Armature* _p_armature) override;
	void dbClear() override;
	void dbUpdate() override;

	void dispose(bool disposeProxy) override;
	
	Armature* getArmature() const override { return p_armature; }
	Animation* getAnimation() const override { return p_armature->getAnimation(); }

    void    add_parent_class(bool _b_debug, const Ref<Texture>& _m_texture_atla);
    void    update_childs(bool _b_color, bool _b_blending = false);
    void    update_texture_atlas(const Ref<Texture> &_m_texture_atlas);
    void    update_material_inheritance(bool _b_inherit_material);

    void    set_debug(bool _b_debug);
};

DRAGONBONES_NAMESPACE_END

#endif // GDARMATUREDESPLAY_H
