#ifndef GDDRAGONBONES_H
#define GDDRAGONBONES_H

#include "scene/2d/node_2d.h"
#include "src/GDFactory.h"
#include "scene/resources/texture.h"

DRAGONBONES_USING_NAME_SPACE;

class GDDragonBones : public GDOwnerNode
{
#if (VERSION_MAJOR == 3)
    GDCLASS(GDDragonBones, GDOwnerNode);
#else
    OBJ_TYPE(GDDragonBones, GDOwnerNode);
#endif

public:
    enum AnimMode {
        ANIMATION_PROCESS_FIXED,
        ANIMATION_PROCESS_IDLE,
    };

    enum AnimFadeOutMode {
    	FadeOut_None,
        FadeOut_SameLayer,
        FadeOut_SameGroup,
        FadeOut_SameLayerAndGroup,
        FadeOut_All,
        FadeOut_Single
    };


	// Resource class
    class GDDragonBonesResource : public Resource
    {
#if (VERSION_MAJOR == 3)
    GDCLASS(GDDragonBonesResource, Resource);
#else
    OBJ_TYPE(GDDragonBonesResource, Resource);
#endif
	
	public:		
		GDDragonBonesResource();
        ~GDDragonBonesResource();

        void       set_def_texture_path(const String& _path);
        bool       load_texture_atlas_data(const String& _path);
        bool       load_bones_data(const String& _path);

        String           str_default_tex_path;
        char*            p_data_texture_atlas;
        char*            p_data_bones;
	};

private:
    GDFactory*                  p_factory;
    Ref<Texture>                m_texture_atlas;
    Ref<GDDragonBonesResource>  m_res;
    String                      str_curr_anim;
    GDArmatureDisplay*          p_armature;
    AnimMode                    m_anim_mode;
    float                       f_speed;
    float                       f_progress;
    int                         c_loop;
    bool                        b_processing;
    bool                        b_active;
    bool                        b_playing;
    bool                        b_debug;
    bool                        b_inited;
    bool                        b_try_playing;
    bool                        b_flip_x;
    bool                        b_flip_y;
    bool                        b_inherit_child_material;

protected:
    void _notification(int _what);
	static void _bind_methods();

    bool _set(const StringName& _str_name, const Variant& _c_r_value);
    bool _get(const StringName& _str_name, Variant &_r_ret) const;
    void _get_property_list(List<PropertyInfo> *_p_list) const;

public:
	GDDragonBones();
    ~GDDragonBones();

    void    _cleanup();

    // to initial pose current animation
    void    _reset();
    void    _set_process(bool _b_process, bool _b_force = false);

    void dispatch_event(const String& _str_type, const EventObject* _p_value);
    void dispatch_snd_event(const String& _str_type, const EventObject* _p_value);

    // setters/getters
    void set_resource(Ref<GDDragonBonesResource> _p_data);
    Ref<GDDragonBonesResource> get_resource();

    void set_inherit_material(bool _b_enable);
    bool is_material_inherited() const;

#if (VERSION_MAJOR == 3)
#else
    void set_opacity(float _f_opacity);
    float get_opacity() const;

    void set_blend_mode(CanvasItem::BlendMode _blend_mode);
     CanvasItem::BlendMode get_blend_mode() const;

     void set_modulate(const Color& _p_color);
     Color get_modulate() const;
#endif

    void fade_in(const String& _name_anim, float _time, int _loop, int _layer, const String& _group, GDDragonBones::AnimFadeOutMode _fade_out_mode);
    void fade_out(const String& _name_anim);

    void set_active(bool _b_active);
    bool is_active() const;

    void set_debug(bool _b_debug);
    bool is_debug() const;

    void set_speed(float _f_speed);
    float get_speed() const;

    void set_texture(const Ref<Texture> &_p_texture);
    Ref<Texture> get_texture() const;

    String get_current_animation() const;

    float tell() const;
    void seek(float _f_p);
    float get_progress() const;

    void set_animation_process_mode(GDDragonBones::AnimMode _mode);
    AnimMode get_animation_process_mode() const;

    void    play(bool _b_play = true);
    void    play_from_time(float _f_time);
    void    play_from_progress(float _f_progress);

    void    flip_x(bool _b_flip);
    bool    is_fliped_x() const;

    void    flip_y(bool _b_flip);
    bool    is_fliped_y() const;

    bool    is_playing() const;

    bool    has_anim(const String& _str_anim) const;
    void    stop(bool _b_all = false);
    inline void    stop_all() { stop(true); }
};

VARIANT_ENUM_CAST(GDDragonBones::AnimMode);
VARIANT_ENUM_CAST(GDDragonBones::AnimFadeOutMode);


#endif // GDDRAGONBONES_H


