#pragma once

#pragma warning(disable: 4530)

// C++ Çì´õ
#include <stdint.h>
#include <assert.h>
#include <typeinfo>
#include <memory>
#include <algorithm>
#include <unordered_map>

#if defined(_WIN64)
#include <DirectXMath.h>
#endif

#include "PrimitiveTypes.h"
#include "../Utilities/Math.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/MathTypes.h"
#include "Id.h"

#ifdef _DEBUG
#define DEBUG_OP(x) x
#else
#define DEBUG_OP(x) (void(0))
#endif


