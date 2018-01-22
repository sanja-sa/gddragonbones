#include "GDArmatureDisplay.h"

#include "GDDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

GDArmatureDisplay::GDArmatureDisplay()
{
	p_armature = nullptr;
}

GDArmatureDisplay::~GDArmatureDisplay()
{
	p_armature = nullptr;
}

void GDArmatureDisplay::dbInit(Armature* armature)
{
	p_armature = armature;
}

void GDArmatureDisplay::dbClear()
{
	p_armature = nullptr;
}

void GDArmatureDisplay::dbUpdate()
{
}

void GDArmatureDisplay::dispatchDBEvent(const std::string& type, EventObject* value)
{
    if(p_owner)
        p_owner->dispatch_event(String(type.c_str()), value);
}

void GDArmatureDisplay::dispose(bool disposeProxy)
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

void GDArmatureDisplay::update_child_colors()
{
    if(!p_armature)
        return;
    auto arr = p_armature->getSlots();
    for (auto item : arr)
    {
        if (!item) continue;
        item->_colorDirty = true;
        item->update(0);
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
