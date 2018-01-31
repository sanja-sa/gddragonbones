#include "GDArmatureDisplay.h"

#include "GDDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

GDArmatureDisplay::GDArmatureDisplay()
{
	p_armature = nullptr;
    set_use_parent_material(true);
}

GDArmatureDisplay::~GDArmatureDisplay()
{
	p_armature = nullptr;
}

void GDArmatureDisplay::dbInit(Armature* _p_armature)
{
    p_armature = _p_armature;
}

void GDArmatureDisplay::dbClear()
{
	p_armature = nullptr;
}

void GDArmatureDisplay::dbUpdate()
{
}

void GDArmatureDisplay::dispatchDBEvent(const std::string& _type, EventObject* _value)
{
    if(p_owner)
        p_owner->dispatch_event(String(_type.c_str()), _value);
}

void GDArmatureDisplay::dispose(bool _disposeProxy)
{
	if (p_armature)
	{
		delete p_armature;
		p_armature = nullptr;
	}
}

void GDArmatureDisplay::add_parent_class(bool _b_debug, const Ref<Texture>& _m_texture_atlas)
{
    if(!p_armature)
        return;
    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;

        auto display = item->getRawDisplay();
        if (!display) continue;
        add_child(static_cast<GDDisplay*>(display));
        static_cast<GDDisplay*>(display)->p_owner = p_owner;
        static_cast<GDDisplay*>(display)->b_debug = _b_debug;
        static_cast<GDDisplay*>(display)->texture = _m_texture_atlas;
    }
}

void GDArmatureDisplay::update_childs(bool _b_color, bool _b_blending)
{
    if(!p_armature)
        return;

    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;

        if(_b_color)
            item->_colorDirty = true;

        if(_b_blending)
            item->invalidUpdate();

        item->update(0);
    }
}

void GDArmatureDisplay::update_material_inheritance(bool _b_inherit_material)
{
    if(!p_armature)
        return;

    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;
        auto display = item->getRawDisplay();
        if (!display) continue;

        static_cast<GDDisplay*>(display)->set_use_parent_material(_b_inherit_material);
    }
}


void GDArmatureDisplay::update_texture_atlas(const Ref<Texture>& _m_texture_atlas)
{
    if(!p_armature)
        return;

    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;
        auto display = item->getRawDisplay();
        if (!display) continue;

        static_cast<GDDisplay*>(display)->texture = _m_texture_atlas;
        static_cast<GDDisplay*>(display)->update();
    }
}

void  GDArmatureDisplay::set_debug(bool _b_debug)
{
    if(!p_armature)
        return;
    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;

        auto display = item->getRawDisplay();
        if (!display) continue;
        static_cast<GDDisplay*>(display)->b_debug = _b_debug;
        static_cast<GDDisplay*>(display)->update();
    }
}

DRAGONBONES_NAMESPACE_END
