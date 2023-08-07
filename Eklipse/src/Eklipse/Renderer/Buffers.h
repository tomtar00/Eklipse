#pragma once
#include <vector>
#include "Vertex.h"

namespace Eklipse
{
	class Buffer
	{
	public:
		virtual void Setup(const void* data, uint64_t size) {};
		virtual void Dispose() = 0;
	};
}