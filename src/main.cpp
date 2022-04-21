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

using namespace std;
using namespace glm;

/* A big global wrapper for all our data */
class Application : public EventCallbacks {

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Shape to be used (from  file)
	shared_ptr<Shape> mesh;
	shared_ptr<Shape> theRocket;
	shared_ptr<Shape> asteroid;
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
	shared_ptr<Texture> earth_albedo;
	shared_ptr<Texture> mars_albedo;
	shared_ptr<Texture> jupiter_albedo;
	shared_ptr<Texture> saturn_albedo;
	shared_ptr<Texture> uranus_albedo;
	shared_ptr<Texture> neptune_albedo;
	shared_ptr<Texture> pluto_albedo;
	
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
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames("../shaders/tex_vert.glsl", "../shaders/tex_frag0.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("Texture0");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");
		prog->addUniform("lightPos");
        prog->addUniform("MatShine");
        prog->addUniform("flip");
        prog->addAttribute("vertNor");

		w = World();
		rp = RenderPipeline(windowManager);
		rp.prog = prog;
	}

	void initGeom(const std::string& resourceDirectory)
	{
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
		asteroid = make_shared<Shape>();
		asteroid->loadMesh(resourceDirectory + "/asteroid.obj");
        asteroid->resize();
        asteroid->init();

		asteroid_albedo = make_shared<Texture>();
		asteroid_albedo->setFilename(resourceDirectory + "/asteroid.jpg");
		asteroid_albedo->init();
		asteroid_albedo->setUnit(0);
		asteroid_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		asteroidMat = make_shared<Material>();
		asteroidMat->t_albedo = asteroid_albedo;

		//skybox loader
		texSphere = make_shared<Shape>();
		texSphere->loadMesh(resourceDirectory + "/texSphere.obj");
        texSphere->resize();
        texSphere->init();

		sky_albedo = make_shared<Texture>();
        sky_albedo->setFilename(resourceDirectory + "/spaceSkybox.jpg");
        sky_albedo->init();
        sky_albedo->setUnit(1);
        sky_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

        skyMat = make_shared<Material>();
        skyMat->t_albedo = sky_albedo;

        rp.skyboxMaterial = skyMat;
        rp.skyboxMesh = texSphere;

		//player loader
		GameObject* player = new GameObject("player");
		player->transform.scale = glm::vec3(0.2);
		Camera* cam = player->addComponentOfType<Camera>();
		cam->windowManager = windowManager;
		cam->eyeOffset = glm::vec3(0, 1, 4);

		MeshRenderer* rocket = player->addComponentOfType<MeshRenderer>();
		rocket->mesh = theRocket;
		rocket->material = rocketMat;

		w.mainCamera = cam;

		BoundingSphereCollider* bsc2 = player->addComponentOfType <BoundingSphereCollider>();

		w.addObject(player);

		rp.skyboxMesh = texSphere;
		rp.skyboxMaterial = skyMat;

		//enemy loader
		GameObject* spawner = new GameObject("spawner");
		EnemySpawner* es = spawner->addComponentOfType<EnemySpawner>();
		es->spawnDelay = .7;

		es->enemyMesh = asteroid;
		es->enemyMat = asteroidMat;

		//Order matters here, because the skybox has to render before anything else.
		w.addObject(player);
		w.addObject(spawner);

		initPlanets(resourceDirectory);

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
		earth->transform.position = glm::vec3(0, -12, 0);
		earth->transform.scale = glm::vec3(10);
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
		mars->transform.position = glm::vec3(10, 0, -100);
		mars->transform.scale = glm::vec3(5);
		MeshRenderer* marsMR = mars->addComponentOfType<MeshRenderer>();
		marsMR->mesh = theMars;
		marsMR->material = marsMat;

		w.addObject(mars);

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
		jupiter->transform.position = glm::vec3(-30, 0, -200);
		jupiter->transform.scale = glm::vec3(30);
		MeshRenderer* jupiterMR = jupiter->addComponentOfType<MeshRenderer>();
		jupiterMR->mesh = theJupiter;
		jupiterMR->material = jupiterMat;

		w.addObject(jupiter);

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
		saturn->transform.position = glm::vec3(40, 0, -450);
		saturn->transform.scale = glm::vec3(35);
		MeshRenderer* saturnMR = saturn->addComponentOfType<MeshRenderer>();
		saturnMR->mesh = theSaturn;
		saturnMR->material = saturnMat;

		w.addObject(saturn);

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
		uranus->transform.position = glm::vec3(50, 0, -600);
		uranus->transform.scale = glm::vec3(25);
		MeshRenderer* uranusMR = uranus->addComponentOfType<MeshRenderer>();
		uranusMR->mesh = theUranus;
		uranusMR->material = uranusMat;

		w.addObject(uranus);

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
		neptune->transform.position = glm::vec3(-45, 0, -800);
		neptune->transform.scale = glm::vec3(25);
		MeshRenderer* neptuneMR = neptune->addComponentOfType<MeshRenderer>();
		neptuneMR->mesh = theNeptune;
		neptuneMR->material = neptuneMat;

		w.addObject(neptune);

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
		pluto->transform.position = glm::vec3(5, 0, -1000);
		MeshRenderer* plutoMR = pluto->addComponentOfType<MeshRenderer>();
		plutoMR->mesh = thePluto;
		plutoMR->material = plutoMat;

		w.addObject(pluto);
	}

	void run()
	{
		// Update frame and global time at the beginning of each frame cycle
		Time* time = time->getInstance();
		time->updateTime();

		Physics* physics = physics->getInstance();
		physics->clearCollideables();

		// Run the world simulation for this frame
		w.handleDestructions();
		w.runSimulation();
		physics->checkCollisions();

		// Get vector of renderable gameobjects and submit to RenderPipeline
		std::vector<GameObject*> renderables = w.getRenderables();
		w.mainCamera->setUpCam(windowManager);
		rp.renderFrame(renderables, w.mainCamera);
	}
};

Time* Time::instance = 0;
Physics* Physics::instance = 0;
InputManager* InputManager::instance = 0;

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
