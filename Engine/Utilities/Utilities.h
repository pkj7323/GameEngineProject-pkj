#pragma once


//나중에 stl의 벡터가아닌 내버전의 벡터를 만들것 같으므로 사용여부를 정할 수 있게 해놓는다.
#define USE_STL_VECTOR 1
#define USE_STL_DEQUE 1
#include <algorithm>

#if USE_STL_VECTOR
#include <vector>
namespace primal::utl
{
	template<typename T>
	using vector = std::vector<T>;

	/*인덱스요소를 마지막 요소와 바꾼다음 맨뒤 삭제*/
	template<typename T>
	void erase_unordered(std::vector<T>& v,size_t index)
	{
		if (v.size() > 1)
		{
			std::iter_swap(v.begin() + index, v.end() - 1);
			v.pop_back();
		}
		else
		{
			v.clear();
		}
	}
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
