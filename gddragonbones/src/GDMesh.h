#ifndef GDMESH_H
#define GDMESH_H

#include "GDDisplay.h"

class GDMesh : public GDDisplay
{ 
#if (VERSION_MAJOR == 3)
    GDCLASS(GDMesh, GDDisplay);
#else
    OBJ_TYPE(GDMesh, GDDisplay);
#endif

private:
    GDMesh(const GDMesh&);

public:
    Vector<int>							indices;
    Vector<Color>						verticesColor;
    Vector<Point2>						verticesUV;
    Vector<Point2>						verticesPos;

    Color                               col_debug;

public:
    GDMesh(){ col_debug = Color( Math::random(0.5f, 1.f), Math::random(0.3f, 1.f), Math::random(0.3f, 1.f), 1);}
	virtual ~GDMesh() {}

    static  GDMesh* create()
    {
        return memnew(GDMesh);
    }

    void _render()
	{
        if (indices.empty())
			return;

        if(texture.is_valid())
        {
            VS::get_singleton()->canvas_item_add_triangle_array(
                get_canvas_item(),
                indices,
                verticesPos,
                verticesColor,
                verticesUV,
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
		Vector<int>(),
		Vector<float>(),
#endif
                texture.is_valid() ? texture->get_rid() : RID(),
                -1
            );
        }

        if(b_debug || !texture.is_valid())
        {
            for (int idx = 0; idx < indices.size(); idx += 3)
            {
                VS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx]], verticesPos[indices[idx+1]], col_debug, 1.0);
                VS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx+1]], verticesPos[indices[idx+2]], col_debug, 1.0);
                VS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx+2]], verticesPos[indices[idx]], col_debug, 1.0);
            }
        }
	}

    virtual void set_modulate(const Color& _col)
    {
        GDDisplay::set_modulate(_col);
        col_debug.a = modulate.a;
        for(unsigned long i = 0; i < verticesColor.size(); ++i)
        {
#if (VERSION_MAJOR == 3 && VERSION_MINOR >= 1)
            verticesColor.write[i] = modulate;
#else
			verticesColor[i] = modulate;
#endif
        }
    }

    void _notification(int p_what)
    {

        switch (p_what)
        {
            case NOTIFICATION_DRAW:
                _render();
                break;
        }
    }
};

#endif // #ifndef GDMESH_H
