#pragma once

#include "GDDisplay.h"
#include "godot_cpp/classes/rendering_server.hpp"
#include "godot_cpp/variant/rid.hpp"

#include "godot_cpp/variant/utility_functions.hpp"

DRAGONBONES_NAMESPACE_BEGIN

class GDMesh : public GDDisplay {
	GDCLASS(GDMesh, GDDisplay)
protected:
	static void _bind_methods() {}

private:
	using RS = godot::RenderingServer;

	GDMesh(const GDMesh &);

public:
	godot::PackedInt32Array indices;
	godot::PackedColorArray verticesColor;
	godot::PackedVector2Array verticesUV;
	godot::PackedVector2Array verticesPos;

	godot::Color col_debug;

public:
	GDMesh() :
			col_debug{ godot::Color(
					godot::UtilityFunctions::randf_range(0.5f, 1.f),
					godot::UtilityFunctions::randf_range(0.3f, 1.f),
					godot::UtilityFunctions::randf_range(0.3f, 1.f), 1) } {}
	virtual ~GDMesh() {}

	static GDMesh *create() {
		return memnew(GDMesh);
	}

	virtual void _draw() override {
		if (indices.is_empty())
			return;

		if (texture.is_valid()) {
			verticesColor.fill(get_modulate());

			RS::get_singleton()->canvas_item_add_triangle_array(
					get_canvas_item(),
					indices,
					verticesPos,
					verticesColor,
					verticesUV,
					{},
					{},
					texture.is_valid() ? texture->get_rid() : godot::RID(),
					-1);
		}

		if (b_debug || !texture.is_valid()) {
			for (size_t idx = 0; idx < indices.size(); idx += 3) {
				col_debug.a = get_modulate().a;

				RS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx]], verticesPos[indices[idx + 1]], col_debug, 1.0);
				RS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx + 1]], verticesPos[indices[idx + 2]], col_debug, 1.0);
				RS::get_singleton()->canvas_item_add_line(get_canvas_item(), verticesPos[indices[idx + 2]], verticesPos[indices[idx]], col_debug, 1.0);
			}
		}
	}
};

DRAGONBONES_NAMESPACE_END
