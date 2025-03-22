#include "PrimitiveMesh.h"
#include "Geometry.h"

namespace primal::tools
{
	namespace 
	{
		using primitive_mesh_creator = void(*)(scene&, const primitive_init_info& info);

		void create_plane(scene& scene, const primitive_init_info& info);
		void create_cube(scene& scene, const primitive_init_info& info);
		void create_uv_sphere(scene& scene, const primitive_init_info& info);
		void create_ico_sphere(scene& scene, const primitive_init_info& info);
		void create_cylinder(scene& scene, const primitive_init_info& info);
		void create_capsule(scene& scene, const primitive_init_info& info);

		primitive_mesh_creator creators[]{
			create_plane,
			create_cube,
			create_uv_sphere,
			create_ico_sphere,
			create_cylinder,
			create_capsule
		};
		static_assert(_countof(creators) == primitive_mesh_type::count);

		enum class axis : u32 {
			x = 0,
			y = 1,
			z = 2
		};


		//horizontal,vertical 인덱스를 사용해서 생성할 3차원 평면 설정가능
		mesh create_plane(const primitive_init_info& info, u32 horizontal_index = (u32)axis::x, u32 vertical_index = (u32)axis::z,
		                  bool flip_winding = false, math::v3 offset = {-0.5f, 0.f, -0.5f},
		                  math::v2 u_range = {0.f, 1.f}, math::v2 v_range = {0.f, 1.f}) {
			assert(horizontal_index < 3 && vertical_index < 3);
			assert(horizontal_index != vertical_index);

			const u32 horizontal_count{ math::clamp(info.segments[horizontal_index], 1u, 10u)};
			const u32 vertical_count{ math::clamp(info.segments[vertical_index], 1u, 10u)};
			const f32 horizontal_step{ 1.f / horizontal_count };
			const f32 vertical_step{ 1.f / vertical_count };
			const f32 u_steps{ (u_range.y - u_range.x) / horizontal_count };
			const f32 v_steps{ (v_range.y - v_range.x) / vertical_count };

			mesh m{};
			utl::vector<math::v2> uvs;

			for (u32 j{ 0 }; j <= vertical_count; ++j) {
				for (u32 i{ 0 }; i <= horizontal_count; ++i) {
					math::v3 position{ offset };
					f32* const as_array{ &position.x };
					as_array[horizontal_index] += i * horizontal_step;
					as_array[vertical_index] += j * vertical_step;
					m.positions.emplace_back(position.x * info.size.x, position.y * info.size.y, position.z * info.size.z);

					math::v2 uv{ u_range.x , 1.0f - v_range.x };
					uv.x += i * u_steps;
					uv.y -= j * v_steps;
					uvs.emplace_back(uv);
				}
			}

			assert(m.positions.size() == (u64)(horizontal_count + 1) * (u64)(vertical_count + 1));

			const u32 row_length{ horizontal_count + 1 };
			for (u32 j{ 0 }, k{ 0 }; j < vertical_count; ++j) {
				for (u32 i{ 0 }; i < horizontal_count; ++i) {
					const u32 index[4]
					{
						i + j * row_length,
						i + (j + 1) * row_length,
						i + 1 + j * row_length,
						i + 1 + (j + 1) * row_length
					};
					if (flip_winding) {
						m.raw_indices.emplace_back(index[0]);
						m.raw_indices.emplace_back(index[1]);
						m.raw_indices.emplace_back(index[2]);

						m.raw_indices.emplace_back(index[2]);
						m.raw_indices.emplace_back(index[1]);
						m.raw_indices.emplace_back(index[3]);
						
					}
					else {
						m.raw_indices.emplace_back(index[0]);
						m.raw_indices.emplace_back(index[2]);
						m.raw_indices.emplace_back(index[1]);

						m.raw_indices.emplace_back(index[2]);
						m.raw_indices.emplace_back(index[3]);
						m.raw_indices.emplace_back(index[1]);
					}
				}
				++k;
			}
			const u32 num_indices{ 3 * 2 * horizontal_count * vertical_count };
			assert(m.raw_indices.size() == num_indices);

			for (u32 i{ 0 }; i < num_indices; ++i) {
				m.uvs[0].emplace_back(uvs[m.raw_indices[i]]);
			}
			return m;
		}

		void create_plane(scene& scene, const primitive_init_info& info) {
			lod_group lod{};
			lod.name = "plane";
			lod.meshes.emplace_back(create_plane(info));
			scene.lod_group.emplace_back(lod);
		}
		void create_cube(scene& scene, const primitive_init_info& info) {
		}
		void create_uv_sphere(scene& scene, const primitive_init_info& info) {
		}
		void create_ico_sphere(scene& scene, const primitive_init_info& info) {
		}
		void create_cylinder(scene& scene, const primitive_init_info& info) {
		}
		void create_capsule(scene& scene, const primitive_init_info& info) {
		}

		
	}


	
	EDITOR_INTERFACE void CreatePrimitiveMesh(scene_data* data, primitive_init_info* info) {
		assert(data && info);
		assert(info->type < primitive_mesh_type::count);
		scene scene{};
		creators[info->type](scene, *info);

		data->settings.calculate_normals = 1;
		process_scene(scene, data->settings);
		pack_data(scene, *data);

		//TODO: 씬 처리하고 패킹해서 레벨에디터로 보내야됨
	}
}


