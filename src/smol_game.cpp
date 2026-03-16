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
        asset_t<mesh_t> cube_mesh = smol::load_asset_sync<mesh_t>("assets/models/croissant.glb");
        asset_t<texture_t> cube_tex = smol::load_asset_sync<texture_t>("assets/textures/low_quality_pastry.png");
        asset_t<shader_t> cube_shader = smol::load_asset_sync<shader_t>("assets/shaders/unlit.slang");
        asset_t<material_t> cube_material = smol::load_asset_sync<material_t>("cube_material", cube_shader);

        cube_material->set_property<color_t>("color", color_t());
        cube_material->set_texture("albedo_tex", cube_tex);
        cube_material->set_sampler("albedo_sampler", sampler_type_e::NEAREST_REPEAT);

        ecs::registry_t& reg = world->registry;

        ecs::entity_t test_entity = reg.create();
        reg.emplace<rotator_t>(test_entity);
        mesh_renderer_t& renderer = reg.emplace<mesh_renderer_t>(test_entity);
        renderer.mesh = cube_mesh;
        renderer.material = cube_material;

        reg.emplace<transform_t>(test_entity);

        ecs::entity_t cam_entity = reg.create();
        reg.emplace<camera_t>(cam_entity);
        reg.emplace<transform_t>(cam_entity);
        reg.emplace<active_camera_tag>(cam_entity);

        smol::transform_system::set_local_position(reg, cam_entity, {0.0f, 0.0f, -5.0f});
        smol::transform_system::set_local_position(reg, test_entity, {0.0f, 0.0f, 0.0f});

        smol::engine::get_active_world().register_update_system(
            [](ecs::registry_t& reg)
            {
                vec3_t rot = {0.8f * (f32)time::get_time(), 0.7f * (f32)time::get_time(), 0.9f * (f32)time::get_time()};
                /*vec3_t scale = {30.0f * std::abs(std::sin((f32)time::time)),
                                30.0f * std::abs(std::sin((f32)time::time)),
                                30.0f * std::abs(std::sin((f32)time::time))};*/

                vec3_t scale = {15.0f, 15.0f, 15.0f};

                for (auto [entity, rotator, transform] : reg.view<rotator_t, transform_t>().each())
                {
                    transform.local_rotation = quat_t::from_euler(rot);
                    transform.local_scale = scale;
                    transform.is_dirty = true;
                }
            });
    }

    SMOL_API void smol_game_update(smol::world_t* world) {}
    SMOL_API void smol_game_shutdown(smol::world_t* world) {}
}