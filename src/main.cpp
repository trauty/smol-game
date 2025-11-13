#include "test_component.h"

#include <smol/asset/asset.h>
#include <smol/asset/asset_manager.h>
#include <smol/asset/mesh.h>
#include <smol/asset/shader.h>
#include <smol/asset/texture.h>
#include <smol/components/camera.h>
#include <smol/components/mesh_renderer.h>
#include <smol/core/gameobject.h>
#include <smol/core/level.h>
#include <smol/engine.h>
#include <smol/rendering/spatial_material.h>

using namespace smol::asset_manager;
using namespace smol::asset;

int main()
{
    smol::engine::init("smol-game", 1280, 720);

    std::shared_ptr<smol::core::level_t> first_level = std::make_shared<smol::core::level_t>("TestLevel");
    smol::engine::load_level(first_level);
    std::shared_ptr<smol::core::gameobject_t> cam_obj = first_level->create_gameobject("cam");
    cam_obj->add_component<smol::components::camera_ct>()->set_as_main_camera();
    smol::math::vec3_t new_rot {0.0f, 0.0f, 0.0f};
    cam_obj->get_transform()->rotate_local(new_rot);

    std::shared_ptr<smol::core::gameobject_t> cube_obj = first_level->create_gameobject("cube");
    smol::components::mesh_renderer_ct* mesh_renderer = cube_obj->add_component<smol::components::mesh_renderer_ct>();
    cube_obj->add_component<smol_game::test_component_t>();

    asset_handle_t shader_handle = smol::asset_manager::load_sync<shader_asset_t>("assets/shaders/unlit.vert|assets/shaders/unlit.frag");
    asset_ptr_t<shader_asset_t> shader_prog = smol::asset_manager::get<shader_asset_t>(shader_handle);

    asset_handle_t model_handle = smol::asset_manager::load_sync<mesh_asset_t>("assets/models/croissant.glb");
    asset_ptr_t<mesh_asset_t> model = smol::asset_manager::get<mesh_asset_t>(model_handle);

    asset_handle_t tex_handle = smol::asset_manager::load_sync<texture_asset_t>("assets/textures/pastry.png", texture_asset_args_t {texture_type_e::ALBEDO});
    asset_ptr_t<texture_asset_t> rock_tex = smol::asset_manager::get<texture_asset_t>(tex_handle);

    std::shared_ptr<smol::rendering::spatial_material_t> cube_mat = std::make_shared<smol::rendering::spatial_material_t>(shader_prog);
    cube_mat->albedo_texture = rock_tex;
    mesh_renderer->set_mesh(model);
    mesh_renderer->set_material(cube_mat);

    smol::math::vec3_t new_pos {0.0f, 0.0f, 5.0f};
    cube_obj->get_transform()->set_local_position(new_pos);

    smol::math::vec3_t new_scale {20.0f, 20.0f, 20.0f};
    cube_obj->get_transform()->set_local_scale(new_scale);

    smol::engine::run();

    smol::engine::shutdown();

    return 0;
}