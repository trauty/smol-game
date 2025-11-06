#include "test_component.h"

#include <smol/asset/asset.h>
#include <smol/asset/asset_manager.h>
#include <smol/asset/mesh.h>
#include <smol/asset/shader.h>
#include <smol/asset/texture.h>
#include <smol/components/transform.h>
#include <smol/core/gameobject.h>
#include <smol/log.h>
#include <smol/math_util.h>

using namespace smol::asset_manager;
using namespace smol::asset;

#include <iostream>

namespace smol_game
{
    void test_component_t::start()
    {
        /*SMOL_LOG_INFO("TEST_COMPONENT", "Start wurde aufgerufen");
        asset_handle_t handle = asset_manager::load_sync<texture_asset_t>("assets/uiia_cat.png", texture_asset_args_t { texture_type_e::BASE_COLOR });
        asset_ptr_t<texture_asset_t> image = asset_manager::get<texture_asset_t>(handle);
        SMOL_LOG_INFO("TEST_COMPONENT", "Breite: {}; Höhe: {};", image->get_width(), image->get_height());
        
        asset_manager::load_async<texture_asset_t>(
            "assets/uiia_cat.png", 
            texture_asset_args_t { texture_type_e::BASE_COLOR }, 
            [](asset_handle_t async_handle) {
                asset_ptr_t<texture_asset_t> async_image = asset_manager::get<texture_asset_t>(async_handle);
                SMOL_LOG_INFO("TEST_COMPONENT", "Breite: {}; Höhe: {};", async_image->get_width(), async_image->get_height());
            }
        );

        asset_handle_t shader_handle = asset_manager::load_sync<shader_asset_t>("assets/shaders/default.vert|assets/shaders/default.frag");
        asset_ptr_t<shader_asset_t> shader_prog = asset_manager::get<shader_asset_t>(shader_handle);

        asset_handle_t model_handle = asset_manager::load_sync<mesh_asset_t>("assets/models/cube.glb");
        asset_ptr_t<mesh_asset_t> model = asset_manager::get<mesh_asset_t>(model_handle);
        SMOL_LOG_INFO("TEST_COMPONENT", "Vertex count: {}", model->get_vertex_count());*/
    }

    void test_component_t::update(f64 delta_time)
    {
        vec3_t rot = {30.0f * (f32)delta_time, 20.0f * (f32)delta_time, 40.0f * (f32)delta_time};
        get_gameobject()->get_transform()->rotate_local(rot);
    }
} // namespace smol_game