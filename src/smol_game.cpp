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