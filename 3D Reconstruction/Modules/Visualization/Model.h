#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <opencv2\core\core.hpp>
#include <iomanip>
#include <assert.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "VisualisationDisplayCameraClass.h"

namespace vis{
	enum model_type{
		sphere,cube
	};
class Model
{
public:
	
	Model::Model();
	Model(enum model_type parameter);

	void generate_sphere(void);
	void generate_cube(void);
	void upload(void);
	void draw(GLuint program);
	
	//variables
	GLuint VAO,VBO,NBO,TBO,IBO,CBO;
	GLuint texture0;
	int numberOfIndices;
	int numberOfVertices;
	
	std::vector<cv::Vec3f> vertexArray, normalArray;
	std::vector<cv::Vec2f> texCoordArray;
	std::vector<cv::Vec3i> indexArray;

};

}