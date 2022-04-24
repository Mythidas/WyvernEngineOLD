#pragma once
#include <Merlin.h>

#include "EditorWindow.h"

#include <Excalibur/Windows/ViewportWindow.h>
#include <Excalibur/Windows/HierarchyWindow.h>
#include <Excalibur/Windows/PropertiesWindow.h>

namespace Merlin::Editor
{
	class EditorLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;
		virtual void OnUIRender() override;
		virtual void OnEvent(Events::Event& e) override;

		static void OpenViewportWindow();
		static void OpenHierarchyWindow();
		static void OpenPropertiesWindow();

		static ViewportCamera* GetEditorCamera() { return m_EditorCamera; }

		static Entity* GetSelectedContext() { return m_SelectedContext; }
		static void SetSelectedContext(Entity* ent) { m_SelectedContext = ent; }
	private:
		static ViewportWindow* m_ViewportWindow;
		static HierarchyWindow* m_HierarchyWindow;
		static PropertiesWindow* m_PropertiesWindow;

		static ViewportCamera* m_EditorCamera;

		static std::vector<EditorWindow*> m_Windows;

		static Entity* m_SelectedContext;
	};

	struct TestNativeComponent : public NativeScriptComponent
	{
		void OnUpdate() override;
	};

	struct Test2NativeComponent : public NativeScriptComponent
	{
		void OnUpdate() override;
	};
}