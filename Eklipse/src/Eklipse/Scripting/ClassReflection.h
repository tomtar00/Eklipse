#pragma once

namespace Eklipse
{
	struct ClassMemberReflection
	{
		String memberName;
		String memberType;
	};
	struct ClassReflection
	{
		String className;
		Vec<ClassMemberReflection> members;
	};
}