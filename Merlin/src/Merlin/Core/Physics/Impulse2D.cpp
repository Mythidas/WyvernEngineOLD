#include "mlpch.h"
#include "Impulse2D.h"

#include "RigidBody2D.h"

namespace Merlin
{
	void Impulse2D::Solve(const std::vector<Collision2D*> collisions)
	{
		for (Collision2D* collision : collisions)
		{
			RigidBody2D* rbA = collision->entityA->GetComponent<RigidBody2D>();
			RigidBody2D* rbB = collision->entityB->GetComponent<RigidBody2D>();

			Vector2 rv = rbB->velocity - rbA->velocity;

			float velN = Vector2::Dot(rv, collision->normal);

			if (velN > 0)
				return;

			float e = fmin(rbA->bounce, rbB->bounce);

			float aInvMass = rbA->mass > 0 ? rbA->invMass : 0;
			float bInvMass = rbB->mass > 0 ? rbB->invMass : 0;

			float j = -(1 + e) * velN;
			j /= aInvMass + bInvMass;

			Vector2 impulse = collision->normal * j;
			rbA->velocity -= impulse * aInvMass;
			rbB->velocity += impulse * bInvMass;
		}
	}
}