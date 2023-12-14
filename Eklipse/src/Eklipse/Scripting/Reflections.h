#pragma once
#include "Script.h"

namespace Eklipse
{
	struct ClassMemberInfo
	{
		std::string type;
		size_t offset;
	};
	struct ClassInfo
	{
		Script* (*create)(Eklipse::Entity entity);
		std::string baseClass;
		std::unordered_map<std::string, ClassMemberInfo> members;
	};
}