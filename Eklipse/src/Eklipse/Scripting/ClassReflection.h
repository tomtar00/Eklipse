#pragma once

namespace Eklipse
{
	struct ClassMemberReflection
	{
		std::string memberName;
		std::string memberType;
	};
	struct ClassReflection
	{
		std::string className;
		std::vector<ClassMemberReflection> members;
	};
}