#include "smol/asset.h"
#include "smol/asset_registry.h"
#include "smol/asset_types.h"
#include "smol/assets/material.h"
#include "smol/assets/mesh.h"
#include "smol/assets/shader.h"
#include "smol/assets/texture.h"
#include "smol/color.h"
#include "smol/components/renderer.h"
#include "smol/components/transform.h"
#include "smol/ecs.h"
#include "smol/engine.h"
#include "smol/game.h"
#include "smol/hash.h"
#include "smol/log.h"
#include "smol/math.h"
#include "smol/reflection.h"
#include "smol/rendering/graphics_state.h"
#include "smol/rendering/renderer.h"
#include "smol/rendering/rendergraph.h"
#include "smol/time.h"
#include "smol/world.h"

#include <cmath>
#include <string>

using namespace smol;

struct rotator_t
{
    i32_t test;
};
struct scaler_t
{
    i32_t test;
};

void smol_game_init(smol::world_t* world)
{
    smol::reflection::factory<rotator_t>{*world->reflection_ctx}
        .type("rotator_t"_h)
        .custom<smol::reflection::editor_prop_t>("Rotator")
        .func<&smol::reflection::get_component<rotator_t>>("get"_h)
        .func<&smol::reflection::add_component<rotator_t>>("add"_h)
        .func<&smol::reflection::remove_component<rotator_t>>("remove"_h)
        .data<&rotator_t::test>("test"_h)
        .custom<smol::reflection::editor_prop_t>("Test Value");

    smol::reflection::factory<scaler_t>{*world->reflection_ctx}
        .type("scaler_t"_h)
        .custom<smol::reflection::editor_prop_t>("Scaler")
        .func<&smol::reflection::get_component<scaler_t>>("get"_h)
        .func<&smol::reflection::add_component<scaler_t>>("add"_h)
        .func<&smol::reflection::remove_component<scaler_t>>("remove"_h)
        .data<&scaler_t::test>("test"_h)
        .custom<smol::reflection::editor_prop_t>("Test Value");

    graphics_state_t& graphics_state = world->registry.ctx().emplace<graphics_state_t>();

    asset_handle_t pp_shader =
        smol::engine::get_asset_registry().load_sync<shader_t>("game://assets/shaders/post_process.slang");

    asset_handle_t pp_material = smol::engine::get_asset_registry().load_sync<material_t>("pp_material", pp_shader);
    graphics_state.add_material("PostProcessing"_h, pp_material);

    renderer::register_renderer_feature(
        [](renderer::rendergraph_t& graph, ecs::registry_t& reg)
        {
            renderer::rg_resource_id scene_color = graph.get_resource("SceneColor"_h);
            renderer::rg_resource_id scene_depth = graph.get_resource("SceneDepth"_h);

            renderer::rg_resource_id final_target = graph.get_resource("FinalOutput"_h);

            renderer::add_mesh_pass(graph, "MainForward"_h, "MainForwardPass", "MainForwardPass", {}, {scene_color},
                                    scene_depth);

            graphics_state_t& graphics_state = reg.ctx().get<graphics_state_t>();
            material_t* pp_mat = graphics_state.get_material_raw("PostProcessing"_h);

            if (pp_mat && pp_mat->shader_handle.is_valid())
            {
                renderer::add_fullscreen_pass(graph, "PostProcess"_h, "PostProcess", pp_mat, {scene_color},
                                              {final_target},
                                              [](renderer::rendergraph_t& g, material_t& mat)
                                              {
                                                  u32_t color_id = g.get_bindless_id(g.get_resource("SceneColor"_h));
                                                  mat.set_property("scene_color_tex"_h, color_id);
                                              });
            }
        });
}

void smol_game_update(smol::world_t* world)
{
    ecs::registry_t& reg = world->registry;

    vec3_t rot = {0.8f * (f32)time::get_time(), 0.7f * (f32)time::get_time(), 0.9f * (f32)time::get_time()};
    for (auto [entity, rotator, transform] : reg.view<rotator_t, transform_t>().each())
    {
        transform.local_rotation = quat_t::from_euler(rot);
        transform.is_dirty = true;
    }

    vec3_t scale = {2.0f * std::cos((f32)time::get_time()), 1.0f, 2.0f * std::cos((f32)time::get_time())};
    for (auto [entity, rotator, transform] : reg.view<scaler_t, transform_t>().each())
    {
        transform.local_scale = scale;
        transform.is_dirty = true;
    }
}

void smol_game_shutdown(smol::world_t* world) {}

SMOL_GAME_ENTRY()
