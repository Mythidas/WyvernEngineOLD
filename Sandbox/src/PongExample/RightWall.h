#pragma once

#include "PongWall.h"

using namespace Merlin;
using namespace Merlin::Renderer;

class RightWall : public PongWall
{
public:
	RightWall();

	void OnAttach() override;
	void OnCollision2D(const Collision2D& collision) override;
};