//UNIVERSIDAD NACIONAL AUTÓNOMA DE MEXICO
// FACULTAD DE INGENIERIA
// COMPUTACION GRAFICA E INTERACCION HUMANO COMPUTADORA

//PROYECTO : TORRES DE HANOI
//Alumna: Carolina Tellez Gallardo
//Numero de cuenta: 316292896

//SE IMPORTAN LAS LIBRERIAS NECESARIAS PARA EL PROYECTO
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

//SE DECLARAN LOS SHADERS PARA EL PROYECTO
// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

//En las siguientes lineas se declaran las funciones que se van a utilizar para el movimiento de la camara y las animaciones
// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(-100.0f, 2.0f, -45.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 0.35f, -45.0f);
bool active;

//POSICIONAMIENTO DE LAS LUCES
// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 LightP1;

//En las siguientes lineas se definen las variables que se utilizaran para las animaciones de cada uno de los discos
// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//La animacion de las torres de hanoi se implementara mediante keyframes
// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z;
float posX1 = PosIni.x, posY1 = PosIni.y, posZ1 = PosIni.z;
float posX2 = PosIni.x, posY2 = PosIni.y, posZ2 = PosIni.z;

//Definimos un maximo de frames que sera guardado para capturar cada posicion de los discos
//Para los frames vamos a definir 21 pasos
#define MAX_FRAMES 21
int i_max_steps = 190;
int i_curr_steps = 0;

//Declaramos una estructura para guardar las posiciones de los discos en los ejes X Y Z
typedef struct _frame
{
	//Variables para GUARDAR Key Frames

	
	float posX;	//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ

	//-------------------------------------------------
	
	float posX1;	//Variable para PosicionX
	float posY1;		//Variable para PosicionY
	float posZ1;		//Variable para PosicionZ

	float incX1;		//Variable para IncrementoX
	float incY1;		//Variable para IncrementoY
	float incZ1;		//Variable para IncrementoZ

	//------------------------------------------------

	float posX2;	//Variable para PosicionX
	float posY2;		//Variable para PosicionY
	float posZ2;		//Variable para PosicionZ

	float incX2;		//Variable para IncrementoX
	float incY2;		//Variable para IncrementoY
	float incZ2;		//Variable para IncrementoZ

}FRAME;

//Con las siguientes variables vamos a inicializar los keyframes
FRAME KeyFrame[MAX_FRAMES]; //Asigno num total de frames
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

//La siguiente funcion guardara los keyframes de cada disco
void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	//----------------------------------------------
	KeyFrame[FrameIndex].posX1 = posX1;
	KeyFrame[FrameIndex].posY1 = posY1;
	KeyFrame[FrameIndex].posZ1 = posZ1;

	//---------------------------------------------

	KeyFrame[FrameIndex].posX2 = posX2;
	KeyFrame[FrameIndex].posY2 = posY2;
	KeyFrame[FrameIndex].posZ2 = posZ2;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;
	
	//------------------------------------------

	posX1 = KeyFrame[0].posX1;
	posY1 = KeyFrame[0].posY1;
	posZ1 = KeyFrame[0].posZ1;

	//-----------------------------------------
	posX2 = KeyFrame[0].posX2;
	posY2 = KeyFrame[0].posY2;
	posZ2 = KeyFrame[0].posZ2;
}

//Un paso importante de la animacion por keyframes es la interpolacion
//esto hara que cada uno de los frames se una para crear un movimiento constante
void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	//------------------------------------------------------------------------------------------------

	KeyFrame[playIndex].incX1 = (KeyFrame[playIndex + 1].posX1 - KeyFrame[playIndex].posX1) / i_max_steps;
	KeyFrame[playIndex].incY1 = (KeyFrame[playIndex + 1].posY1 - KeyFrame[playIndex].posY1) / i_max_steps;
	KeyFrame[playIndex].incZ1 = (KeyFrame[playIndex + 1].posZ1 - KeyFrame[playIndex].posZ1) / i_max_steps;

	//-------------------------------------------------------------------------------------------------
	KeyFrame[playIndex].incX2 = (KeyFrame[playIndex + 1].posX2 - KeyFrame[playIndex].posX2) / i_max_steps;
	KeyFrame[playIndex].incY2 = (KeyFrame[playIndex + 1].posY2 - KeyFrame[playIndex].posY2) / i_max_steps;
	KeyFrame[playIndex].incZ2 = (KeyFrame[playIndex + 1].posZ2 - KeyFrame[playIndex].posZ2) / i_max_steps;



}

