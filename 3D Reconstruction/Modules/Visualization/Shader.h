#pragma once

#include <Windows.h>
#include <gl\GLEW.h>
#include <gl\GLU.h>

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include <assert.h>

using namespace std;

class Shader
{
public:
	Shader();
	Shader(std::string vertexShaderFile, std::string fragmentShaderFile);
	~Shader(void);
	GLuint programRef;
	
	std::string vertexShaderPath, fragmentShaderPath;

private:
	char* readFromFile(std::string filename);
	void loadShader(std::string vertexShaderFile, std::string fragmentShaderFile);
};

