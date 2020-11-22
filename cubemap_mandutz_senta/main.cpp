//demonstration of the different features of cubemaps by Senta Mandutz
//sources: https://learnopengl.com/ and https://stackoverflow.com/questions/60686457/issue-in-drawing-ellipsoid-with-opengl?noredirect=1&lq=1 
//for the making of spheres
//in the controlls.pdf the keys to change the rerndering are displayed

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <glm/gtc/matrix_inverse.hpp>

int mapNum = 0;
int shaderNum = 0;
int objectNum = 0;
bool cursor = 0;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);									//camera position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);								//direction vector
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);									//positve y axis of camera

float deltaTime = 0.0f;																//time between current and last frame
float lastFrame = 0.0f;																//time of last frame

float lastX = 400, lastY = 300;														//center of frame
float yaw;
float pitch;
bool firstMouse = true;

float Zoom = 45.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);												//position of light source

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;															//creation and binding of texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)									//iterate through all 6 faces
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,			//loading image data with stb_image
			&nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,			//call far each face 
				width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap failed to load at path: " << faces[i]
				<< std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			//wraping specification
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,							//set all 3 texture dimensions on clamp to edge to fill space between 2 faces
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
		GL_CLAMP_TO_EDGE);
	return textureID;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);											//positions
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		
		glm::vec2 uv1(0.0f, 1.0f);													//texture coordinates
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		
		glm::vec3 nm(0.0f, 0.0f, 1.0f);												//normal vector

		glm::vec3 edge1 = pos2 - pos1;												//triangle 1
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		edge1 = pos3 - pos1;														//triangle 2
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		float quadVertices[] = {
			// positions            // normal         // texcoords  
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y
		};
		
		glGenVertexArrays(1, &quadVAO);												//configure plane VAO
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO;
void renderCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// positions			 // normals				// texture coords
			-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,		 0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
			0.5f, 0.5f, -0.5f,		 0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
			0.5f, 0.5f, -0.5f,		 0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f,		 0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
			0.5f, -0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
			0.5f, 0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
			0.5f, 0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f,		 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f,		 -1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f,		 -1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	 -1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	 -1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f,		 -1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f,		 -1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

			0.5f, 0.5f, 0.5f,		 1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			0.5f, 0.5f, -0.5f,		 1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,		 1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,		 1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			0.5f, -0.5f, 0.5f,		 1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			0.5f, 0.5f, 0.5f,		 1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,		 0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
			0.5f, -0.5f, 0.5f,		 0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			0.5f, -0.5f, 0.5f,		 0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f,		 0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f,		0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f,		 0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
			0.5f, 0.5f, -0.5f,		 0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
			0.5f, 0.5f, 0.5f,		 0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
			0.5f, 0.5f, 0.5f,		 0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f,		 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f,		 0.0f, 1.0f, 0.0f,		0.0f, 1.0f
		};

		glGenVertexArrays(1, &cubeVAO);												//generate VAO
		glGenBuffers(1, &cubeVBO);													//generate VBO

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);										//bind buffer to target
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//copy vertex data into buffer's memory

		glBindVertexArray(cubeVAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),			//how to interpret vertex data: (what attribute to configure, size of vertex attribute, type of data, data to be normalized, stride from one to next vertex, offset in buffer)
			(void*)0);	
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),			//how to interpret vertex data: (what attribute to configure, size of vertex attribute, type of data, data to be normalized, stride from one to next vertex, offset in buffer)
			(void*)(3 * sizeof(float)));	
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),			//how to interpret vertex data: (what attribute to configure, size of vertex attribute, type of data, data to be normalized, stride from one to next vertex, offset in buffer)
			(void*)(6 * sizeof(float)));	
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int sphereVAO = 0;
unsigned int sphereVBO[4];
void renderSphere()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	float M_PI = 3.14159265359;
	int stacks = 50;
	int	slices = 50;
	float radius = 0.5;
	if (sphereVAO == 0)
	{
	
		for (int i = 0; i <= stacks; ++i)
		{
			float V = i / (float)stacks;											//V texture coordinate
			float phi = V * M_PI;

			for (int j = 0; j <= slices; ++j) 
			{
				float U = j / (float)slices;										//U texture coordinate
				float theta = U * 2.0f * M_PI;

				float X = cos(theta) * sin(phi);
				float Y = cos(phi);
				float Z = sin(theta) * sin(phi);

				positions.push_back(glm::vec3(X, Y, Z) * radius);
				normals.push_back(glm::vec3(X, Y, Z));
				textureCoords.push_back(glm::vec2(U, V));

			}

		}

		std::vector<GLuint> indicies;												//generate the index buffer

		int noPerSlice = slices + 1;

		for (int i = 0; i < stacks; ++i) {

			for (int j = 0; j < slices; ++j) {

				int start_i = (i * noPerSlice) + j;

				indicies.push_back(start_i);
				indicies.push_back(start_i + noPerSlice + 1);
				indicies.push_back(start_i + noPerSlice);

				indicies.push_back(start_i + noPerSlice + 1);
				indicies.push_back(start_i);
				indicies.push_back(start_i + 1);

			}

		}

		glGenVertexArrays(1, &sphereVAO);
		glBindVertexArray(sphereVAO);

		glGenBuffers(4, sphereVBO);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), textureCoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(sphereVAO);

	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, (slices * stacks + slices) * 6, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

