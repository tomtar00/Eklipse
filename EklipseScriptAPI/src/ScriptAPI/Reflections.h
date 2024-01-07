#pragma once
#include <string>
#include <unordered_map>

#include "Script.h"

namespace EklipseEngine
{
	namespace Reflections
	{
		struct ClassMemberInfo
		{
			std::string type;
			size_t offset;
		};
		struct ClassInfo
		{
			Script* (*create)(Ref<Eklipse::Entity> entity);
			std::string baseClass;
			std::unordered_map<std::string, ClassMemberInfo> members;
		};
	}
}