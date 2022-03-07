#pragma once

#include <Merlin/Window/Window.h>
#include <Merlin/Core/Layer.h>
#include <Merlin/Core/LayerStack.h>

#include <Merlin/Scene/Wizard.h>
#include <Merlin/Scene/WizardStack.h>

#include <Merlin/Core/Events/Event.h>
#include <Merlin/Core/Events/WindowEvent.h>

#include <chrono>

namespace Merlin
{
	class Application
	{
	public:
		Application();
		~Application();

		virtual void Run();

		virtual void OnEvent(Events::Event& e);

		virtual void PushLayer(Layer* layer);
		virtual void PushOverlay(Layer* overlay);

		virtual void PushWizard(Wizard* wizard);

		virtual bool OnWindowResize(Events::WindowResizeEvent& e);

		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }
	private:
		static Application* s_Instance;
		Window* m_Window;

		LayerStack m_LayerStack;
		WizardStack m_WizardStack;

		bool m_Minimized = false;

		std::chrono::steady_clock::time_point m_LastFrameTime;
		std::chrono::steady_clock::time_point m_StartTime;
	};
}