//DECLARACION DE LA FUNCION MAIN
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto_CarolinaTellezGallardo", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	//LLAMADO A LOS SHADERS
	//Para este proyecto es necesario el uso de shaders
	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");

	//A ontinuacion se declaran los modelos a utilizar
	//Estos modelos fueron creados con la ayuda del software Blender 
	Model mesa((char*)"Models/Carro/mesa.obj");
	Model torre1((char*)"Models/Carro/torre1.obj");
	Model disco((char*)"Models/Carro/disco.obj");
	Model disco2((char*)"Models/Carro/disco2.obj");
	Model disco3((char*)"Models/Carro/disco3.obj");
	Model letrero((char*)"Models/Carro/letrero.obj");


	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;

		//-----------------------------------------------------
		KeyFrame[i].posX1 = 0;
		KeyFrame[i].incX1 = 0;
		KeyFrame[i].incY1 = 0;
		KeyFrame[i].incZ1 = 0;

		//----------------------------------------------------
		KeyFrame[i].posX2 = 0;
		KeyFrame[i].incX2 = 0;
		KeyFrame[i].incY2 = 0;
		KeyFrame[i].incZ2 = 0;

		
	}

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};

	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Load textures
	GLuint texture1, texture2;
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);

	int textureWidth, textureHeight, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// Diffuse map
	image = stbi_load("images/TexturesCom_GravelCobble0019_7_S.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);

	// Specular map
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	image = stbi_load("images/TexturesCom_GravelCobble0019_7_S.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		//En las siguientes lineas vamos a definir las posiciones de las luces direccionales, pointligths y spot light

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		//DECLARACION DE LAS LUCES POINLIGTHS Y SPOTLIGHT

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);

		//EN LAS SIGUIENTES LINEAS A TRAVES DE LAS TRANSFORMACIONES BASICAS SE COLOCAN LOS OBJETOS EN EL ESPACIO 3D
		//Colocare una mesa para que el escenario se vea mas ordenado

		//Colocar mesa
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-58.0f, 1.0f, -40.5f));
		model = glm::scale(model, glm::vec3(3.5f, 1.0f, 3.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);

		//Colocar pilar
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-110.0f, 2.0f, -75.5f));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		torre1.Draw(lightingShader);

		//Colocar segundo pilar
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-80.0f, 2.0f, -75.5f));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		torre1.Draw(lightingShader);


		//Colocar tercer pilar
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-50.0f, 2.0f, -75.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		torre1.Draw(lightingShader);

		//Se colocaran los toroides en la base 1 en su posicion inicial
		//Toroide 1 toroide rojo
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::translate(model, glm::vec3(-15.5f, 3.0f, -30.5f));
		//Con la siguiente transformacion vamos a mover el toroide 1
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		disco.Draw(lightingShader);

		//Toroide2 toroide verde
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-15.5f, 4.0f, -30.5f));
		//Con la siguiente transformacion vamoa a mover el toroide 2
		model = glm::translate(model, glm::vec3(posX1, posY1, posZ1));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		disco2.Draw(lightingShader);

		//Toroide 3 toroide azul
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posX2, posY2, posZ2));
		model = glm::translate(model, glm::vec3(-15.5f, 5.0f, -30.5f));
		//Con la siguiente transformacion vamoa a mover el toroide 2
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		disco3.Draw(lightingShader);


		//En las siguientes lineas se colocara el plano con el nombre de la alumna
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-30.0f, 35.0f,-75.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(2.3f, 3.2f, 2.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		letrero.Draw(lightingShader);

		glBindVertexArray(0);

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}

