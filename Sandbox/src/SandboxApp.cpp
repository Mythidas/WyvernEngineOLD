#include "Merlin.h"

#include "ArenaExample/ArenaLayer.h"
#include "PongExample/PongLayer.h"

using namespace Merlin;
using namespace Merlin::Renderer;

class SandboxApp : public Application
{
public:
	SandboxApp()
	{
		ResourceManager::LoadShader("..\\Sandbox\\Assets\\Shader\\FlatShader.vert",
			"..\\Sandbox\\Assets\\Shader\\FlatShader.frag", nullptr, "FlatShader");

		ResourceManager::LoadShader("..\\Sandbox\\Assets\\Shader\\TextShader.vert",
			"..\\Sandbox\\Assets\\Shader\\TextShader.frag", nullptr, "TextShader");

		ResourceManager::LoadTexture("..\\Sandbox\\Assets\\Texture\\tileset.png", true, true, "Dungeon");
		ResourceManager::LoadTexture("..\\Sandbox\\Assets\\Texture\\ball.png", true, true, "Ball");

		ResourceManager::LoadSubTexture("Floor", "Dungeon", { 1,14 }, { 1,1 }, { 32,32 });
		ResourceManager::LoadSubTexture("Wall", "Dungeon", { 25,13 }, { 1,1 }, { 32,32 });
		ResourceManager::LoadSubTexture("Player", "Dungeon", { 17,6 }, { 1,1 }, { 32,32 });

		PushLayer(new PongLayer());
	}

	~SandboxApp()
	{

	}
};

int main()
{
	Scope<SandboxApp> app = std::make_unique<SandboxApp>();
	app->Run();
}