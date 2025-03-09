#pragma once
// C++ 헤더
#include <stdint.h>
#include <assert.h>

//Common Header
#include "PrimitiveTypes.h"

// 인스턴스로 하는 방식은 cpu를 제대로 안쓸수도 있으므로
//32비트 부호없는 정수로 해서 각 엔티티의 인덱스를 부여하고
//각 엔티티가 가지고있는 컴포넌트도 인덱스를 부여한다. 없는 컴포넌트는 invalid bit로 표시
//근데 이렇게하면 캐시미스가 존나 발생하므로
//
//엔티티는 엔티티끼리 배열로 저장해놓고
//트랜스폼은 트랜스폼끼리 배열로 저장하듯이 컴포넌트를 배열로 쭉저장해놓는다 어차피 인덱스
