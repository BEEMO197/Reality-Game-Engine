#include "BuoyancySystem.h"
#include "TransformComponentV2.h"
#include "ForceAndTorqueAccumulatorComponent.h"
#include "RigidBodyComponent.h"
#include "ParticleComponent.h"
#include "GravityForceComponent.h"

namespace Reality
{
	BuoyancySystem::BuoyancySystem()
	{
		requireComponent<BuoyancyComponent>();
	}

	void BuoyancySystem::Update(float deltaTime)
	{
		if (glfwGetKey(getWorld().data.renderUtil->window->glfwWindow, GLFW_KEY_UP) == GLFW_PRESS)
		{
			for (auto e : getEntities())
			{
				if (e.hasComponent<BuoyancyComponent>())
				{
					e.getComponent<BuoyancyComponent>().liquidDensity += 5;

					if (e.getComponent<BuoyancyComponent>().liquidDensity >= 10000)
					{
						e.getComponent<BuoyancyComponent>().liquidDensity = 10000;
					}

					currentBuoyancyValue = e.getComponent<BuoyancyComponent>().liquidDensity;
				}
			}
		}

		if (glfwGetKey(getWorld().data.renderUtil->window->glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			for (auto e : getEntities())
			{
				if (e.hasComponent<BuoyancyComponent>())
				{
					e.getComponent<BuoyancyComponent>().liquidDensity -= 5;

					if (e.getComponent<BuoyancyComponent>().liquidDensity <= 10)
					{
						e.getComponent<BuoyancyComponent>().liquidDensity = 10;
					}

					currentBuoyancyValue = e.getComponent<BuoyancyComponent>().liquidDensity;
				}
			}
		}

		for (auto e : getEntities())
		{
			auto& buoyancy = e.getComponent<BuoyancyComponent>();

			if (buoyancy.connectedEntity.hasComponent<ForceAndTorqueAccumulatorComponent>())
			{
				auto& forceAndTorqueAcc = buoyancy.connectedEntity.getComponent<ForceAndTorqueAccumulatorComponent>();

				currentBuoyancyValue = buoyancy.liquidDensity;

				if (buoyancy.connectedEntity.hasComponent<TransformComponentV2>())
				{
					auto& transform = buoyancy.connectedEntity.getComponent<TransformComponentV2>();

					Vector3 worldBuoyancyTransform = transform.LocalToWorldPosition(buoyancy.centerOfBuoyancy);

					float depth = worldBuoyancyTransform.y;

					if (DEBUG_LOG_LEVEL > 0)
					{
						getWorld().data.renderUtil->DrawCube(Vector3(0, (buoyancy.waterHeight - buoyancy.maxDepth) / 2, -50), Vector3(width * 2, buoyancy.maxDepth, length * 2), Quaternion(Vector3(1, 1, 1), Vector3(1, 1, 1)), Color::Blue);
					}

					if (depth >= buoyancy.waterHeight || (worldBuoyancyTransform.x >= width * 2 || worldBuoyancyTransform.x <= -width * 2) || (worldBuoyancyTransform.z >= length * 2 || worldBuoyancyTransform.z <= -length * 2))
					{

					}
					else
					{
						Vector3 force(0, 0, 0);

						if (depth <= buoyancy.waterHeight - buoyancy.maxDepth)
						{
							force.y = (buoyancy.liquidDensity * buoyancy.volume) / 10;
							cout << force.y << endl;

							forceAndTorqueAcc.AddForceAtPoint(force, worldBuoyancyTransform, transform.GetPosition());

							if (DEBUG_LOG_LEVEL > 0)
							{
								getWorld().data.renderUtil->DrawLine(Vector3(worldBuoyancyTransform.x, buoyancy.waterHeight, worldBuoyancyTransform.z), worldBuoyancyTransform, Color::Red);
							}
						}
						else
						{
							float scale = depth + buoyancy.maxDepth <= 1 ? 1 : depth + buoyancy.maxDepth;

							force.y = (buoyancy.liquidDensity / (scale - buoyancy.waterHeight) * buoyancy.volume) / 10;
							cout << force.y << endl;

							forceAndTorqueAcc.AddForceAtPoint(force, worldBuoyancyTransform, transform.GetPosition());

							if (DEBUG_LOG_LEVEL > 0)
							{
								getWorld().data.renderUtil->DrawLine(Vector3(worldBuoyancyTransform.x, buoyancy.waterHeight, worldBuoyancyTransform.z), worldBuoyancyTransform, Color::Green);
							}
						}

						if (buoyancy.connectedEntity.hasComponent<RigidbodyComponent>())
						{
							
						}
					}
				}
			}
		}
	}
}