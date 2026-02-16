#include "smol/assets/shader.h"
#include "smol/components/transform.h"
#include "smol/ecs.h"
#include "smol/log.h"
#include "smol/math.h"
#include "smol/rendering/material.h"
#include "smol/systems/transform.h"
#include "smol/time.h"
#include "smol/world.h"

#include "cglm/cglm.h"

#include <memory>
#include <smol/asset.h>
#include <smol/assets/mesh.h>
#include <smol/assets/shader.h>
#include <smol/assets/texture.h>
#include <smol/components/camera.h>
#include <smol/components/renderer.h>
#include <smol/engine.h>
#include <utility>

using namespace smol;

struct rotator_t
{
    SMOL_COMPONENT(rotator_t)
};

int main()
{
    smol::engine::init("smol-game", 1280, 720);

    {
        std::unique_ptr<smol::world_t> cur_scene = std::make_unique<smol::world_t>();
        smol::engine::set_scene(std::move(cur_scene));

        ecs::registry_t& reg = smol::engine::get_active_world().registry;

        asset_t<mesh_t> cube_mesh = smol::load_asset<mesh_t>(reg, "assets/models/croissant.glb");
        asset_t<shader_t> cube_shader = smol::load_asset<shader_t>(reg, "assets/shaders/unlit.slang");
        asset_t<texture_t> cube_tex = smol::load_asset<texture_t>(reg, "assets/textures/pastry.png");

        ecs::entity_t test_entity = reg.create();
        reg.emplace<rotator_t>(test_entity);
        mesh_renderer_t& renderer = reg.emplace<mesh_renderer_t>(test_entity);
        renderer.mesh = cube_mesh;
        renderer.material.set_shader(cube_shader);
        renderer.material.set_texture("albedo_tex", cube_tex);
        renderer.material.set_texture("albedo_sampler", cube_tex);

        transform_t& test_transform = reg.emplace<transform_t>(test_entity);

        ecs::entity_t cam_entity = reg.create();
        camera_t& cam = reg.emplace<camera_t>(cam_entity);
        reg.emplace<transform_t>(cam_entity);

        smol::transform_system::set_local_position(reg, cam_entity, {0.0f, 0.0f, -5.0f});

        smol::engine::get_active_world().register_update_system([](ecs::registry_t& reg) {
            vec3_t rot = {0.8f * (f32)time::time, 0.7f * (f32)time::time, 0.9f * (f32)time::time};
            vec3_t scale = {30.0f * std::abs(std::sin((f32)time::time)), 30.0f * std::abs(std::sin((f32)time::time)),
                            30.0f * std::abs(std::sin((f32)time::time))};

            for (auto [entity, rotator, transform] : reg.view<rotator_t, transform_t>())
            {
                transform.local_rotation = quat_t::from_euler(rot);
                transform.local_scale = scale;
                transform.is_dirty = true;
            }
        });

        smol::engine::run();
    }

    smol::engine::shutdown();

    return 0;
}