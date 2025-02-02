#pragma once
#include "ECSConfig.h"
#include "NBodyComponent.h"

namespace Reality
{
	class NBodySystem : public ECSSystem
	{
	public:
		NBodySystem();
		void Update(float deltaTime);
		float nGravity = 1.0f;
	};
}
