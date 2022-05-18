#include "wvpch.h"
#include "Project.h"

#include <Wyvern/Core/Scene/Scene.h>
#include <Wyvern/Core/Scene/Serializer.h>
#include <Wyvern/Core/Application/Application.h>
#include <Wyvern/Utils/FileSystem.h>

#include <yaml-cpp/yaml.h>

namespace Wyvern
{
	std::string Project::s_StartScenePath;

	void Project::LoadProject()
	{
		Utils::FileSystem projectPath = Application::GetSpecification().AssetsPath / "/project.wproj";
		YAML::Node in;

		in = YAML::Load(projectPath.ReadFile());

		if (!in["Project"])
			return;

		s_StartScenePath = in["StartScene"].as<std::string>();
	}

	void Project::SaveProject()
	{
		Utils::FileSystem projectPath = Application::GetSpecification().AssetsPath / "/project.wproj";
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "Project" << YAML::Value << "Project File";
		out << YAML::Key << "StartScene" << YAML::Value << s_StartScenePath;

		out << YAML::EndMap;

		projectPath.WriteFile(out.c_str());
	}

	void Project::SetStartScene(Ref<Scene> scene)
	{
		s_StartScenePath = scene->m_ScenePath;
	}

	Ref<Scene> Project::LoadStartScene()
	{
		Ref<Scene> scene = CreateRef<Scene>();
		Serializer::Deserialize(scene, s_StartScenePath);
		Scene::SetActiveScene(scene);
		return scene;
	}
}