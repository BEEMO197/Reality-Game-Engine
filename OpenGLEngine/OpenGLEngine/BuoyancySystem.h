#pragma once
#include "ECSConfig.h"
#include "BuoyancyComponent.h"

namespace Reality
{
	class BuoyancySystem : public ECSSystem
	{
	public:
		BuoyancySystem();
		void Update(float deltaTime);

		bool spawning = true;
		float currentBuoyancyValue;
		float width = 2500;
		float length = 2500;
	};
}