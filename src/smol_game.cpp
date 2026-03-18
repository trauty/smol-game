#include "smol/asset.h"
#include "smol/assets/shader.h"
#include "smol/ecs_fwd.h"
#include "smol/hash.h"
#include "smol/rendering/graphics_state.h"
#include "smol/rendering/renderer.h"
#include "smol/rendering/renderer_resources.h"
#include "smol/rendering/renderer_types.h"
#include "smol/rendering/rendergraph.h"
#include "smol/rendering/vulkan.h"
#include "vulkan/vulkan_core.h"

#include <smol/assets/material.h>
#include <smol/assets/mesh.h>
#include <smol/assets/texture.h>
#include <smol/color.h>
#include <smol/components/camera.h>
#include <smol/components/renderer.h>
#include <smol/components/transform.h>
#include <smol/ecs.h>
#include <smol/game.h>
#include <smol/systems/transform.h>
#include <smol/time.h>

using namespace smol;

struct rotator_t
{
    i32_t test;
};

extern "C"
{
    SMOL_API void smol_game_init(smol::world_t* world)
    {
        graphics_state_t& graphics_state = world->registry.ctx().emplace<graphics_state_t>();

        asset_t<mesh_t> croissant_mesh = smol::load_asset_sync<mesh_t>("assets/models/croissant.glb");
        asset_t<mesh_t> monkee_mesh = smol::load_asset_sync<mesh_t>("assets/models/monkee.glb");
        asset_t<texture_t> croissant_tex = smol::load_asset_sync<texture_t>("assets/textures/low_quality_pastry.png");
        asset_t<texture_t> rock_tex = smol::load_asset_sync<texture_t>("assets/textures/rock_albedo.png");
        asset_t<shader_t> unlit_shader = smol::load_asset_sync<shader_t>("assets/shaders/unlit.slang");

        asset_t<shader_t> pp_shader = smol::load_asset_sync<shader_t>("assets/shaders/post_process.slang");
        asset_t<material_t> pp_material = smol::load_asset_sync<material_t>("pp_material", pp_shader);

        graphics_state.add_material(smol::hash_string("PostProcessing"), pp_material);

        renderer::register_renderer_feature(
            [](renderer::rendergraph_t& graph, ecs::registry_t& reg)
            {
                renderer::rg_resource_id scene_color = graph.get_resource("SceneColor");
                renderer::rg_resource_id scene_depth = graph.get_resource("SceneDepth");
                renderer::rg_resource_id swapchain = graph.get_resource("Swapchain");

                renderer::add_mesh_pass(graph, "MainForward", "MainForwardPass", {}, {scene_color}, scene_depth);

                graphics_state_t& graphics_state = reg.ctx().get<graphics_state_t>();
                asset_t<material_t> pp_mat = graphics_state.get_material(smol::hash_string("PostProcessing"));

                if (pp_mat && pp_mat->shader && pp_mat->shader->ready())
                {
                    renderer::add_fullscreen_pass(graph, "PostProcess", pp_mat, {scene_color}, {swapchain},
                                                  [](renderer::rendergraph_t& g, material_t& mat)
                                                  {
                                                      u32_t color_id = g.get_bindless_id(g.get_resource("SceneColor"));
                                                      mat.set_property("scene_color_tex", color_id);
                                                  });
                }
            });

        std::vector<asset_t<material_t>> material_palette;

        color_t colors[5] = {
            {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.2f, 0.2f, 1.0f},
            {0.2f, 1.0f, 0.2f, 1.0f},
            {0.2f, 0.2f, 1.0f, 1.0f},
            {1.0f, 1.0f, 0.2f, 1.0f}
        };

        for (int i = 0; i < 5; i++)
        {
            std::string mat_name = "croissant_mat_" + std::to_string(i);
            asset_t<material_t> mat = smol::load_asset_sync<material_t>(mat_name.c_str(), unlit_shader);

            mat->set_property<color_t>("color", colors[i]);
            mat->set_texture("albedo_tex", i % 2 == 0 ? croissant_tex : rock_tex);
            mat->set_sampler("albedo_sampler", sampler_type_e::NEAREST_REPEAT);

            material_palette.push_back(mat);
        }

        ecs::registry_t& reg = world->registry;

        int spawn_count = 0;
        for (int x = -10; x < 10; x++)
        {
            for (int y = -10; y < 10; y++)
            {
                ecs::entity_t test_entity = reg.create();
                reg.emplace<rotator_t>(test_entity);
                mesh_renderer_t& renderer = reg.emplace<mesh_renderer_t>(test_entity);

                renderer.mesh = spawn_count % 2 == 0 ? croissant_mesh : monkee_mesh;

                renderer.material = material_palette[spawn_count % material_palette.size()];

                reg.emplace<transform_t>(test_entity);
                smol::transform_system::set_local_position(reg, test_entity, {x * 2.0f, y * 2.0f, 0.0f});

                if (renderer.mesh != monkee_mesh)
                {
                    smol::transform_system::set_local_scale(reg, test_entity, {15.0f, 15.0f, 15.0f});
                }

                spawn_count++;
            }
        }

        ecs::entity_t cam_entity = reg.create();
        reg.emplace<camera_t>(cam_entity);
        reg.emplace<transform_t>(cam_entity);
        reg.emplace<active_camera_tag>(cam_entity);

        smol::transform_system::set_local_position(reg, cam_entity, {0.0f, 0.0f, -36.0f});

        smol::engine::get_active_world().register_update_system(
            [](ecs::registry_t& reg)
            {
                vec3_t rot = {0.8f * (f32)time::get_time(), 0.7f * (f32)time::get_time(), 0.9f * (f32)time::get_time()};

                for (auto [entity, rotator, transform] : reg.view<rotator_t, transform_t>().each())
                {
                    transform.local_rotation = quat_t::from_euler(rot);
                    transform.is_dirty = true;
                }
            });
    }

    SMOL_API void smol_game_update(smol::world_t* world) {}

    SMOL_API void smol_game_shutdown(smol::world_t* world) {}
}