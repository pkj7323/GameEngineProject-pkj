#pragma once
#include "CommonHeader.h"

namespace primal::platform
{
	DEFINE_TYPED_ID(window_id)
	class window {
	public:
		constexpr explicit window(window_id id) : id_{ id } {}
		constexpr window() : id_{ id::invalid_id } {}
		constexpr window_id getId() const { return id_; }
		constexpr bool is_valid() const { return id::is_valid(id_); }

		void set_fullscreen(bool is_fullscreen) const;
		bool is_fullscreen() const;
		void* handle() const;//void 포인터인 이유 : 플랫폼에 따라 다르기 때문에
		void set_caption(const wchar_t* caption) const;
		math::u32v4 size() const;
		void resize(u32 width, u32 height) const;
		u32 get_width() const;
		u32 get_height() const;
		bool is_closed() const;
	private:
		window_id id_;
	};

	
}
