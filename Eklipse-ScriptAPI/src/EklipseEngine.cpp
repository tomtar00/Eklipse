#include "EklipseEngine.h"
#include <Eklipse.h>

#include <spdlog/sinks/stdout_color_sinks.h>

using namespace Eklipse;

namespace EklipseEngine
{
    static std::shared_ptr<spdlog::logger> logger;
    void Log(const char* message)
    {
        if (!logger)
        {
            logger = spdlog::stdout_color_mt("TEST");
        }
		
        logger->info(message);

	    //EK_INFO(message);
    }
}


//Transform& EklipseEngine::Script::GetTransform()
//{
//	auto& entt = Application::Get().GetScene()->GetRegistry().get<entt::entity>(m_entityHandle);
//	return entt.GetComponent<Transform>().transform;
//}