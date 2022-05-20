/* Hello triangle lab - intro OGL
Z. Wood + S. Sueda
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Time.h"
#include "Physics.h"
#include "InputManager.h"
#include "World.h"
#include "GameObject.h"
#include "RenderPipeline.h"
#include "Texture.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "EnemySpawner.h"
#include "Player.h"

#include "ForwardRenderPass.h"
#include "ParticleRenderPass.h"
#include "ParticleSystem.h"
#include "DeferredSamplingPass.h"
#include "DeferredLightingPass.h"

using namespace std;
using namespace glm;

/* A big global wrapper for all our data */
class Application : public EventCallbacks {

public:

	WindowManager * windowManager = nullptr;

	// Shape to be used (from  file)
	shared_ptr<Shape> asteroid_shapes[15];
	shared_ptr<Shape> mesh;
	shared_ptr<Shape> theRocket;
	shared_ptr<Shape> texcube;
	shared_ptr<Shape> texSphere;
	shared_ptr<Shape> theEarth;
	shared_ptr<Shape> theMars;
	shared_ptr<Shape> theJupiter;
	shared_ptr<Shape> theSaturn;
	shared_ptr<Shape> theUranus;
	shared_ptr<Shape> theNeptune;
	shared_ptr<Shape> thePluto;

	shared_ptr<Texture> rocket_albedo;
	shared_ptr<Texture> grass_albedo;
	shared_ptr<Texture> asteroid_albedo;
	shared_ptr<Texture> sky_albedo;
	shared_ptr<Texture> sky_noise_albedo;
	shared_ptr<Texture> earth_albedo;
	shared_ptr<Texture> mars_albedo;
	shared_ptr<Texture> jupiter_albedo;
	shared_ptr<Texture> saturn_albedo;
	shared_ptr<Texture> uranus_albedo;
	shared_ptr<Texture> neptune_albedo;
	shared_ptr<Texture> pluto_albedo;
	shared_ptr<Texture> particleTexture;
	
	shared_ptr<Material> rocketMat;
	shared_ptr<Material> groundMat;
	shared_ptr<Material> asteroidMat;
	shared_ptr<Material> skyMat;
	shared_ptr<Material> earthMat;
	shared_ptr<Material> marsMat;
	shared_ptr<Material> jupiterMat;
	shared_ptr<Material> saturnMat;
	shared_ptr<Material> uranusMat;
	shared_ptr<Material> neptuneMat;
	shared_ptr<Material> plutoMat;
	shared_ptr<Material> particleMat;

	World w;
	RenderPipeline rp;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		InputManager* input = input->getInstance();

		// These inputs bypass the manager and can thus be considered "reserved" for their purposes
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		if (action == GLFW_PRESS)
		{
			input->KeyPressed(key);
		}

