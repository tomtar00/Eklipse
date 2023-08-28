#include "precompiled.h"
#include "VKShader.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKShader::VKShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
		{
			m_name = name;
		}
		void VKShader::Bind() const
		{
		}
		void VKShader::Unbind() const
		{
		}
		void VKShader::Dispose() const
		{
		}
		void VKShader::UploadMat4(const std::string& name, const glm::mat4& matrix)
		{
		}
	}
}
