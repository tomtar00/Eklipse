#pragma once
#include <string>
#include <map>

#include "Script.h"

namespace Eklipse
{
	class Entity;
}

namespace EklipseEngine
{
	namespace Reflections
	{
		struct ClassMemberReflection
		{
			String memberName;
			String memberType;
			uint32_t memberOffset;
		};
		struct ClassReflection
		{
			String className;
			std::map<String, ClassMemberReflection> members;
		};

		struct ClassInfo
		{
			Script* (*create)(Ref<Eklipse::Entity> entity);
			ClassReflection reflection;
		};
	}
}