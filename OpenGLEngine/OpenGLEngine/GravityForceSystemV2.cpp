#include "GravityForceSystemV2.h"

namespace Reality
{
	GravityForceSystemV2::GravityForceSystemV2()
	{
		requireComponent<ForceAndTorqueAccumulatorComponent>();
		requireComponent<GravityForceComponentV2>();
	}

	void GravityForceSystemV2::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto& forceAcc = e.getComponent<ForceAndTorqueAccumulatorComponent>();
			auto& gravity = e.getComponent<GravityForceComponentV2>();

			if (forceAcc.inverseMass > 0)
			{
				forceAcc.AddForce(worldGravity * gravity.gravityScale / forceAcc.inverseMass);
			}
		}
	}
}