//Esta funcion es la encargada de mover a traves de los ejes correspondientes X Y Z cada uno de los toroides
void DoMovement()
{
	{
		//Movimiento Toroide2 azul   C V E R
		
		if (keys[GLFW_KEY_C])
		{
			posX2 -= 0.1;
		}

		if (keys[GLFW_KEY_V])
		{
			posX2 += 0.1;
		}

		if (keys[GLFW_KEY_E])
		{
			posY2 -= 0.1;
		}

		if (keys[GLFW_KEY_R])
		{
			posY2 += 0.1;
		}
		
		//Movimiento Toroide2 verde   Z Y I U

		if (keys[GLFW_KEY_Z])
		{
			posX1-= 0.1;
		}

		if (keys[GLFW_KEY_Y])
		{
			posX1+= 0.1;
		}

		if (keys[GLFW_KEY_I])
		{
			posY1-= 0.1;
		}

		if (keys[GLFW_KEY_U])
		{
			posY1+= 0.1;
		}



		//Movimiento toroide 1 rojo teclas  G J B N 
		if (keys[GLFW_KEY_G])
		{
			posX -= 0.1;
		}

		if (keys[GLFW_KEY_J])
		{
			posX += 0.1;
		}

		if (keys[GLFW_KEY_B])
		{
			posY -= 0.1;
		}

		if (keys[GLFW_KEY_N])
		{
			posY += 0.1;
		}

		
		if (keys[GLFW_KEY_1])
		{
			range += 0.1;
			rot += 1;
			printf("El rango es %f\n", range);
		}

		if (keys[GLFW_KEY_2])
		{
			range -= 0.1;
			printf("El rango es %f\n", range);
		}

		if (keys[GLFW_KEY_3])
		{
			range += 0.1;
			printf("El spotangle es %f\n", range);
		}

		if (keys[GLFW_KEY_4])
		{
			range -= 0.1;
			printf("El spotangle es %f\n", range);
		}

		// Camera controls
		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		{
			camera.ProcessKeyboard(FORWARD, deltaTime);

		}

		if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		{
			camera.ProcessKeyboard(BACKWARD, deltaTime);


		}

		if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		{
			camera.ProcessKeyboard(LEFT, deltaTime);


		}

		if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		{
			camera.ProcessKeyboard(RIGHT, deltaTime);


		}

		if (keys[GLFW_KEY_T])
		{
			//pointLightPositions[0].x -= 0.1f;
			//pointLightPositions[0].y -= 0.1f;
			pointLightPositions[0].z += 0.1f;
		}
		if (keys[GLFW_KEY_G])
		{
			//pointLightPositions[0].x -= 0.1f;
			//pointLightPositions[0].y -= 0.1f;
			pointLightPositions[0].z -= 0.1f;
		}

		if (keys[GLFW_KEY_Y])
		{
			pointLightPositions[1].x += 0.1f;
			pointLightPositions[1].y += 0.1f;
			pointLightPositions[1].z += 0.1f;
		}

		if (keys[GLFW_KEY_H])
		{
			pointLightPositions[1].x -= 0.1f;
			pointLightPositions[1].y -= 0.1f;
			pointLightPositions[1].z -= 0.1f;
		}
		if (keys[GLFW_KEY_U])
		{
			pointLightPositions[2].x += 0.1f;
			pointLightPositions[2].y += 0.1f;
			pointLightPositions[2].z += 0.1f;
		}
		if (keys[GLFW_KEY_J])
		{
			pointLightPositions[2].x -= 0.1f;
			pointLightPositions[2].y -= 0.1f;
			pointLightPositions[2].z -= 0.1f;

		}


		if (keys[GLFW_KEY_I])
		{
			pointLightPositions[3].x += 0.1f;
			pointLightPositions[3].y += 0.1f;
			pointLightPositions[3].z += 0.1f;

		}

		//ANIMACIONES DE LOS OBJETOS

		if (keys[GLFW_KEY_K])
		{
			pointLightPositions[3].x -= 0.1f;
			pointLightPositions[3].y -= 0.1f;
			pointLightPositions[3].z -= 0.1f;
		}

	}
}

//FUNCION ANIMACIÓN
//En esta funcion se va a hacer el conteo de los frames (pasos) de cada uno de los discos
void animacion()
{

	//Movimiento del personaje

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			posX1 += KeyFrame[playIndex].incX1;
			posY1 += KeyFrame[playIndex].incY1;
			posZ1 += KeyFrame[playIndex].incZ1;

			posX2 += KeyFrame[playIndex].incX2;
			posY2 += KeyFrame[playIndex].incY2;
			posZ2 += KeyFrame[playIndex].incZ2;


			i_curr_steps++;
		}

	}
}

//En la siguiente funcion se establecen las teclas con las cuales se van a guardar cada uno de los frames
//Asimismo se establece la tecla con la que se va a ejecutar la animación

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

//Por ultimo tenemos una funcion que movera la camara en cuanto a orientacion mediante el raton (mouse)
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}