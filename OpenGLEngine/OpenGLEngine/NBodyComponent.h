#pragma once
#include "ECSConfig.h"

namespace Reality
{
	struct NBodyComponent
	{
		NBodyComponent(float _mass = 1.0f)
			: mass(_mass)
		{

		}

		float mass;
	};
}
