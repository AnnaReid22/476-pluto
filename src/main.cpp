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

	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> mesh;
	shared_ptr<Shape> theDog;
	shared_ptr<Shape> texcube;

	shared_ptr<Texture> dog_albedo;
	shared_ptr<Texture> grass_albedo;

	shared_ptr<Material> dogMat;
	shared_ptr<Material> groundMat;

	World w;
	RenderPipeline rp;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		InputManager* input = input->getInstance();

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
		string errStr;

		// Initialize dog mesh.
		vector<tinyobj::shape_t> TOshapesB;
		vector<tinyobj::material_t> objMaterialsB;
		//load in the dogmesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/dog.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			theDog = make_shared<Shape>();
			theDog->createShape(TOshapesB[0]);
			theDog->measure();
			theDog->init();
		}

		dog_albedo = make_shared<Texture>();
		dog_albedo->setFilename(resourceDirectory + "/cartoonWood.jpg");
		dog_albedo->init();
		dog_albedo->setUnit(0);
		dog_albedo->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		dogMat = make_shared<Material>();
		dogMat->t_albedo = dog_albedo;

		rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/texcube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			texcube = make_shared<Shape>();
			texcube->createShape(TOshapesB[0]);
			texcube->measure();
			texcube->init();
		}

		grass_albedo = make_shared<Texture>();
		grass_albedo->setFilename(resourceDirectory + "/grass.jpg");
		grass_albedo->init();
		grass_albedo->setUnit(1);
		grass_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

		dogMat = make_shared<Material>();
		dogMat->t_albedo = dog_albedo;

		groundMat = make_shared<Material>();
		groundMat->t_albedo = grass_albedo;

		GameObject* ground = new GameObject("ground");
		ground->transform.position = glm::vec3(0, -1, 0);
		ground->transform.scale = glm::vec3(30, 0.1, 30);

		MeshRenderer* gr = ground->addComponentOfType<MeshRenderer>();
		gr->mesh = texcube;
		gr->material = groundMat;

		w.addObject(ground);

		GameObject* player = new GameObject("player");
		Camera* cam = player->addComponentOfType<Camera>();
		cam->windowManager = windowManager;
		cam->eyeOffset = glm::vec3(0, 1, 0);
		w.mainCamera = cam;

		BoundingSphereCollider* bsc2 = player->addComponentOfType <BoundingSphereCollider>();

		w.addObject(player);

		GameObject* spawner = new GameObject("spawner");
		EnemySpawner* es = spawner->addComponentOfType<EnemySpawner>();
		es->spawnDelay = 5;

		es->enemyMesh = theDog;
		es->enemyMat = dogMat;

		w.addObject(spawner);
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
