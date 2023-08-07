#pragma once
#include <Eklipse.h>
#include <iostream>

class EditorLayer : public Eklipse::Layer 
{
public:
	~EditorLayer() {}

	void OnAttach() override
	{
		EK_INFO("Editor layer attached");
	}
	void OnDetach() override
	{
		EK_INFO("Editor layer detached");
	}
	void Update(float deltaTime) override
	{

	}
};

class EklipseEditor : public Eklipse::Application
{
public: 
	EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
	{
		PushLayer(new EditorLayer());
	}
};

Eklipse::Application* Eklipse::CreateApplication()
{
	EK_INFO("Starting editor...");

	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1280;
	info.windowHeight = 720;

	return new EklipseEditor(info);
}