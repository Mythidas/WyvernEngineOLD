
#pragma once

#include "RigidBody2D.h"
#include "Collider2D.h"
#include "CollisionSolver2D.h"
#include "PhysicsSolver2D.h"

#include "Impulse2D.h"
#include "PushSolver2D.h"

#include <Merlin/Core/Scene/Wizard.h>

namespace Merlin
{
	class Physics2DWizard : public Wizard
	{
	public:
		Physics2DWizard()
		{
			AddSolver(new Impulse2D());
			AddSolver(new PushSolver2D());
		}

		void OnUpdate(Timestep ts) const override;

		void AddSolver(CollisionSolver2D* solver);
		void AddSolver(PhysicsSolver2D* solver);
	private:
		std::vector<CollisionSolver2D*> m_CollisionSolvers;
		std::vector<PhysicsSolver2D*> m_PhysicsSolvers;

		static bool CheckCollision(
			const BoxCollider2D* boxCollider,
			const Transform* transform,
			const BoxCollider2D* otherCollider,
			const Transform* otherTransform);
	};
}