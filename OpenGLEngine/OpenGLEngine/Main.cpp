//#define STB_IMAGE_IMPLEMENTATION
#include "RenderingSystem.h"
#include "RenderingSystemV2.h"
#include "InputEventSystem.h"
#include "FPSControlSystem.h"
#include "RotateSystem.h"
#include "RotateSystemV2.h"
#include "FireworksSystem.h"
#include "GravityForceSystem.h"
#include "DragForceSystem.h"
#include "FixedSpringSystem.h"
#include "PairedSpringSystem.h"
#include "ParticleSphereSystem.h"
#include "CableSystem.h"
#include "RodSystem.h"
#include "PlaneSystem.h"
#include "ParticleContactResolutionSystem.h"
#include "ResetPenetrationDeltaMoveSystem.h"
#include "ForceAccumulatorSystem.h"
#include "ParticleSystem.h"
#include "DynamicDirectionalLightSystem.h"
#include "DynamicPointLightSystem.h"
#include "DynamicSpotLightSystem.h"
#include <string>
#include <stdlib.h>     
#include <time.h>      

using namespace Reality;

void LoadShaders(ECSWorld& world);
void LoadModels(ECSWorld& world);
void SetupLights(ECSWorld& world);
void MakeABunchaObjects(ECSWorld& world);
void MakeFireworks(ECSWorld& world);
void Make3Particles(ECSWorld& world);
void MakeABunchaSprings(ECSWorld& world);
void MakeABunchaSpheres(ECSWorld& world);
void MakeABunchaCablesAndRods(ECSWorld& world);
void MakeARopeBridge(ECSWorld& world);
void MakeABunchaObjectsV2(ECSWorld& world);

