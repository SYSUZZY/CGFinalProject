#pragma comment(lib,"glew32.lib")
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <string>

#include "my_class/dependency/glew.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "my_class/particle/Flame.h"
#include "my_class/shader/shader_m.h"
#include "my_class/camera/camera.h"
#include "my_class/model/model.h"
#include "my_class/tools/PhysicsEngine.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, -20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// PhysicsEngine
//PhysicsEngine *physicsEngine;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	Flame::Flame flame;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// build and compile shaders
	// -------------------------
	Shader pandaShader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
	Shader sceneShader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");
	Shader meteoriteShader("resource/shader/1.model_loading.vs", "resource/shader/1.model_loading.fs");

	// load models
	// -----------
	string pandaPath = "resource/model/Panda_animation/panda.FBX";
	string sencePath = "resource/model/scene/scene.obj";
	string meteoritePath = "resource/model/Meteorite/stg600_obj_meteoriteB01.obj";
	

	std::cout << "Start loading panda" << std::endl;
	Model pandaModel(pandaPath);
	std::cout << "Panda complete" << std::endl;

	std::cout << "Start loading scene" << std::endl;
	Model senceModel(sencePath);
	std::cout << "Scene complete" << std::endl;

	std::cout << "Start loading meteorite" << std::endl;
	Model meteoriteModel(meteoritePath);
	std::cout << "Meteorite complete" << std::endl;

	// ÉèÖÃcollisionBox
	//physicsEngine = new PhysicsEngine;
	//physicsEngine->setSceneOuterBoundary(glm::vec2 (-80, -60), glm::vec2(132, 60));
	//physicsEngine->setSceneInnerBoundary(glm::vec3(-80, -1, -60), glm::vec3(132, 1, 60));

	// draw in wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		pandaShader.use();
		sceneShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		pandaShader.setMat4("projection", projection);
		sceneShader.setMat4("projection", projection);
		pandaShader.setMat4("view", view);
		sceneShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model_panda;
		model_panda = glm::translate(model_panda, camera.Position); // translate it down so it's at the center of the scene
		model_panda = glm::scale(model_panda, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down

		glm::mat4 model1;
		model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model1 = glm::scale(model1, glm::vec3(0.05f, 0.05f, 0.05f));

		glm::mat4 model_scene;
		model_scene = glm::translate(model_scene, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		//model1 = glm::scale(model1, glm::vec3(0.005f, 0.005f, 0.005f));

		glm::mat4 model_meteorite;
		model_meteorite = glm::translate(model_meteorite, glm::vec3(0.0f, 50.0f, -50.0f));

		pandaShader.setMat4("model", model_panda);
		//pandaModel.Draw(pandaShader);


		sceneShader.setMat4("model", model_scene);
		senceModel.Draw(sceneShader);

		meteoriteShader.setMat4("model", model_meteorite);
		meteoriteModel.Draw(meteoriteShader);

		projection = glm::mat4(1.0f);
		glm::mat4 model_flame;
		model_flame = glm::translate(model_flame, glm::vec3(0.0f, 0.0f, 0.0f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		flame.Render(deltaTime, model_flame, view, projection);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
