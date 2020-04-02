#pragma once
#include "ECSConfig.h"

namespace Reality
{
	struct BuoyancyComponent
	{
		BuoyancyComponent(float _maxDepth = 10.0f, float _volume = 10.0f, float _waterHeight = 0.0f, float _liquidDensity = 1000.0f, Vector3 _centerOfBuoyancy = Vector3(0, 0, 0), ECSEntity _connectedEntity = ECSEntity())
			: maxDepth(_maxDepth), volume(_volume), waterHeight(_waterHeight), liquidDensity(_liquidDensity), centerOfBuoyancy(_centerOfBuoyancy), connectedEntity(_connectedEntity)
		{

		}

		float maxDepth;
		float volume;
		float waterHeight;
		float liquidDensity;
		Vector3 centerOfBuoyancy;
		ECSEntity connectedEntity;
	};
}