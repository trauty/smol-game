#include "smol/asset.h"
#include "smol/assets/shader.h"
#include "smol/ecs_fwd.h"
#include "smol/hash.h"
#include "smol/input.h"
#include "smol/log.h"
#include "smol/rendering/graphics_state.h"
#include "smol/rendering/renderer.h"
#include "smol/rendering/rendergraph.h"
#include "smol/world.h"

#include <cmath>
#include <smol/assets/material.h>
#include <smol/assets/mesh.h>
#include <smol/assets/texture.h>
#include <smol/color.h>
#include <smol/components/camera.h>
#include <smol/components/lighting.h>
#include <smol/components/renderer.h>
#include <smol/components/transform.h>
#include <smol/ecs.h>
#include <smol/game.h>
#include <smol/systems/transform.h>
#include <smol/time.h>
#include <sys/types.h>

using namespace smol;

struct rotator_t
{
    i32_t test;
};
struct scaler_t
{
    i32_t test;
};

typedef void (*scene_load_fn)(smol::world_t*);

struct game_state_t
{
    std::vector<scene_load_fn> scenes;
    u32_t current_scene_index = 0;
};

void load_scene_base_draw(smol::world_t* world)
{
    ecs::registry_t& reg = world->registry;
    reg.clear();

    graphics_state_t& state = reg.ctx().get<graphics_state_t>();

    asset_t<mesh_t> monkee_mesh = smol::load_asset_sync<mesh_t>("assets/models/monkee.glb");
    asset_t<mesh_t> croissant_mesh = smol::load_asset_sync<mesh_t>("assets/models/croissant.glb");

    std::vector<asset_t<material_t>> materials;
    for (int i = 0; i < 5; i++)
    {
        std::string mat_name = "croissant_mat_" + std::to_string(i);
        materials.push_back(state.get_material(smol::hash_string(mat_name.c_str())));
    }

    int spawn_count = 0;
    for (int x = -10; x < 10; x++)
    {
        for (int y = -10; y < 10; y++)
        {
            ecs::entity_t test_entity = reg.create();
            reg.emplace<rotator_t>(test_entity);
            mesh_renderer_t& renderer = reg.emplace<mesh_renderer_t>(test_entity);

            renderer.mesh = spawn_count % 2 == 0 ? croissant_mesh : monkee_mesh;
            renderer.material = materials[spawn_count % materials.size()];

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
}

void load_scene_blinn_phong(smol::world_t* world)
{
    ecs::registry_t& reg = world->registry;
    reg.clear();

    graphics_state_t& state = reg.ctx().get<graphics_state_t>();

    asset_t<mesh_t> monkee_mesh = smol::load_asset_sync<mesh_t>("assets/models/monkee.glb");
    asset_t<material_t> lit_mat = state.get_material(smol::hash_string("lit_mat0"));

    ecs::entity_t obj = reg.create();
    mesh_renderer_t& renderer = reg.emplace<mesh_renderer_t>(obj);
    renderer.mesh = monkee_mesh;
    renderer.material = lit_mat;
    reg.emplace<transform_t>(obj);
    reg.emplace<rotator_t>(obj);
    reg.emplace<scaler_t>(obj);

    ecs::entity_t sun = reg.create();
    reg.emplace<transform_t>(sun);
    smol::transform_system::set_local_rotation(reg, sun, quat_t::from_euler({-0.8f, 0.5f, 0.0f}));

    directional_light_t& dir_light = reg.emplace<directional_light_t>(sun);
    dir_light.color = {1.0f, 0.95f, 0.9f};
    dir_light.intensity = 1.0f;

    ecs::entity_t p_light = reg.create();
    reg.emplace<transform_t>(p_light);
    smol::transform_system::set_local_position(reg, p_light, {2.0f, 1.0f, -2.0f});

    point_light_t& point = reg.emplace<point_light_t>(p_light);
    point.color = {0.2f, 0.5f, 1.0f};
    point.intensity = 15.0f;
    point.radius = 10.0f;

    ecs::entity_t cam = reg.create();
    reg.emplace<camera_t>(cam);
    reg.emplace<transform_t>(cam);
    reg.emplace<active_camera_tag>(cam);
    smol::transform_system::set_local_position(reg, cam, {0.0f, 0.0f, -8.0f});
}

void load_scene_transparency(smol::world_t* world)
{
    ecs::registry_t& reg = world->registry;
    reg.clear();

    graphics_state_t& state = reg.ctx().get<graphics_state_t>();
    asset_t<mesh_t> monkee_mesh = smol::load_asset_sync<mesh_t>("assets/models/monkee.glb");

    asset_t<material_t> opaque_mat = state.get_material(smol::hash_string("lit_mat0"));
    asset_t<material_t> cutout_mat = state.get_material(smol::hash_string("cutout_mat"));
    asset_t<material_t> glass_mat = state.get_material(smol::hash_string("glass_mat"));

    ecs::entity_t bg_obj = reg.create();
    mesh_renderer_t& bg_res = reg.emplace<mesh_renderer_t>(bg_obj);
    bg_res.mesh = monkee_mesh;
    bg_res.material = opaque_mat;
    reg.emplace<transform_t>(bg_obj);
    smol::transform_system::set_local_position(reg, bg_obj, {0.0f, 0.0f, 5.0f});
    reg.emplace<rotator_t>(bg_obj);

    if (cutout_mat)
    {
        ecs::entity_t c_obj = reg.create();
        mesh_renderer_t& c_res = reg.emplace<mesh_renderer_t>(c_obj);
        c_res.mesh = monkee_mesh;
        c_res.material = cutout_mat;
        reg.emplace<transform_t>(c_obj);
        smol::transform_system::set_local_position(reg, c_obj, {-2.5f, 0.0f, 0.0f});
        reg.emplace<rotator_t>(c_obj);
    }

    if (glass_mat)
    {
        ecs::entity_t t_obj = reg.create();
        mesh_renderer_t& t_res = reg.emplace<mesh_renderer_t>(t_obj);
        t_res.mesh = monkee_mesh;
        t_res.material = glass_mat;
        reg.emplace<transform_t>(t_obj);
        smol::transform_system::set_local_position(reg, t_obj, {2.5f, 0.0f, -4.0f});
        reg.emplace<rotator_t>(t_obj);
    }

    ecs::entity_t sun = reg.create();
    reg.emplace<transform_t>(sun);
    smol::transform_system::set_local_rotation(reg, sun, quat_t::from_euler({-0.5f, 0.3f, 0.0f}));
    directional_light_t& dir_light = reg.emplace<directional_light_t>(sun);
    dir_light.color = {1.0f, 1.0f, 1.0f};
    dir_light.intensity = 1.0f;

    ecs::entity_t cam = reg.create();
    reg.emplace<camera_t>(cam);
    reg.emplace<transform_t>(cam);
    reg.emplace<active_camera_tag>(cam);
    smol::transform_system::set_local_position(reg, cam, {0.0f, 0.0f, -10.0f});
}

extern "C"
{
    SMOL_API void smol_game_init(smol::world_t* world)
    {
        graphics_state_t& graphics_state = world->registry.ctx().emplace<graphics_state_t>();

        asset_t<texture_t> croissant_tex = smol::load_asset_sync<texture_t>("assets/textures/low_quality_pastry.png");
        asset_t<texture_t> cutout_test_tex = smol::load_asset_sync<texture_t>("assets/textures/cutout_test.png");
        asset_t<texture_t> rock_tex = smol::load_asset_sync<texture_t>("assets/textures/rock_albedo.png");

        asset_t<shader_t> pp_shader = smol::load_asset_sync<shader_t>("assets/shaders/post_process.slang");
        renderer::register_custom_shader(pp_shader);

        asset_t<material_t> pp_material = smol::load_asset_sync<material_t>("pp_material", "PostProcessShader");
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
                                                      mat.set_property("scene_color_tex"_h, color_id);
                                                  });
                }
            });

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
            asset_t<material_t> mat = smol::load_asset_sync<material_t>(mat_name.c_str(), "UnlitShader");

            mat->set_property<color_t>("color"_h, colors[i]);
            mat->set_texture("albedo_tex"_h, i % 2 == 0 ? croissant_tex : rock_tex);
            mat->set_sampler("albedo_sampler"_h, sampler_type_e::LINEAR_REPEAT);

            graphics_state.add_material(smol::hash_string(mat_name.c_str()), mat);
        }

        asset_t<material_t> lit_mat = smol::load_asset_sync<material_t>("lit_mat0", "SimpleLitShader");
        lit_mat->set_property<color_t>("color"_h, {1.0f, 1.0f, 1.0f, 1.0f});
        lit_mat->set_texture("albedo_tex"_h, rock_tex);
        lit_mat->set_sampler("albedo_sampler"_h, sampler_type_e::LINEAR_REPEAT);
        lit_mat->set_property<float>("smoothness"_h, 32.0f);
        graphics_state.add_material(smol::hash_string("lit_mat0"), lit_mat);

        asset_t<material_t> cutout_mat = smol::load_asset_sync<material_t>("cutout_mat", "CutoutLitShader");
        if (cutout_mat)
        {
            cutout_mat->set_property<color_t>("color"_h, {1.0f, 1.0f, 1.0f, 1.0f});
            cutout_mat->set_property<float>("alpha_cutoff"_h, 0.5f);
            cutout_mat->set_property<float>("smoothness"_h, 32.0f);
            cutout_mat->set_texture("albedo_tex"_h, cutout_test_tex);
            cutout_mat->set_sampler("albedo_sampler"_h, sampler_type_e::NEAREST_REPEAT);
            graphics_state.add_material(smol::hash_string("cutout_mat"), cutout_mat);
        }

        asset_t<material_t> glass_mat = smol::load_asset_sync<material_t>("glass_mat", "GlassShader");
        if (glass_mat)
        {
            glass_mat->set_property<color_t>("color"_h, {0.4f, 0.8f, 1.0f, 0.4f});
            graphics_state.add_material(smol::hash_string("glass_mat"), glass_mat);
        }

        game_state_t& game_state = world->registry.ctx().emplace<game_state_t>();
        game_state.scenes.push_back(load_scene_base_draw);
        game_state.scenes.push_back(load_scene_blinn_phong);
        game_state.scenes.push_back(load_scene_transparency);

        game_state.scenes[0](world);
    }

    SMOL_API void smol_game_update(smol::world_t* world)
    {
        ecs::registry_t& reg = world->registry;
        game_state_t& game_state = world->registry.ctx().get<game_state_t>();

        if (smol::input::get_key_down(input::key_t::Space))
        {
            game_state.current_scene_index = (game_state.current_scene_index + 1) % game_state.scenes.size();
            SMOL_LOG_INFO("GAME", "Switching to scene index: {}", game_state.current_scene_index);

            game_state.scenes[game_state.current_scene_index](world);
        }

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

    SMOL_API void smol_game_shutdown(smol::world_t* world) {}
}