unsigned int skyboxVAO = 0;
unsigned int skyboxVBO;
void renderSkybox(unsigned int skyboxTexture)
{
	if (skyboxVAO == 0)
	{
		

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &skyboxVAO);											//generate VAO
		glGenBuffers(1, &skyboxVBO);												//generate VBO

		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);									//bind buffer to target
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,		//copy vertex data into buffer's memory
			GL_STATIC_DRAW);		

		glBindVertexArray(skyboxVAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),			//how to interpret vertex data: (what attribute to configure, size of vertex attribute, type of data, data to be normalized, stride from one to next vertex, offset in buffer)
			(void*)0);	
		glEnableVertexAttribArray(0);

	}

	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)			//resize window function
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)												//check for key presses
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)							//check if ESC was pressed; if true close window
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 2.5f * deltaTime;										//camera speed
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		if (objectNum >= 3)
		{
			objectNum = 0;
		}
		else
		{
			objectNum++;
		}
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (shaderNum >= 2)
		{
			shaderNum = 0;
		}
		else
		{
			shaderNum++;
		}
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		if (cursor == 1)
		{
			cursor = 0;
		}
		else
		{
			cursor = 1;
		}
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		if (mapNum >= 4)
		{
			mapNum = 0;
		}
		else
		{
			mapNum++;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;													//calculate offset between last and current frame	
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;														//camera direction vector
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));			//right/left look of camera
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));										//looking up down
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

