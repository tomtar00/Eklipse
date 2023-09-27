#pragma once

#ifdef EK_PLATFORM_WINDOWS

extern Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication();

int main(int argc, char** argv)
{
	Eklipse::Log::Init();
	Eklipse::Profiler::Init();

	EK_PROFILE_BEGIN("Startup");
	auto app = Eklipse::CreateApplication();

	app->Run();
}

#endif