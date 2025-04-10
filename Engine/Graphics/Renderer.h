#pragma once
#include "CommonHeader.h"
#include "../Platform/Window.h"

namespace primal::graphics
{
	DEFINE_TYPED_ID(surface_id)
	class surface {
	public:
		constexpr explicit surface(surface_id id) : id_{ id } {}
		constexpr surface() = default;
		constexpr surface_id getId() const { return id_; }
		constexpr bool is_valid() const { return id::is_valid(id_); }

		
		void resize(u32 width, u32 height) const;
		u32 width() const;
		u32 height() const;
		void render() const;
	private:
		surface_id id_{ id::invalid_id };
	};
	struct render_surface {
		platform::window window{};
		surface surface{};
	};
	enum class graphics_platform : u32
	{
		direct3d12 = 0,
		vulkan = 1,
		open_gl = 2 ,
	};
	bool initialize(graphics_platform platform);
	void shutdown();
	

	surface create_surface(platform::window window);
	void remove_surface(surface_id id);
}
