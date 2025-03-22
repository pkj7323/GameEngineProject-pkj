#include "Geometry.h"

namespace primal::tools
{
	namespace
	{
		using namespace DirectX;
		using namespace math;
		void recalculate_normals(mesh& mesh) {
			const u32 num_indices{ static_cast<u32>(mesh.raw_indices.size()) };
			mesh.normals.reserve(num_indices);

			for (u32 i = 0; i < num_indices; ++i)
			{
				const u32 i0{ mesh.raw_indices[i] };
				const u32 i1{ mesh.raw_indices[++i] };
				const u32 i2{ mesh.raw_indices[++i] };

				XMVECTOR v0{ XMLoadFloat3(&mesh.positions[i0]) };
				XMVECTOR v1{ XMLoadFloat3(&mesh.positions[i1]) };
				XMVECTOR v2{ XMLoadFloat3(&mesh.positions[i2]) };

				XMVECTOR e0{ v1 - v0 };
				XMVECTOR e1{ v2 - v0 };
				XMVECTOR n{ XMVector3Normalize(XMVector3Cross(e0, e1)) };

				XMStoreFloat3(&mesh.normals[i], n);
				mesh.normals[i - 1] = mesh.normals[i];
				mesh.normals[i - 2] = mesh.normals[i];
			}
		}

		void process_normals(mesh& mesh, f32 smoothing_angle) {
			const f32 cos_angle{ XMScalarCos(pi - smoothing_angle * pi / 180.f) };
			const bool is_hard_edge{ XMScalarNearEqual(smoothing_angle,180.f,epsilon) };
			const bool is_soft_edge{ XMScalarNearEqual(smoothing_angle,0,epsilon) };
			const u32 num_indices{ static_cast<u32>(mesh.raw_indices.size()) };
			const u32 num_vertices{ static_cast<u32>(mesh.positions.size()) };
			assert(num_indices && num_vertices);

			mesh.indices.resize(num_indices);

			utl::vector<utl::vector<u32>> idx_ref(num_vertices);
			for (u32 i{ 0 }; i < num_indices; ++i) {
				idx_ref[mesh.raw_indices[i]].emplace_back(i);
			}
			for (u32 i{0};i<num_vertices; ++i)
			{
				auto& ref{ idx_ref[i] };
				u32 num_ref{ static_cast<u32>(ref.size()) };
				for (u32 j = 0; j < num_ref; ++j)
				{
					mesh.indices[ref[j]] = (u32)mesh.vertices.size();
					vertex& v{ mesh.vertices.emplace_back() };
					v.position = mesh.positions[mesh.raw_indices[ref[j]]];

					XMVECTOR n1{ XMLoadFloat3(&mesh.normals[ref[j]]) };
					if (!is_hard_edge)
					{
						for (u32 k = j + 1; k < num_ref; k++) {
							f32 n{ 0.1f };
							XMVECTOR n2{ XMLoadFloat3(&mesh.normals[ref[k]]) };
							if(!is_soft_edge)
							{
								// NOTE : 이 계산에서 n1의 길이를 고려하는 이유는 이 루프 반복에서 변경될 수 있기 때문임
								//		우리는 n2의 유닛 길이로 가정한다.
								// cos(angle) = dot(n1,n2) / (||(n1)|| * ||(n2)||)
								XMStoreFloat(&n, XMVector3Dot(n1, n2) * XMVector3ReciprocalLength(n1));
							}

							if (is_soft_edge || n >= cos_angle) {
								n1 += n2;

								mesh.indices[ref[k]] = mesh.indices[ref[j]];
								ref.erase(ref.begin() + k);
								--num_ref;
								--k;
							}

						}
					}
					XMStoreFloat3(&v.normal, XMVector3Normalize(n1));
				}
			}
		}

		void process_uvs(mesh& mesh) {
			utl::vector<vertex> old_vertices;
			old_vertices.swap(mesh.vertices);
			utl::vector<u32> old_indices(mesh.indices.size());
			old_indices.swap(mesh.indices);

			const u32 num_vertices{ static_cast<u32>(old_vertices.size()) };
			const u32 num_indices{ static_cast<u32>(old_indices.size()) };
			assert(num_indices && num_vertices);

			utl::vector <utl::vector<u32>> idx_ref(num_vertices);
			for (u32 i = 0; i < num_indices; ++i) {
				idx_ref[old_indices[i]].emplace_back(i);
			}

			for (int i = 0; i < num_indices; ++i)
			{
				auto& ref{ idx_ref[i] };
				u32 num_ref{ static_cast<u32>(ref.size()) };
				for (u32 j = 0; j < num_ref; ++j) {

					mesh.indices[ref[j]] = static_cast<u32>(mesh.vertices.size());
					vertex& v{ old_vertices[old_indices[ref[j]]]};

					v.uv = mesh.uvs[0][ref[j]];
					mesh.vertices.emplace_back(v);

					for (u32 k = j + 1; k < num_ref; ++k)
					{
						v2& uv1{ mesh.uvs[0][ref[k]] };
						if (XMScalarNearEqual(v.uv.x, uv1.x, epsilon) 
							&& XMScalarNearEqual(v.uv.y, uv1.y, epsilon)) {
							mesh.indices[ref[k]] = mesh.indices[ref[j]];
							ref.erase(ref.begin() + k);
							--num_ref;
							--k;
						}
					}

					
				}
			}

		}

		void pack_vertices_static(mesh& mesh) {
			const u32 num_vertices{ static_cast<u32>(mesh.vertices.size()) };
			assert(num_vertices);

			mesh.packed_vertices_static.reserve(num_vertices);

			for (u32 i{0}; i < num_vertices; ++i)
			{
				vertex& v{ mesh.vertices[i] };
				const u8 signs{ static_cast<u8>((v.normal.z > 0.f) << 1) };
				const u16 normal_x{ static_cast<u16>(pack_float<16>(v.normal.x, -1.f, 1.f)) };
				const u16 normal_y{ static_cast<u16>(pack_float<16>(v.normal.y, -1.f, 1.f)) };

				mesh.packed_vertices_static.emplace_back(packed_vertex::vertex_static{
					v.position,
					{0,0,0},
					signs,
					{normal_x,normal_y},
					{},
					v.uv
					});
			}
		}

		void process_vertices(mesh& mesh, geometry_import_settings settings) {
			//파이프라인 함수
			assert(mesh.raw_indices.size() % 3 == 0);
			if (settings.calculate_normals || mesh.normals.empty())
			{
				recalculate_normals(mesh);
			}

			process_normals(mesh, settings.smoothing_angle);

			if (!mesh.uvs.empty())
			{
				process_uvs(mesh);
			}
			pack_vertices_static(mesh);
		}
	}

	

	void process_scene(scene& scene, const geometry_import_settings settings) {
		for (lod_group& lod: scene.lod_group) {
			for (mesh& mesh : lod.meshes)
			{
				process_vertices(mesh, settings);
			}
		}
	}
	void pack_data(const scene& scene, scene_data& data) {
	}
}
