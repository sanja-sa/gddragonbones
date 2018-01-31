#ifndef 	GDISPLAY_H
#define     GDISPLAY_H

#include "scene/resources/texture.h"
#include "servers/visual_server.h"
#include "scene/2d/node_2d.h"
#include <version_generated.gen.h>

DRAGONBONES_USING_NAME_SPACE;

class GDOwnerNode : public Node2D
{
#if (VERSION_MAJOR == 3)
    GDCLASS(GDOwnerNode, Node2D);
#else
    OBJ_TYPE(GDOwnerNode, Node2D);
#endif

public:
    Color		    modulate;

public:
    GDOwnerNode()  { modulate = Color(1,1,1,1); }
    virtual ~GDOwnerNode() {}

    virtual void set_modulate(const Color& _col)
    {
        modulate = _col;
    }

    virtual void dispatch_event(const String& _str_type, const EventObject* _p_value) = 0;
    virtual void dispatch_snd_event(const String& _str_type, const EventObject* _p_value) = 0;
};

class GDDisplay : public Node2D
{

#if (VERSION_MAJOR == 3)
    GDCLASS(GDDisplay, Node2D);
#else
    OBJ_TYPE(GDDisplay, Node2D);
#endif

private:
    GDDisplay(const GDDisplay&);

public:
    Ref<Texture>    texture;
#if (VERSION_MAJOR == 3)
    CanvasItemMaterial* p_canvas_mat;
#endif

	Color		    modulate;
    GDOwnerNode*    p_owner;
    bool            b_debug;

public:
    GDDisplay()  {
                   modulate = Color(1,1,1,1);
                   p_owner = nullptr;
                   b_debug = false;
#if (VERSION_MAJOR == 3)
                   p_canvas_mat = memnew(CanvasItemMaterial);
#endif
                 }
    virtual ~GDDisplay() {}

    virtual void set_modulate(const Color& _col)
	{
		modulate = _col;
	}


#if (VERSION_MAJOR == 3)
    void    set_blend_mode(CanvasItem::BlendMode _blend)
    {
        p_canvas_mat->set_blend_mode((CanvasItemMaterial::BlendMode)_blend);
        Ref<CanvasItemMaterial> __mat = Ref<CanvasItemMaterial>(p_canvas_mat);
        set_material(__mat);
    }
#endif
};

#endif // GDISPLAY_H