		if (action == GLFW_RELEASE)
		{
			input->KeyReleased(key);
		}

	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);

		ResourceManager* rm = rm->getInstance();

		rm->addNumericalValue("screenWidth", width);
		rm->addNumericalValue("screenHeight", height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		ResourceManager* rm = rm->getInstance();
		int width, height;

		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		rm->addNumericalValue("screenWidth", width);
		rm->addNumericalValue("screenHeight", height);

		w = World();
		rp = RenderPipeline(windowManager);


		rm->addOther("WindowManager", windowManager);

		rp.addRenderPass(std::make_shared<DeferredSamplingPass>());
		rp.addRenderPass(std::make_shared<DeferredLightingPass>());
		//rp.addRenderPass(std::make_shared<ForwardRenderPass>());
		rp.addRenderPass(std::make_shared<ParticleRenderPass>());
		// add render passes with more shaders here
	}

	void initGeom(const std::string& resourceDirectory)
	{
		ResourceManager* rm = rm->getInstance();

		//rocket loader
		theRocket = make_shared<Shape>();
		theRocket->loadMesh(resourceDirectory + "/rocket.obj");
        theRocket->resize();
        theRocket->init();

		rocket_albedo = make_shared<Texture>();
		rocket_albedo->setFilename(resourceDirectory + "/flowers.jpg");
		rocket_albedo->init();
		rocket_albedo->setUnit(0);
		rocket_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		rocketMat = make_shared<Material>();
		rocketMat->t_albedo = rocket_albedo;

		//asteroid loader
		asteroid_albedo = make_shared<Texture>();
		asteroid_albedo->setFilename(resourceDirectory + "/asteroid.jpg");
		asteroid_albedo->init();
		asteroid_albedo->setUnit(0);
		asteroid_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		asteroidMat = make_shared<Material>();
		asteroidMat->t_albedo = asteroid_albedo;
		string asteroid_names[15] = { "", "0", "1", "00", "01", "10", "11", "000", "001", "010", "011", "100", "101", "110", "111" };
		for (int i = 0; i < 15; i++) {
				asteroid_shapes[i] = make_shared<Shape>();
				asteroid_shapes[i]->loadMesh(resourceDirectory + "/asteroids/asteroid" + asteroid_names[i] + ".obj");
				asteroid_shapes[i]->shift();
				asteroid_shapes[i]->init();
		}
		rm->addOther("asteroid_shapes", asteroid_shapes);
		rm->addOther("asteroid_material", &asteroidMat);

		asteroidMat = make_shared<Material>();
		asteroidMat->t_albedo = asteroid_albedo;

		//skybox loader
		texSphere = make_shared<Shape>();
		texSphere->loadMesh(resourceDirectory + "/texCube.obj");
        texSphere->resize();
        texSphere->init();

		sky_albedo = make_shared<Texture>();
        sky_albedo->setFilename(resourceDirectory + "/spaceSkybox.jpg");
        sky_albedo->init();
        sky_albedo->setUnit(0);
        sky_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

        skyMat = make_shared<Material>();
        skyMat->t_albedo = sky_albedo;


		sky_noise_albedo = make_shared<Texture>();
		sky_noise_albedo->setFilename(resourceDirectory + "/twinkle_noise.jpg");
		sky_noise_albedo->init();
		sky_noise_albedo->setUnit(0);
		sky_noise_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

		rm->addMesh("skybox", texSphere);
		rm->addUserTextureResource("skybox", sky_albedo);
		rm->addUserTextureResource("skyboxNoise", sky_noise_albedo);

		//player loader
		GameObject* player = new GameObject("player");
		Player* pl = player->addComponentOfType<Player>();
		player->transform.scale = glm::vec3(0.4);
		GameObject* camera = new GameObject("camera");
		Camera* cam = camera->addComponentOfType<Camera>();
		cam->rocket = pl;

		MeshRenderer* rocket = player->addComponentOfType<MeshRenderer>();
		rocket->mesh = theRocket;
		rocket->material = rocketMat;

		rm->addOther("player_game_object", player);

		w.mainCamera = cam;

		BoundingSphereCollider* bsc2 = player->addComponentOfType <BoundingSphereCollider>();

		w.addObject(player);

		//enemy loader
		GameObject* spawner = new GameObject("spawner");
		EnemySpawner* es = spawner->addComponentOfType<EnemySpawner>();
		es->spawnDelay = .7;


		//Order matters here, because the skybox has to render before anything else.

		initPlanets(resourceDirectory);

		//add particle system texture
		particleTexture = make_shared<Texture>();
		particleTexture->setFilename(resourceDirectory + "/fire.png");
		particleTexture->init();
		particleTexture->setUnit(0);
		particleTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		rm->addUserTextureResource("particleTexture", particleTexture);

		ParticleSystem* ps = player->addComponentOfType<ParticleSystem>();
		ps->start = pl->getPosition()-pl->getForward();
		ps->type = "moving";
		ps->numParticles = 300;
		ps->color = vec4(1.0, 0.7, 0.2, 1.0f);
		ps->max_velocity = vec3(-0.05, -0.02, -0.04);
		ps->min_velocity = vec3(-0.1, -0.05, -0.1);
		ps->lifespan = 2.0f;
		ps->GPUSetup();

		GameObject* ps_death = new GameObject("ps_death");
		ParticleSystem* ps_die = ps_death->addComponentOfType<ParticleSystem>();
		ps_die->start = vec3(0.0);
		ps_die->type = "static";
		ps_die->numParticles = 300;
		ps_die->color = vec4(1.0, 0.3, 0.2, 1.0f);
		ps_die->max_velocity = vec3(-0.01);
		ps_die->min_velocity = vec3(-0.05);
		ps_die->lifespan = 3.0f;
		ps_die->GPUSetup();
		ps_death->Disable();
		rm->addOther("particle_system_death", ps_death);

		//add celebration code
		GameObject* ps_celebrate1 = new GameObject("ps_celebrate1");
		ParticleSystem* ps_c1 = ps_celebrate1->addComponentOfType<ParticleSystem>();
		ps_c1->start = vec3(1.0);
		ps_c1->type = "static";
		ps_c1->numParticles = 300;
		ps_c1->color = vec4(2.0, 0.7, 0.2, 1.0f);
		ps_c1->max_velocity = vec3(-0.01);
		ps_c1->min_velocity = vec3(-0.05);
		ps_c1->lifespan = 8.0f;
		ps_c1->GPUSetup();
		ps_celebrate1->Disable();
		rm->addOther("particle_system_c1", ps_celebrate1);

		GameObject* ps_celebrate2 = new GameObject("ps_celebrate2");
		ParticleSystem* ps_c2 = ps_celebrate2->addComponentOfType<ParticleSystem>();
		ps_c2->start = vec3(2.0);
		ps_c2->type = "static";
		ps_c2->numParticles = 300;
		ps_c2->color = vec4(1.0, 0.1, 0.4, 1.0f);
		ps_c2->max_velocity = vec3(-0.01);
		ps_c2->min_velocity = vec3(-0.03);
		ps_c2->lifespan = 5.0f;
		ps_c2->GPUSetup();
		ps_celebrate2->Disable();
		rm->addOther("particle_system_c2", ps_celebrate2);

		GameObject* ps_celebrate3 = new GameObject("ps_celebrate3");
		ParticleSystem* ps_c3 = ps_celebrate3->addComponentOfType<ParticleSystem>();
		ps_c3->start = vec3(0.0);
		ps_c3->type = "static";
		ps_c3->numParticles = 300;
		ps_c3->color = vec4(0.9, 0.0, 0.9, 1.0f);
		ps_c3->max_velocity = vec3(-0.05);
		ps_c3->min_velocity = vec3(-0.1);
		ps_c3->lifespan = 7.0f;
		ps_c3->GPUSetup();
		ps_celebrate3->Disable();
		rm->addOther("particle_system_c3", ps_celebrate3);

		GameObject* ps_celebrate4 = new GameObject("ps_celebrate4");
		ParticleSystem* ps_c4 = ps_celebrate4->addComponentOfType<ParticleSystem>();
		ps_c4->start = vec3(2.0);
		ps_c4->type = "static";
		ps_c4->numParticles = 300;
		ps_c4->color = vec4(1.0, 0.0, 0.0, 1.0f);
		ps_c4->max_velocity = vec3(-0.01);
		ps_c4->min_velocity = vec3(-0.06);
		ps_c4->lifespan = 7.0f;
		ps_c4->GPUSetup();
		ps_celebrate4->Disable();
		rm->addOther("particle_system_c4", ps_celebrate4);

		GameObject* ps_celebrate5 = new GameObject("ps_celebrate5");
		ParticleSystem* ps_c5 = ps_celebrate5->addComponentOfType<ParticleSystem>();
		ps_c5->start = vec3(2.0);
		ps_c5->type = "static";
		ps_c5->numParticles = 300;
		ps_c5->color = vec4(0.0, 1.0, 1.0, 1.0f);
		ps_c5->max_velocity = vec3(-0.02);
		ps_c5->min_velocity = vec3(-0.08);
		ps_c5->lifespan = 6.0f;
		ps_c5->GPUSetup();
		ps_celebrate5->Disable();
		rm->addOther("particle_system_c5", ps_celebrate5);


		w.addObject(ps_death);
		w.addObject(ps_celebrate1);
		w.addObject(ps_celebrate2);
		w.addObject(ps_celebrate3);
		w.addObject(ps_celebrate4);
		w.addObject(ps_celebrate5);
		w.addObject(player);
		w.addObject(spawner);
		w.addObject(camera);
	}

	void initPlanets(const std::string& resourceDirectory){
		//earth
		theEarth = make_shared<Shape>();
		theEarth->loadMesh(resourceDirectory + "/planets/earth.obj");
        theEarth->resize();
        theEarth->init();

		earth_albedo = make_shared<Texture>();
		earth_albedo->setFilename(resourceDirectory + "/planets/earth.png");
		earth_albedo->init();
		earth_albedo->setUnit(0);
		earth_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		earthMat = make_shared<Material>();
		earthMat->t_albedo = earth_albedo;

		GameObject* earth = new GameObject("earth");
		// earth->transform.position = glm::vec3(0, -30, 0);
		// earth->transform.scale = glm::vec3(45);
		earth->transform.position = glm::vec3(0, -12, 0);
		earth->transform.scale = glm::vec3(10);
		earth->tag = "planet";
		MeshRenderer* earthMR = earth->addComponentOfType<MeshRenderer>();
		earthMR->mesh = theEarth;
		earthMR->material = earthMat;

		w.addObject(earth);

		//mars
		theMars = make_shared<Shape>();
		theMars->loadMesh(resourceDirectory + "/planets/mars.obj");
        theMars->resize();
        theMars->init();

		mars_albedo = make_shared<Texture>();
		mars_albedo->setFilename(resourceDirectory + "/planets/mars.png");
		mars_albedo->init();
		mars_albedo->setUnit(0);
		mars_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		marsMat = make_shared<Material>();
		marsMat->t_albedo = mars_albedo;

		GameObject* mars = new GameObject("mars");
		mars->transform.position = glm::vec3(190, 0, -200);
		mars->transform.scale = glm::vec3(80);
		// mars->transform.position = glm::vec3(10, 0, -100);
		// mars->transform.scale = glm::vec3(5);
		mars->tag = "planet";
		MeshRenderer* marsMR = mars->addComponentOfType<MeshRenderer>();
		marsMR->mesh = theMars;
		marsMR->material = marsMat;

		BoundingSphereCollider* bsc_mars = mars->addComponentOfType<BoundingSphereCollider>();
		
		w.addObject(mars);
		bsc_mars->radius = 1;

		//jupiter
		theJupiter = make_shared<Shape>();
		theJupiter->loadMesh(resourceDirectory + "/planets/jupiter.obj");
        theJupiter->resize();
        theJupiter->init();

		jupiter_albedo = make_shared<Texture>();
		jupiter_albedo->setFilename(resourceDirectory + "/planets/jupiter.png");
		jupiter_albedo->init();
		jupiter_albedo->setUnit(0);
		jupiter_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		jupiterMat = make_shared<Material>();
		jupiterMat->t_albedo = jupiter_albedo;

		GameObject* jupiter = new GameObject("jupiter");
		jupiter->transform.position = glm::vec3(-280, 0, -300);
		jupiter->transform.scale = glm::vec3(175);
		// jupiter->transform.position = glm::vec3(-30, 0, -200);
		// jupiter->transform.scale = glm::vec3(30);
		jupiter->tag = "planet";
		MeshRenderer* jupiterMR = jupiter->addComponentOfType<MeshRenderer>();
		jupiterMR->mesh = theJupiter;
		jupiterMR->material = jupiterMat;

		BoundingSphereCollider* bsc_jupiter = jupiter->addComponentOfType<BoundingSphereCollider>();

		w.addObject(jupiter);
		bsc_jupiter->radius = 1;

		//saturn
		theSaturn = make_shared<Shape>();
		theSaturn->loadMesh(resourceDirectory + "/planets/saturn.obj");
        theSaturn->resize();
        theSaturn->init();

		saturn_albedo = make_shared<Texture>();
		saturn_albedo->setFilename(resourceDirectory + "/planets/saturn.jpg");
		saturn_albedo->init();
		saturn_albedo->setUnit(0);
		saturn_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		saturnMat = make_shared<Material>();
		saturnMat->t_albedo = saturn_albedo;

		GameObject* saturn = new GameObject("saturn");
		saturn->transform.position = glm::vec3(250, 0, -450);
		saturn->transform.scale = glm::vec3(150);
		// saturn->transform.position = glm::vec3(40, 0, -450);
		// saturn->transform.scale = glm::vec3(35);
		saturn->tag = "planet";
		MeshRenderer* saturnMR = saturn->addComponentOfType<MeshRenderer>();
		saturnMR->mesh = theSaturn;
		saturnMR->material = saturnMat;

		BoundingSphereCollider* bsc_saturn = saturn->addComponentOfType<BoundingSphereCollider>();

		w.addObject(saturn);
		bsc_saturn->radius = 1;

		//uranus
		theUranus = make_shared<Shape>();
		theUranus->loadMesh(resourceDirectory + "/planets/uranus.obj");
        theUranus->resize();
        theUranus->init();

		uranus_albedo = make_shared<Texture>();
		uranus_albedo->setFilename(resourceDirectory + "/planets/uranus.jpg");
		uranus_albedo->init();
		uranus_albedo->setUnit(0);
		uranus_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		uranusMat = make_shared<Material>();
		uranusMat->t_albedo = uranus_albedo;

		GameObject* uranus = new GameObject("uranus");
		uranus->transform.position = glm::vec3(120, 0, -700);
		uranus->transform.scale = glm::vec3(110);
		// uranus->transform.position = glm::vec3(50, 0, -600);
		// uranus->transform.scale = glm::vec3(25);
		uranus->tag = "planet";
		MeshRenderer* uranusMR = uranus->addComponentOfType<MeshRenderer>();
		uranusMR->mesh = theUranus;
		uranusMR->material = uranusMat;

		BoundingSphereCollider* bsc_uranus = uranus->addComponentOfType<BoundingSphereCollider>();

		w.addObject(uranus);
		bsc_uranus->radius = 1;

		//neptune
		theNeptune = make_shared<Shape>();
		theNeptune->loadMesh(resourceDirectory + "/planets/neptune.obj");
        theNeptune->resize();
        theNeptune->init();

		neptune_albedo = make_shared<Texture>();
		neptune_albedo->setFilename(resourceDirectory + "/planets/neptune.png");
		neptune_albedo->init();
		neptune_albedo->setUnit(0);
		neptune_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		neptuneMat = make_shared<Material>();
		neptuneMat->t_albedo = neptune_albedo;

		GameObject* neptune = new GameObject("neptune");
		neptune->transform.position = glm::vec3(-120, 0, -800);
		neptune->transform.scale = glm::vec3(100);
		// neptune->transform.position = glm::vec3(-45, 0, -800);
		// neptune->transform.scale = glm::vec3(25);
		neptune->tag = "planet";
		MeshRenderer* neptuneMR = neptune->addComponentOfType<MeshRenderer>();
		neptuneMR->mesh = theNeptune;
		neptuneMR->material = neptuneMat;

		BoundingSphereCollider* bsc_neptune = neptune->addComponentOfType<BoundingSphereCollider>();

		w.addObject(neptune);
		bsc_neptune->radius = 1;

		//pluto
		thePluto = make_shared<Shape>();
		thePluto->loadMesh(resourceDirectory + "/planets/pluto.obj");
        thePluto->resize();
        thePluto->init();

		pluto_albedo = make_shared<Texture>();
		pluto_albedo->setFilename(resourceDirectory + "/planets/pluto.png");
		pluto_albedo->init();
		pluto_albedo->setUnit(0);
		pluto_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		plutoMat = make_shared<Material>();
		plutoMat->t_albedo = pluto_albedo;

		GameObject* pluto = new GameObject("pluto");
		pluto->transform.position = glm::vec3(0, 0, -1000);
		pluto->transform.scale = glm::vec3(15);
		// pluto->transform.position = glm::vec3(5, 0, -1000);
		// pluto->transform.scale = glm::vec3(1);
		pluto->tag = "planet";
		MeshRenderer* plutoMR = pluto->addComponentOfType<MeshRenderer>();
		plutoMR->mesh = thePluto;
		plutoMR->material = plutoMat;

		BoundingSphereCollider* bsc_pluto = pluto->addComponentOfType<BoundingSphereCollider>();

		w.addObject(pluto);
		bsc_pluto->radius = 1;
	}

	void run()
	{
		// Update frame and global time at the beginning of each frame cycle
		Time* time = time->getInstance();
		ResourceManager* rm = rm->getInstance();
		Physics* physics = physics->getInstance();

		time->updateTime();
		physics->clearCollideables();

		// Run the world simulation for this frame
		w.handleDestructions();
		w.runSimulation();
		physics->checkCollisions();

		// Get vector of renderable gameobjects and submit to RenderPipeline
		std::vector<GameObject*> renderables = w.getRenderables();
		std::vector<GameObject*> curPS = w.getParticleSystems();


		rm->addOther("particleSystem", &curPS);
		rm->addOther("renderables", &renderables);
		rm->addOther("activeCamera", w.mainCamera);

		rp.executePipeline();
	}
};

Time* Time::instance = 0;
Physics* Physics::instance = 0;
InputManager* InputManager::instance = 0;
ResourceManager* ResourceManager::instance = 0;

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application* application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	InputManager* input = input->getInstance();

	input->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->run();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
