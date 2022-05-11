#pragma once

#include <Wyvern.h>
#include <WyvernEditor.h>

using namespace Wyvern::Editor;

namespace Wyvern
{
	class PropertiesWindow : public EditorWindow
	{
	public:
		const char* GetWindowTitle() const { return "Properties"; }

		virtual void OnGUI() override;
	private:
		Entity* s_SelectedContext;

		void DrawComponents();
		void DrawComponent(const std::string label, Component* component, Entity* ent, bool isDefault = false);
	};
}