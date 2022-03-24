#include "PongLayer.h"

#include "PongWall.h"
#include "LeftWall.h"

#include <imgui.h>

using namespace Merlin;
using namespace Merlin::Renderer;

PongLayer::PongLayer()
	: Layer("Arena Layer"), fps(0)
{
}

void PongLayer::OnAttach()
{
	Entity* camera = Entity::CreateEntity();
	Camera* cam = camera->AddComponent<Camera>();
	cam->offset = { 0,0,8 };
	cam->transform = camera->GetTransform();

	PongWall* wall1 = Scene::CreateEntity<PongWall>();
	wall1->GetTransform()->position = { 4, 0, 0 };
	wall1->GetTransform()->scale = { 0.125f, 4, 1 };
	wall1->col->size = { 0.125f, 4 };

	LeftWall* left = Scene::CreateEntity<LeftWall>();

	PongWall* wall3 = Scene::CreateEntity<PongWall>();
	wall3->GetTransform()->position = { 0, 4, 0 };
	wall3->GetTransform()->scale = { 4.125f, 0.125f, 1 };
	wall3->col->size = { 4.125f, 0.125f };

	PongWall* wall4 = Scene::CreateEntity<PongWall>();
	wall4->GetTransform()->position = { 0, -4, 0 };
	wall4->GetTransform()->scale = { 4.125f, 0.125f, 1 };
	wall4->col->size = { 4.125f, 0.125f };

	Entity* player1 = Entity::CreateEntity();
	Material2D* matp1 = player1->AddComponent<Material2D>();
	matp1->shader = ResourceManager::GetShader("FlatShader");
	matp1->color = { 1,1,1,1 };
	player1->GetTransform()->position = { 3.5f, 0, 0 };
	player1->GetTransform()->scale = { 0.125f, 0.5f, 1 };

	Entity* player2 = Entity::CreateEntity();
	Material2D* matp2 = player2->AddComponent<Material2D>();
	matp2->shader = ResourceManager::GetShader("FlatShader");
	matp2->color = { 1,1,1,1 };
	player2->GetTransform()->position = { -3.5f, 0, 0 };
	player2->GetTransform()->scale = { 0.125f, 0.5f, 1 };
	BoxCollider2D* player2Col = player2->AddComponent<BoxCollider2D>();
	player2Col->size = player2->GetTransform()->scale;
	RigidBody2D* player2Rb = player2->AddComponent<RigidBody2D>();
	player2Rb->bodyType = RigidBody2D::PhysicsBody::Static;
	player2Rb->bounce = 1.0f;
	player2Rb->mass = 0;

	Entity* ball = Entity::CreateEntity();
	Material2D* matb = ball->AddComponent<Material2D>();
	matb->shader = ResourceManager::GetShader("FlatShader");
	matb->texture = ResourceManager::GetTexture("Ball");
	matb->color = { 1,1,1,1 };
	ball->GetTransform()->scale = { 0.25f, 0.25f, 1 };
	SphereCollider2D* ballCol = ball->AddComponent<SphereCollider2D>();
	ballCol->radius = 0.25f;
	RigidBody2D* ballRb = ball->AddComponent<RigidBody2D>();
	ballRb->bounce = 1.0f;
	ballRb->drag = 0;
	ballRb->force += {-90, 5};
}

void PongLayer::OnUpdate(Merlin::Timestep ts)
{
	fps = ts.GetFPS();
}

void PongLayer::OnImGuiRender()
{
	ImGui::Begin("Engine Info");

	std::string fpsText("FPS: ");
	fpsText.append(std::to_string(fps).c_str());
	ImGui::Text(fpsText.c_str());

	std::string entText("Entity Count: ");
	entText.append(std::to_string(Scene::GetEntityCount()).c_str());
	ImGui::Text(entText.c_str());

	ImGui::End();
}