int main()
{
	ECSWorld world;

	// Init and Load
	world.data.InitRendering();
	//LoadAssets(world);
	
	world.data.renderUtil->camera.Position = Vector3(0, 0.0f, 50.0f);
	world.data.renderUtil->SetFOV(60);
	// Create entities

	// Make a player controller
	auto e = world.createEntity();
	e.addComponent<FPSControlComponent>();

	SetupLights(world);
	//MakeABunchaObjects(world);
	//MakeFireworks(world);
	//Make3Particles(world);
	//MakeABunchaSprings(world);
	//MakeABunchaSpheres(world);
	//MakeABunchaCablesAndRods(world);
	MakeARopeBridge(world);
	//MakeABunchaObjectsV2(world);

	// Create Systems
	world.getSystemManager().addSystem<RenderingSystem>();
	world.getSystemManager().addSystem<RenderingSystemV2>();
	world.getSystemManager().addSystem<InputEventSystem>();
	world.getSystemManager().addSystem<FPSControlSystem>();
	world.getSystemManager().addSystem<RotateSystem>();
	world.getSystemManager().addSystem<RotateSystemV2>();
	world.getSystemManager().addSystem<FireworksSystem>();
	world.getSystemManager().addSystem<GravityForceSystem>();
	world.getSystemManager().addSystem<DragForceSystem>();
	world.getSystemManager().addSystem<FixedSpringSystem>();
	world.getSystemManager().addSystem<PairedSpringSystem>();
	world.getSystemManager().addSystem<ParticleSphereSystem>();
	world.getSystemManager().addSystem<CableSystem>();
	world.getSystemManager().addSystem<RodSystem>();
	world.getSystemManager().addSystem<PlaneSystem>();
	world.getSystemManager().addSystem<ParticleContactResolutionSystem>();
	world.getSystemManager().addSystem<ResetPenetrationDeltaMoveSystem>();
	world.getSystemManager().addSystem<ForceAccumulatorSystem>();
	world.getSystemManager().addSystem<ParticleSystem>();
	world.getSystemManager().addSystem<DynamicDirectionalLightSystem>();
	world.getSystemManager().addSystem<DynamicPointLightSystem>();
	world.getSystemManager().addSystem<DynamicSpotLightSystem>();

	float time = glfwGetTime();
	float stepTime = glfwGetTime();
	float deltaTime = 0;
	float elapsedDeltaTime = 0;
	float logicDelta = 0;
	float debugDelta = 0;

	LoadShaders(world);
	bool shadersLoaded = false;
	bool modelsLoadStarted = false;
	// game loop
	// -----------
	while (!glfwWindowShouldClose(world.data.renderUtil->window->glfwWindow))
	{
		float current = glfwGetTime();
		deltaTime = current - time;
		deltaTime = 1 / 60.0f;
		time = glfwGetTime();

		world.update();

		// Poll OpenGl events
		glfwPollEvents();

		world.data.renderUtil->ClearDisplay(world.data.renderUtil->window->glfwWindow);

		// Load
		if (!shadersLoaded)
		{
			shadersLoaded = world.data.assetLoader->ShadersLoaded();
		}
		if(shadersLoaded && !modelsLoadStarted)
		{
			LoadModels(world);
			modelsLoadStarted = true;
		}
		// Update View
		world.data.renderUtil->UpdateViewMatrix();
		// Process Input
		world.getSystemManager().getSystem<InputEventSystem>().Update(deltaTime);

		// Game Logic Update
		world.getSystemManager().getSystem<FPSControlSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RotateSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RotateSystemV2>().Update(deltaTime);
		world.getSystemManager().getSystem<FireworksSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<ParticleSphereSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<CableSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RodSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<PlaneSystem>().Update(deltaTime);

		// Update Transform

		// Physics
		//float fixedDeltaTime = glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS ? 1 / 60.0f : 0;		
		float fixedDeltaTime = 1 / 60.0f;
		// Force Generator
		world.getSystemManager().getSystem<GravityForceSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<DragForceSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<FixedSpringSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<PairedSpringSystem>().Update(fixedDeltaTime);

		// Force Accumulator
		world.getSystemManager().getSystem<ForceAccumulatorSystem>().Update(fixedDeltaTime);

		// Contact Resolution
		world.getSystemManager().getSystem<ParticleContactResolutionSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ResetPenetrationDeltaMoveSystem>().Update(fixedDeltaTime);

		// Integrator
		world.getSystemManager().getSystem<ParticleSystem>().Update(fixedDeltaTime);

		// Rendering Update
		///*** HACK: For the last DrawCall not working on some systems
		world.data.renderUtil->DrawCube(Vector3(0, 0, 0), Vector3(0, 0, 0));
		///*** HACK: For the last DrawCall not working on some systems
		world.getSystemManager().getSystem<DynamicDirectionalLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicPointLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicSpotLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RenderingSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RenderingSystemV2>().Update(deltaTime);

		elapsedDeltaTime = glfwGetTime() - time;
		logicDelta = elapsedDeltaTime - world.data.renderUtil->GetRenderDelta();
		stepTime = glfwGetTime();

		// Debug
		if (DEBUG_LOG_LEVEL > 0)
		{
			world.data.renderUtil->RenderText("FPS : " + std::to_string((int)round(1.0f / deltaTime)), 1810.0f, 1060.0f, 0.5f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 1)
		{
			int logic = (int)round(logicDelta * 100.0f / deltaTime);
			std::string logicString = logic < 10 ? " " + std::to_string(logic) : std::to_string(logic);
			int render = (int)round(world.data.renderUtil->GetRenderDelta() * 100.0f / deltaTime);
			std::string renderString = logic < 10 ? " " + std::to_string(render) : std::to_string(render);
			int debug = (int)round(debugDelta * 100.0f / deltaTime);
			std::string debugString = logic < 10 ? " " + std::to_string(debug) : std::to_string(debug);
			
			world.data.renderUtil->RenderText("Logic : " + logicString + "%" +
				//+ " | Physics : " + std::to_string((int)round(physicsDelta * 100.0f / deltaTime)) + "%" +
				+ " | Rendering : " + renderString + "%" +
				+ " | Debug : " + debugString + "%"
				, 1680.0f, 1040.0f, 0.25f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 2)
		{
			world.data.renderUtil->RenderText("Draw Calls : " + std::to_string(world.data.renderUtil->GetDrawCalls())
				+ " | Verts : " + std::to_string(world.data.renderUtil->GetVerts())
				+ " | Tris : " + std::to_string(world.data.renderUtil->GetTris())
				+ " | Lines : " + std::to_string(world.data.renderUtil->GetLines())
				, 1610.0f, 1020.0f, 0.25f, Color(0, 1, 1, 1));
		}

		// Update debug delta
		debugDelta = glfwGetTime() - stepTime;
		stepTime = glfwGetTime();

		world.data.renderUtil->SwapBuffers(world.data.renderUtil->window->glfwWindow);

		// Show FPS in console
		//std::cout << "FPS : " << 1.0f / deltaTime << std::endl;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void LoadShaders(ECSWorld& world)
{
	world.data.assetLoader->StartShaderLoading({ {"Shaders/Lighting_Maps.vs", "Shaders/Lighting_Maps.fs"} });
}
void LoadModels(ECSWorld& world)
{
	world.data.assetLoader->StartModelLoading({
		//ModelData("Resources/Models/snowy-mountain-terrain/SnowyMountainMesh.obj"),
		ModelData("Resources/Models/Sponza-master/sponza.obj"),
		ModelData("Resources/Models/nanosuit/nanosuit.obj"),
		ModelData("Resources/Models/supermarine-spitfire/spitfire.fbx",
			{{"spitfire_d.png"}})
		});
}

void MakeABunchaObjects(ECSWorld& world)
{
	auto castle = world.createEntity();
	castle.addComponent<TransformComponent>(Vector3(0, -3.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0, 270, 0));
	// Add mesh
	castle.addComponent<ModelComponent>("Resources/Models/Sponza-master/sponza.obj");

	//auto flight = world.createEntity();
	//flight.addComponent<TransformComponent>(Vector3(0, 30, -50), Vector3(0.1f, 0.1f, 0.1f), Vector3(270, 0, 0));
	//// Add mesh
	//flight.addComponent<ModelComponent>("Resources/Models/supermarine-spitfire/spitfire.fbx");
	//flight.addComponent<RotateComponent>(Vector3(0, 90, 0));
	//flight.addComponent<ParticleComponent>(Vector3(0, 30, 0));
	//flight.addComponent<ForceAccumulatorComponent>();
	//flight.addComponent<GravityForceComponent>();

}

void MakeFireworks(ECSWorld & world)
{
	for (int i = 0; i < 3; i++)
	{
		auto fireworks = world.createEntity();
		fireworks.addComponent<TransformComponent>(Vector3(-100 + 100 * i, 30 + RANDOM_FLOAT(-10, 10), -50));
		fireworks.addComponent<ParticleComponent>(Vector3(0, 100, 0));
		fireworks.addComponent<ForceAccumulatorComponent>();
		fireworks.addComponent<GravityForceComponent>();
		fireworks.addComponent<FireworksComponent>(6, 3, 3 + RANDOM_FLOAT(-1, 1));
	}
	
}

void Make3Particles(ECSWorld & world)
{
	auto particle1 = world.createEntity();
	particle1.addComponent<TransformComponent>(Vector3(-10, 60, -50));
	particle1.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	particle1.addComponent<ForceAccumulatorComponent>();
	particle1.addComponent<GravityForceComponent>();
	particle1.addComponent<DragForceComponent>(0, 0);

	auto particle2 = world.createEntity();
	particle2.addComponent<TransformComponent>(Vector3(0, 60, -50));
	particle2.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	particle2.addComponent<ForceAccumulatorComponent>();
	particle2.addComponent<GravityForceComponent>();
	particle2.addComponent<DragForceComponent>(1, 0);

	auto particle3 = world.createEntity();
	particle3.addComponent<TransformComponent>(Vector3(10, 60, -50));
	particle3.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	particle3.addComponent<ForceAccumulatorComponent>();
	particle3.addComponent<GravityForceComponent>();
	particle3.addComponent<DragForceComponent>(1, 1);
}

void MakeABunchaSprings(ECSWorld & world)
{
	auto particle1 = world.createEntity();
	particle1.addComponent<TransformComponent>(Vector3(0, 20, -50));
	particle1.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	particle1.addComponent<ForceAccumulatorComponent>();
	particle1.addComponent<GravityForceComponent>();

	auto particle2= world.createEntity();
	particle2.addComponent<TransformComponent>(Vector3(-10, 0, -50));
	particle2.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	particle2.addComponent<ForceAccumulatorComponent>();
	particle2.addComponent<GravityForceComponent>();

	auto spring1 = world.createEntity();
	spring1.addComponent<TransformComponent>(Vector3(10, 60, -50));
	spring1.addComponent<FixedSpringComponent>(20.0f, 20.0f, particle1);

	auto spring2 = world.createEntity();
	spring2.addComponent<TransformComponent>(Vector3(-10, 60, -50));
	spring2.addComponent<FixedSpringComponent>(20.0f, 15.0f, particle1);

	auto pairedSpring = world.createEntity();
	pairedSpring.addComponent<PairedSpringComponent>(20.0f, 20.0f, particle1, particle2);

}

void MakeABunchaSpheres(ECSWorld & world)
{
	for (int i = 0; i < 40; i++)
	{
		auto sphere = world.createEntity();
		sphere.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 10)));
		sphere.addComponent<ParticleComponent>(Vector3(RANDOM_FLOAT(-40, 40), RANDOM_FLOAT(-40, 40), RANDOM_FLOAT(-40, 40)));
		sphere.addComponent<ForceAccumulatorComponent>(1.0f);
		sphere.addComponent<GravityForceComponent>();
		sphere.addComponent<ParticleSphereComponent>(RANDOM_FLOAT(1, 3));
	}
}

void CreateParticleArchetype(ECSEntity e)
{
	e.addComponent<ParticleComponent>();
	e.addComponent<ForceAccumulatorComponent>();
	e.addComponent<GravityForceComponent>();
	//e.addComponent<ParticleSphereComponent>();
	e.addComponent<PenetrationDeltaMoveComponent>();
}

void MakeARopeBridge(ECSWorld & world)
{
	auto ePivot1 = world.createEntity();
	ePivot1.addComponent<TransformComponent>(Vector3(3, 10, 5));
	
	auto e1 = world.createEntity();
	e1.addComponent<TransformComponent>(Vector3(0, 3, 5));
	CreateParticleArchetype(e1);
	
	auto ePivot2 = world.createEntity();
	ePivot2.addComponent<TransformComponent>(Vector3(3, 10, -5));
	
	auto e2 = world.createEntity();
	e2.addComponent<TransformComponent>(Vector3(0, 3, -5));
	CreateParticleArchetype(e2);
	
	auto rod1 = world.createEntity();
	rod1.addComponent<RodComponent>(e1, e2, 10);
	
	auto cable1 = world.createEntity();
	cable1.addComponent<CableComponent>(ePivot1, e1, 18, 0.5);
	
	auto cable2 = world.createEntity();
	cable2.addComponent<CableComponent>(ePivot2, e2, 18, 0.5);
	
	// 2
	auto ePivot3 = world.createEntity();
	ePivot3.addComponent<TransformComponent>(Vector3(3 + 10, 10, 5));
	
	auto e3 = world.createEntity();
	e3.addComponent<TransformComponent>(Vector3(0 + 10, 0, 5));
	CreateParticleArchetype(e3);
	
	auto ePivot4 = world.createEntity();
	ePivot4.addComponent<TransformComponent>(Vector3(3 + 10, 10, -5));
	
	auto e4 = world.createEntity();
	e4.addComponent<TransformComponent>(Vector3(0 + 10, 0, -5));
	CreateParticleArchetype(e4);
	
	auto rod2 = world.createEntity();
	rod2.addComponent<RodComponent>(e3, e4, 10);
	
	auto cable3 = world.createEntity();
	cable3.addComponent<CableComponent>(ePivot3, e3, 15, 0.5);
	
	auto cable4 = world.createEntity();
	cable4.addComponent<CableComponent>(ePivot4, e4, 15, 0.5);
	
	auto sphere1 = world.createEntity();
	sphere1.addComponent<TransformComponent>(Vector3(5, 10, -4));
	sphere1.addComponent<ParticleComponent>(Vector3(0, 0, 0));
	sphere1.addComponent<ForceAccumulatorComponent>(1.0f);
	sphere1.addComponent<GravityForceComponent>();
	sphere1.addComponent<ParticleSphereComponent>(1.0f);

	auto plane1 = world.createEntity();
	plane1.addComponent<PlaneComponent>(e1, e2, e3, e4);
	
	// 3
	auto ePivot5 = world.createEntity();
	ePivot5.addComponent<TransformComponent>(Vector3(3 - 10, 10, 5));
	
	auto e5 = world.createEntity();
	e5.addComponent<TransformComponent>(Vector3(0 - 10, -3, 5));
	CreateParticleArchetype(e5);
	
	auto ePivot6 = world.createEntity();
	ePivot6.addComponent<TransformComponent>(Vector3(3 - 10, 10, -5));
	
	auto e6 = world.createEntity();
	e6.addComponent<TransformComponent>(Vector3(0 - 10, -3, -5));
	CreateParticleArchetype(e6);
	
	auto rod3 = world.createEntity();
	rod3.addComponent<RodComponent>(e5, e6, 10);
	
	auto cable5 = world.createEntity();
	cable5.addComponent<CableComponent>(ePivot5, e5, 21, 0.5);
	
	auto cable6 = world.createEntity();
	cable6.addComponent<CableComponent>(ePivot6, e6, 21, 0.5);
	
	auto plane2 = world.createEntity();
	plane2.addComponent<PlaneComponent>(e5, e6, e1, e2);

	// 
	auto ePivot7 = world.createEntity();
	ePivot7.addComponent<TransformComponent>(Vector3(3 - 20, 10, 5));

	auto e7 = world.createEntity();
	e7.addComponent<TransformComponent>(Vector3(0 - 20, -5, 5));
	CreateParticleArchetype(e7);

	auto ePivot8 = world.createEntity();
	ePivot8.addComponent<TransformComponent>(Vector3(3 - 20, 10, -5));

	auto e8 = world.createEntity();
	e8.addComponent<TransformComponent>(Vector3(0 - 20, -5, -5));
	CreateParticleArchetype(e8);

	auto rod4 = world.createEntity();
	rod4.addComponent<RodComponent>(e7, e8, 10);

	auto cable7 = world.createEntity();
	cable7.addComponent<CableComponent>(ePivot7, e7, 23, 0.5);

	auto cable8 = world.createEntity();
	cable8.addComponent<CableComponent>(ePivot8, e8, 23, 0.5);

	auto plane3 = world.createEntity();
	plane3.addComponent<PlaneComponent>(e7, e8, e5, e6);

	//

	// 
	auto ePivot9 = world.createEntity();
	ePivot9.addComponent<TransformComponent>(Vector3(3 - 30, 10, 5));

	auto e9 = world.createEntity();
	e9.addComponent<TransformComponent>(Vector3(0 - 30, -3, 5));
	CreateParticleArchetype(e9);

	auto ePivot10 = world.createEntity();
	ePivot10.addComponent<TransformComponent>(Vector3(3 - 30, 10, -5));

	auto e10 = world.createEntity();
	e10.addComponent<TransformComponent>(Vector3(0 - 30, -3, -5));
	CreateParticleArchetype(e10);

	auto rod5 = world.createEntity();
	rod5.addComponent<RodComponent>(e9, e10, 10);

	auto cable9 = world.createEntity();
	cable9.addComponent<CableComponent>(ePivot9, e9, 21, 0.5);

	auto cable10 = world.createEntity();
	cable10.addComponent<CableComponent>(ePivot10, e10, 21, 0.5);

	auto plane4 = world.createEntity();
	plane4.addComponent<PlaneComponent>(e9, e10, e7, e8);

	//

	// 
	auto ePivot11 = world.createEntity();
	ePivot11.addComponent<TransformComponent>(Vector3(3 - 40, 10, 5));

	auto e11 = world.createEntity();
	e11.addComponent<TransformComponent>(Vector3(0 - 40, 0, 5));
	CreateParticleArchetype(e11);

	auto ePivot12 = world.createEntity();
	ePivot12.addComponent<TransformComponent>(Vector3(3 - 40, 10, -5));

	auto e12 = world.createEntity();
	e12.addComponent<TransformComponent>(Vector3(0 - 40, 0, -5));
	CreateParticleArchetype(e12);
	
	auto rod6 = world.createEntity();
	rod5.addComponent<RodComponent>(e11, e12, 10);

	auto cable11 = world.createEntity();
	cable11.addComponent<CableComponent>(ePivot11, e11, 18, 0.5);

	auto cable12 = world.createEntity();
	cable12.addComponent<CableComponent>(ePivot12, e12, 18, 0.5);

	auto plane5 = world.createEntity();
	plane5.addComponent<PlaneComponent>(e11, e12, e9, e10);

	//

	// 
	auto ePivot13 = world.createEntity();
	ePivot13.addComponent<TransformComponent>(Vector3(3 - 50, 10, 5));

	auto e13 = world.createEntity();
	e13.addComponent<TransformComponent>(Vector3(0 - 50, 3, 5));
	CreateParticleArchetype(e13);

	auto ePivot14 = world.createEntity();
	ePivot14.addComponent<TransformComponent>(Vector3(3 - 50, 10, -5));

	auto e14 = world.createEntity();
	e14.addComponent<TransformComponent>(Vector3(0 - 50, 3, -5));
	CreateParticleArchetype(e14);

	auto rod7 = world.createEntity();
	rod5.addComponent<RodComponent>(e13, e14, 10);

	auto cable13 = world.createEntity();
	cable13.addComponent<CableComponent>(ePivot13, e13, 15, 0.5);

	auto cable14 = world.createEntity();
	cable14.addComponent<CableComponent>(ePivot14, e14, 15, 0.5);

	auto plane6 = world.createEntity();
	plane6.addComponent<PlaneComponent>(e13, e14, e11, e12);

	//

	// rods
	auto rod8 = world.createEntity();
	rod8.addComponent<RodComponent>(e1, e3, 10);
	auto rod9 = world.createEntity();
	rod9.addComponent<RodComponent>(e2, e4, 10);
	auto rod10 = world.createEntity();
	rod10.addComponent<RodComponent>(e5, e1, 10);
	auto rod11 = world.createEntity();
	rod11.addComponent<RodComponent>(e6, e2, 10);
	
	auto rod12 = world.createEntity();
	rod12.addComponent<RodComponent>(e7, e5, 10);
	auto rod13 = world.createEntity();
	rod13.addComponent<RodComponent>(e8, e6, 10);
	auto rod14 = world.createEntity();
	rod14.addComponent<RodComponent>(e9, e7, 10);
	auto rod15 = world.createEntity();
	rod15.addComponent<RodComponent>(e10, e8, 10);

	auto rod16 = world.createEntity();
	rod16.addComponent<RodComponent>(e11, e9, 10);
	auto rod17 = world.createEntity();
	rod17.addComponent<RodComponent>(e12, e10, 10);
	auto rod18 = world.createEntity();
	rod18.addComponent<RodComponent>(e13, e11, 10);
	auto rod19 = world.createEntity();
	rod19.addComponent<RodComponent>(e14, e12, 10);

	// diagonal rods
	auto rod20 = world.createEntity();
	rod20.addComponent<RodComponent>(e1, e4, 10 * pow(2.0f, 0.5f));
	auto rod21 = world.createEntity();
	rod21.addComponent<RodComponent>(e2, e3, 10 * pow(2.0f, 0.5f));
	auto rod22 = world.createEntity();
	rod22.addComponent<RodComponent>(e6, e1, 10 * pow(2.0f, 0.5f));
	auto rod23 = world.createEntity();
	rod23.addComponent<RodComponent>(e5, e2, 10 * pow(2.0f, 0.5f));

	auto rod24 = world.createEntity();
	rod24.addComponent<RodComponent>(e5, e8, 10 * pow(2.0f, 0.5f));
	auto rod25 = world.createEntity();
	rod25.addComponent<RodComponent>(e6, e7, 10 * pow(2.0f, 0.5f));
	auto rod26 = world.createEntity();
	rod26.addComponent<RodComponent>(e7, e10, 10 * pow(2.0f, 0.5f));
	auto rod27 = world.createEntity();
	rod27.addComponent<RodComponent>(e8, e9, 10 * pow(2.0f, 0.5f));

	auto rod28 = world.createEntity();
	rod28.addComponent<RodComponent>(e9, e12, 10 * pow(2.0f, 0.5f));
	auto rod29 = world.createEntity();
	rod29.addComponent<RodComponent>(e10, e11, 10 * pow(2.0f, 0.5f));
	auto rod30 = world.createEntity();
	rod30.addComponent<RodComponent>(e11, e14, 10 * pow(2.0f, 0.5f));
	auto rod31 = world.createEntity();
	rod31.addComponent<RodComponent>(e12, e13, 10 * pow(2.0f, 0.5f));
}

void MakeABunchaObjectsV2(ECSWorld & world)
{
	//auto flightV1 = world.createEntity();
	//flightV1.addComponent<TransformComponent>(Vector3(0, 30, -50), Vector3(0.1f, 0.1f, 0.1f), Vector3(270, 0, 0));
	//// Add mesh
	//flightV1.addComponent<ModelComponent>("Resources/Models/supermarine-spitfire/spitfire.fbx");
	//flightV1.addComponent<RotateComponent>(Vector3(0, 90, 0));

	auto flightV2 = world.createEntity();
	flightV2.addComponent<TransformComponentV2>(Vector3(0, 30, -50), Vector3(0.1f, 0.1f, 0.1f), Vector3(270, 0, 0));
	// Add mesh
	flightV2.addComponent<ModelComponent>("Resources/Models/supermarine-spitfire/spitfire.fbx");
	flightV2.addComponent<RotateComponentV2>(Vector3(0, 45, 0));

}

void MakeABunchaCablesAndRods(ECSWorld & world)
{
	auto ePivot = world.createEntity();
	ePivot.addComponent<TransformComponent>(Vector3(3, 10, 0));

	auto e1 = world.createEntity();
	e1.addComponent<TransformComponent>(Vector3(0, 10, 0));
	e1.addComponent<ParticleComponent>();
	e1.addComponent<ForceAccumulatorComponent>();
	e1.addComponent<GravityForceComponent>();
	e1.addComponent<ParticleSphereComponent>();
	e1.addComponent<PenetrationDeltaMoveComponent>();

	auto e2 = world.createEntity();
	e2.addComponent<TransformComponent>(Vector3(5, 5, 0));
	e2.addComponent<ParticleComponent>();
	e2.addComponent<ForceAccumulatorComponent>();
	e2.addComponent<GravityForceComponent>();
	e2.addComponent<ParticleSphereComponent>();
	e2.addComponent<PenetrationDeltaMoveComponent>();

	auto e3 = world.createEntity();
	e3.addComponent<TransformComponent>(Vector3(0, 0, 0));
	e3.addComponent<ParticleComponent>();
	e3.addComponent<ForceAccumulatorComponent>();
	e3.addComponent<GravityForceComponent>();
	e3.addComponent<ParticleSphereComponent>();
	e3.addComponent<PenetrationDeltaMoveComponent>();

	auto e4 = world.createEntity();
	e4.addComponent<TransformComponent>(Vector3(-5, 5, 0));
	e4.addComponent<ParticleComponent>();
	e4.addComponent<ForceAccumulatorComponent>();
	e4.addComponent<GravityForceComponent>();
	e4.addComponent<ParticleSphereComponent>();
	e4.addComponent<PenetrationDeltaMoveComponent>();

	auto cable1 = world.createEntity();
	//cable.addComponent<CableComponent>(ePivot, e1, 5, 1);
	cable1.addComponent<PairedSpringComponent>(50, 2, ePivot, e1);

	auto cable2 = world.createEntity();
	//cable.addComponent<CableComponent>(ePivot, e1, 5, 1);
	cable2.addComponent<PairedSpringComponent>(50, 25, ePivot, e2);

	auto cable3 = world.createEntity();
	cable3.addComponent<CableComponent>(ePivot, e3, 15, 1);
	//cable3.addComponent<PairedSpringComponent>(50, 20, ePivot, e3);

	auto rod1 = world.createEntity();
	rod1.addComponent<RodComponent>(e1, e2, 5 * pow(2, 0.5f));

	auto rod2 = world.createEntity();
	rod2.addComponent<RodComponent>(e2, e3, 5 * pow(2, 0.5f));

	auto rod3 = world.createEntity();
	rod3.addComponent<RodComponent>(e3, e4, 5 * pow(2, 0.5f));

	auto rod4 = world.createEntity();
	rod4.addComponent<RodComponent>(e4, e1, 5 * pow(2, 0.5f));

	auto rod5 = world.createEntity();
	rod5.addComponent<RodComponent>(e1, e3, 10);

	auto rod6 = world.createEntity();
	rod6.addComponent<RodComponent>(e2, e4, 10);

	//for (int i = 0; i < 20; i++)
	//{
	//	auto e1 = world.createEntity();
	//	e1.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5)));
	//	e1.addComponent<ParticleComponent>();
	//	e1.addComponent<ForceAccumulatorComponent>();
	//	e1.addComponent<GravityForceComponent>();
	//	e1.addComponent<ParticleSphereComponent>(RANDOM_FLOAT(0.5, 1.5));
	//	e1.addComponent<PenetrationDeltaMoveComponent>();

	//	auto e2 = world.createEntity();
	//	e2.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5), RANDOM_FLOAT(-5, 5)));
	//	e2.addComponent<ParticleComponent>();
	//	e2.addComponent<ForceAccumulatorComponent>();
	//	e2.addComponent<GravityForceComponent>();
	//	e2.addComponent<ParticleSphereComponent>(RANDOM_FLOAT(0.5, 1.5));
	//	e2.addComponent<PenetrationDeltaMoveComponent>();

	//	auto rod = world.createEntity();
	//	rod.addComponent<RodComponent>(e1, e2, RANDOM_FLOAT(6, 10));
	//}
}

void SetupLights(ECSWorld& world)
{
	auto l = world.createEntity();
	l.addComponent<TransformComponent>(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(90, 0, 0));
	l.addComponent<DynamicDirectionalLightComponent>(Color(0.0, 0.1, 0.1), Color(0.0, 0.1, 0.1), Color(0.0, 0.1, 0.1));

	// Lanterns
	auto pl1 = world.createEntity();
	pl1.addComponent<TransformComponent>(Vector3(22, 14, 48.5f));
	pl1.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0, 0), Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f));
	auto hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(23, 15, 48.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 13.5f, 50.5f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(21, 12.5f, 47.5f));

	auto pl2 = world.createEntity();
	pl2.addComponent<TransformComponent>(Vector3(-14.5f, 14, 49.0f));
	pl2.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0, 0.1f), Color(0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 1, 14 - 1, 49.0f - 1));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.5f, 14 + 1, 49.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f, 14 - 1, 49.0f + 1));
	
	auto pl3 = world.createEntity();
	pl3.addComponent<TransformComponent>(Vector3(22, 14, -62.0f));
	pl3.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0.1f, 0), Color(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 - 1, 14 - 1, -62.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 14 + 0.5f, -62.0f - 1));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 + 1, 14, -62.0f + 0.5f));

	auto pl4 = world.createEntity();
	pl4.addComponent<TransformComponent>(Vector3(-14.5f, 14, -61.5f));
	pl4.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0.05, 0), Color(1.0f, 0.55f, 0.0f), Color(1.0f, 0.55f, 0.0f));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 1, 14, -61.5f -1));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.25f, 14 - 0.5f, -61.5f + 1));
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 0.5f, 14+ 1, -61.5f + 1));

	// Spears
	std::vector<Color> cols = { Color(1,0,0), Color(0,1,0), Color(0,0,1), Color(0.7f,0.55f,0) };
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pl1 = world.createEntity();
			pl1.addComponent<TransformComponent>(Vector3((i % 2 == 0 ? 8 : -1), 85, 49.5f - 37 * j), Vector3(1, 1, 1), Vector3(180, 0, 0));
			pl1.addComponent<DynamicSpotLightComponent>(10.0f, 100, Color(0, 0, 0), cols[3 - j], cols[3 - j], 5);
		}
	}
}