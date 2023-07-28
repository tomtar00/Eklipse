#pragma once

#ifdef EK_PLATFORM_WINDOWS

extern Eklipse::Application* Eklipse::CreateApplication();

int main(int argc, char** argv)
{
	Eklipse::Log::Init();

	auto app = Eklipse::CreateApplication();
	app->Run();
	delete app;
}

#endif