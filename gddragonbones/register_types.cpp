#include <version_generated.gen.h>

#if (VERSION_MAJOR == 3)
#include <core/class_db.h>
#include <core/project_settings.h>
#else
#include "object_type_db.h"
#include "core/globals.h"
#endif

#include "register_types.h"

#include "core/os/file_access.h"
#include "core/os/os.h"
#include "core/io/resource_loader.h"
#include "scene/resources/texture.h"

#include "gddragonbones.h"

#if (VERSION_MAJOR == 3)
    #define REG_CLASS_BIND_GODO  ClassDB::register_class
#else
    #define REG_CLASS_BIND_GODO  ObjectTypeDB::register_type

#endif

class ResourceFormatLoaderGDDragonBones : public ResourceFormatLoader
{

public:
	virtual RES load(const String& p_path, const String& p_original_path = "", Error *p_err=NULL) {

		float __tm_start = OS::get_singleton()->get_ticks_msec();
		GDDragonBones::GDDragonBonesResource* __p_res = memnew(GDDragonBones::GDDragonBonesResource);
		Ref<GDDragonBones::GDDragonBonesResource> __p_ref(__p_res);
		
#if (VERSION_MAJOR == 3)
        String __str_path_base = p_path.get_basename();
#else
        String __str_path_base = p_path.basename();
#endif
        __str_path_base.erase(__str_path_base.length() - strlen("_ske"), strlen("_ske"));

        // texture path
        __p_ref->set_def_texture_path(__str_path_base + "_tex.png");

        // loading atlas data
        bool __bret = __p_ref->load_texture_atlas_data(String(__str_path_base + "_tex.json").ascii().get_data());
        ERR_FAIL_COND_V(!__bret, 0);

        // loading bones data
        __bret = __p_ref->load_bones_data(p_path.ascii().get_data());
        ERR_FAIL_COND_V(!__bret, 0);

#ifdef TOOLS_ENABLED
        __p_res->set_path(p_path, true);
#else
	__p_res->set_path(p_path);
#endif
		
		float __tm_finish = OS::get_singleton()->get_ticks_msec();
	        print_line("DB resource (" + p_path + ") loaded in " + itos(__tm_finish-__tm_start) + " msecs");
		return __p_ref;
	}


    virtual void get_recognized_extensions(List<String> *p_extensions) const
    {
		p_extensions->push_back("dbbin");
		p_extensions->push_back("json");
	}

    virtual bool handles_type(const String& p_type) const
    {

		return p_type=="GDDragonBonesResource";
	}

#if (VERSION_MAJOR == 3)
    virtual String get_resource_type(const String &p_path) const
    {
        String el = p_path.get_extension().to_lower();
        if ((el == "json" || el == "dbbin") && p_path.get_basename().to_lower().ends_with("_ske"))
            return "GDDragonBonesResource";
        return "";
    }
#else
    virtual String get_resource_type(const String &p_path) const
    {
        String el = p_path.extension().to_lower();
        if ((el == "json" || el == "dbbin") && p_path.basename().to_lower().ends_with("_ske"))
            return "GDDragonBonesResource";
        return "";
    }
#endif

};

static ResourceFormatLoaderGDDragonBones *resource_loader_GDDragonBones = NULL;

void register_gddragonbones_types()
{
    REG_CLASS_BIND_GODO<GDDragonBones>();
    REG_CLASS_BIND_GODO<GDDragonBones::GDDragonBonesResource>();
	resource_loader_GDDragonBones = memnew( ResourceFormatLoaderGDDragonBones );
	ResourceLoader::add_resource_format_loader(resource_loader_GDDragonBones);
}

void unregister_gddragonbones_types()
{
	if (resource_loader_GDDragonBones)
		memdelete(resource_loader_GDDragonBones);
}


