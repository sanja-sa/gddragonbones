#ifndef GDARMATUREDESPLAY_H
#define GDARMATUREDESPLAY_H

#include <dragonBones\DragonBonesHeaders.h>
#include "GDDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

class GDArmatureDisplay : public GDDisplay, virtual public IArmatureProxy
{
   OBJ_TYPE(GDArmatureDisplay, GDDisplay);

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

    void addEvent(const std::string& type, const std::function<void(EventObject*)>& callback);
    void removeEvent(const std::string& type);

	bool hasDBEventListener(const std::string& type) const override { return true; }
    void addDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener) {}
    void removeDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener) {}
    void dispatchDBEvent(const std::string& type, EventObject* value);

	void dbInit(Armature* armature) override;
	void dbClear() override;
	void dbUpdate() override;

	void dispose(bool disposeProxy) override;
	
	Armature* getArmature() const override { return p_armature; }
	Animation* getAnimation() const override { return p_armature->getAnimation(); }

    void    add_parent_class(bool _b_debug, const Ref<Texture>& _m_texture_atla);
    void    update_child_colors();
    void    update_child_blends();
    void    update_texture_atlas(const Ref<Texture> &_m_texture_atlas);
    void    set_debug(bool _b_debug);

};

DRAGONBONES_NAMESPACE_END

#endif // GDARMATUREDESPLAY_H
