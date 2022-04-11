#pragma once

#include "Shader.h"

#include <Merlin/Core/Scene/Component.h>
#include <Merlin/Core/Math/Vector.h>

namespace Merlin
{
	struct Material
	{
		Shader* shader = nullptr;
	};
}