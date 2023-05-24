
#include<filesystem>
using namespace std;



#include<math.h>
#include"Model.h"
#include "Camera.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glad/glad.h>

const unsigned int width = 800;
const unsigned int height = 800;


float skyboxVertices[] =
{
	//   Koordynaty skyboxa
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// prawy
	1, 2, 6,
	6, 5, 1,
	// lewy
	0, 4, 7,
	7, 3, 0,
	// gora
	4, 5, 6,
	6, 7, 4,
	// dol
	0, 3, 2,
	2, 1, 0,
	// tyl
	0, 1, 5,
	5, 4, 0,
	// przod
	3, 7, 6,
	6, 2, 3
};

float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main()
{
	// Inicjalizacja GLFW
	glfwInit();


	// Wersja opengl ktorej uzywamy : OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Okno 800x800
	GLFWwindow* window = glfwCreateWindow(width, height, "Rutkowska_Rynduch", NULL, NULL);
	// Error check
	if (window == NULL)
	{
		std::cout << "Nie mozna zaladowac okna GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);


	// Generates Shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader dustShader("dust.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	dustShader.Activate();
	glUniform4f(glGetUniformLocation(dustShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(dustShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);





	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));


	//Elementy inicjaliowna przez podanie do nich sciezki gdzie sie znajduja
	//std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string crowPath = "C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/models/crow/scene.gltf";
	std::string dustPath = "C:/Users/zuzan/Desktop/Grfika_Rutkowska_Rynduch/Elementy/models/dust/scene.gltf";

	// zaladowanie modeli
	Model crow((crowPath).c_str());


	// Stworzenie VAO, VBO, and EBO dla skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// obrazy dla cubemap
	std::string facesCubemap[6] =
	{
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/posx.jpg", //prawy
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/negx.jpg", //lewy
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/posy.jpg", //gora
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/negy.jpg", //dol
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/posz.jpg", //front
		"C:/Users/zuzan/Desktop/Grafika_Rutkowska_Rynduch/Elementy/skybox/negz.jpg",  //tyl
	};

	// Rekstura do cubemap
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// istatne by nie tworzy³y sie szpary miedzy teskturami
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// zaladowanie tekstur do cubemap
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Blad zaladowania tesktur: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	// Liczba utworzonych pylkow
	const unsigned int number = 10000;
	// promien kola
	float radius = 30.0f;
	// Jak bardzo daleko sa pylki od poczatku kola
	float radiusDeviation = 50.0f;

	std::vector <glm::mat4> instanceMatrix;

	for (unsigned int i = 0; i < number; i++)
	{
		// generacja kola
		float x = randf();
		float finalRadius = radius + randf() * radiusDeviation;
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

		// transformacje pred ich powieleniem
		glm::vec3 tempTranslation;
		glm::quat tempRotation;
		glm::vec3 tempScale;

		if (randf() > 0.5f)
		{
			tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
		}
		else
		{
			tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
		}
		// generacja randomowego rozmieszczenia
		tempRotation = glm::quat(1.0f, randf(), randf(), randf());
		// Generacja randomowej skali pylkow
		tempScale = 0.1f * glm::vec3(randf(), randf(), randf());


		// Inicjalizacja
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		trans = glm::translate(trans, tempTranslation);
		rot = glm::mat4_cast(tempRotation);
		sca = glm::scale(sca, tempScale);

		instanceMatrix.push_back(trans * rot * sca);
	}

	Model dust((dustPath).c_str(), number, instanceMatrix);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		// kolor tla
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// czyszcenie bufferow
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs (delete this if you have disabled VSync)
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);


		// kruk
		crow.Draw(shaderProgram, camera);
		// pyl
		dust.Draw(dustShader, camera);

		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// usuwanie stworonych obiektow
	shaderProgram.Delete();
	skyboxShader.Delete();
	// usuwanie okna przed zamknieciem programu
	glfwDestroyWindow(window);
	// Terminowanie GLFW pzred zamknieciem programu
	glfwTerminate();
	return 0;
}