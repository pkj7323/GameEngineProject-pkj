#pragma once


//나중에 stl의 벡터가아닌 내버전의 벡터를 만들것 같으므로 사용여부를 정할 수 있게 해놓는다.
#define USE_STL_VECTOR 1
#define USE_STL_DEQUE 1

#if USE_STL_VECTOR
#include <vector>
namespace primal::utl
{
	template<typename T>
	using vector = std::vector<T>;
}
#endif

#if USE_STL_DEQUE
#include <deque>
namespace primal::utl
{
	template<typename T>
	using deque = std::deque<T>;
}
#endif

namespace primal::utl
{
	//TODO: 나만의 컨테이너로 교체할 것

}