int main()
{
	glfwInit();																		//start glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);									//set opengl-Version to 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);					//use core profile
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);							//only for Mac OS X

	GLFWwindow* window = glfwCreateWindow(800, 600, "cubemap_mandutz_senta", 
		NULL, NULL);																//window width, height and name
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);													//make context of window main context of current thread
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))						//Glad is passed function to load the address function pointers
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);														//enables depth testing

	glViewport(0, 0, 800, 600);														//set size of rendering window

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);				//call resize function on every resize of window

	unsigned int cubeDiffuseMap = loadTexture("objecttextures/container2.png");
	unsigned int cubeSpecularMap = loadTexture("objecttextures/container2_specular.png");

	Shader normalShader("shaders/shader.vs",
		"shaders/shader.fs");

	std::string folder = "Witcher1";												//folder name of textures
	std::string format = ".png";
	std::vector<std::string> faces													//vector with 6 cubemap textures
	{
		folder + "/right" + format,
		folder + "/left" + format,
		folder + "/top" + format,
		folder + "/bottom" + format,
		folder + "/front" + format,
		folder + "/back" + format
	};
	unsigned int skyboxTexture = loadCubemap(faces);

	Shader SkyboxShader("shaders/shader_skybox.vs",
		"shaders/shader_skybox.fs");

	Shader refractionShader("shaders/refractionShader.vs",
		"shaders/refractionShader.fs");

	Shader reflectionShader("shaders/reflectionShader.vs",
		"shaders/reflectionShader.fs");

	unsigned int waterNormalMap = loadTexture("objecttextures/water_normal.jpg");
	unsigned int waterTwoNormalMap = loadTexture("objecttextures/water2normal.jpg");
	unsigned int iceNormalMap = loadTexture("objecttextures/ice.jpg");
	unsigned int circuitsNormalMap = loadTexture("objecttextures/circuits.jpg");
	unsigned int metalNormalMap = loadTexture("objecttextures/metal.jpg");
	unsigned int metalTwoNormalMap = loadTexture("objecttextures/metal2.jpg");

	refractionShader.use();
	refractionShader.setInt("normalMap", 0);

	while (!glfwWindowShouldClose(window))											//render loop
	{

		float currentFrame = glfwGetTime();											//sets delta time and currentFrame
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if(cursor == 1)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);			//hide/capture cursor
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		glfwSetCursorPosCallback(window, mouse_callback);							//callback function called when mouse moved

		glfwSetScrollCallback(window, scroll_callback);								//callback function called when scrolled

		processInput(window);														//check for key pressed

		glfwSetKeyCallback(window, key_callback);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);										//set state of clear color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);							//use state of clear color --> set it and reset depth buffer

		glm::mat4 view;
		view = glm::lookAt(cameraPos,												//requires position, target and up vector
			cameraPos + cameraFront,
			cameraUp);
		glm::mat4 proj = glm::perspective(glm::radians(Zoom),						//perspective projection matrix: (field of view, aspect ratio, near, far)
			800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);											//model matrix

		switch (shaderNum)
		{
		case 0:
			normalShader.use();														//every following rendering will use program

			normalShader.setInt("material.diffuse", 0);
			normalShader.setInt("material.specular", 1);
			normalShader.setFloat("material.shininess", 32.0f);
			normalShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);				
			normalShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);				
			normalShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
			normalShader.setVec3("lightPos", lightPos);
			normalShader.setVec3("viewPos", cameraPos);

			normalShader.setMat4("model", model);									//set uniforms for light source
			normalShader.setMat4("view", view);
			normalShader.setMat4("projection", proj);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cubeDiffuseMap);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cubeSpecularMap);
			break;
		case 1:
			reflectionShader.use();													//every following rendering will use program

			reflectionShader.setVec3("lightPos", lightPos);
			reflectionShader.setVec3("viewPos", cameraPos);

			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), //rotate the quad to show normal mapping from multiple directions
				glm::normalize(glm::vec3(1.0, 0.0, 1.0))); 

			reflectionShader.setMat4("model", model);								//set uniforms for light source
			reflectionShader.setMat4("view", view);
			reflectionShader.setMat4("projection", proj);

			glActiveTexture(GL_TEXTURE0);
			switch (mapNum)
			{
			case 0:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, metalNormalMap);
				break;
			case 1:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, metalTwoNormalMap);
				break;
			case 2:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, circuitsNormalMap);
				break;
			case 3:
				break;
			}
			
			break;
		case 2:
			refractionShader.use();													//every following rendering will use program

			refractionShader.setVec3("lightPos", lightPos);
			refractionShader.setVec3("viewPos", cameraPos);

			model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), //rotate the quad to show normal mapping from multiple directions
				glm::normalize(glm::vec3(1.0, 0.0, 1.0)));

			refractionShader.setMat4("model", model);								//set uniforms for light source
			refractionShader.setMat4("view", view);
			refractionShader.setMat4("projection", proj);

			
			switch (mapNum)
			{
			case 0:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, iceNormalMap);
				break;
			case 1:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, waterTwoNormalMap);
				break;
			case 2:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, waterNormalMap);
				break;
			case 3:
				break;
			}
			
			

			break;
		}
		switch (objectNum)
		{
		case 0:	
			
			renderQuad();

			break;
		case 1:
			

			renderCube();
			break;
		case 2:
			renderSphere();
			break;
		}

		glDepthFunc(GL_LEQUAL);														//depth function should let everything less tha or equal to 1.0 through
		SkyboxShader.use();
		
		view = glm::mat4(glm::mat3(glm::lookAt(cameraPos,							//remove translation section from matrix to stop the cubemap from moving
			cameraPos + cameraFront,
			cameraUp)));
		SkyboxShader.setMat4("view", view);
		SkyboxShader.setMat4("projection", proj);

		renderSkybox(skyboxTexture);
		
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);													//swap color buffer
		glfwPollEvents();															//check if events are triggered
	}

	glfwTerminate();																//delete/clean all glfw-resources
	return 0;
}