#ifndef 	GDISPLAY_H
#define     GDISPLAY_H

#include "scene/resources/texture.h"
#include "servers/visual_server.h"
#include "scene/2d/node_2d.h"

DRAGONBONES_USING_NAME_SPACE;

class GDOwnerNode : public Node2D
{
    OBJ_TYPE(GDOwnerNode, Node2D);

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
	OBJ_TYPE(GDDisplay, Node2D);

private:
    GDDisplay(const GDDisplay&);

public:
    Ref<Texture>    texture;
	Color		    modulate;
    GDOwnerNode*    p_owner;
    bool            b_debug;

public:
    GDDisplay()  { modulate = Color(1,1,1,1); p_owner = nullptr; b_debug = false; }
    virtual ~GDDisplay() {}

    virtual void set_modulate(const Color& _col)
	{
		modulate = _col;
	}

    void _notification(int p_what)
    {

        switch (p_what)
        {
            case NOTIFICATION_ENTER_TREE:
            {
                set_use_parent_material(true);
            }
            break;
        }
    }
};

#endif // GDISPLAY_H
