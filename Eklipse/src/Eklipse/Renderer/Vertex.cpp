#include "precompiled.h"
#include "Vertex.h"

namespace Eklipse
{
	bool Vertex::operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
}