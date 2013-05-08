#include "Shader.h"


Shader::Shader(){
	
}

Shader::Shader(std::string vertexFilename, std::string fragmentFilename){

	assert(glCreateShader != NULL); //check if we have an initialized opengl
	programRef = 0;
	vertexShaderPath = vertexFilename;
	fragmentShaderPath = fragmentFilename;
	loadShader(vertexFilename, fragmentFilename);
}

GLchar* Shader::readFromFile(std::string filename){
	std::ifstream shaderFile;
	int length;

	shaderFile.open(filename, std::ios::binary);

	if(!shaderFile){
		std::cout << "shader file not found, ERROR ERROR ERROR" << std::endl;
		return NULL;
	}

	shaderFile.seekg(0,std::ios::end);
	length = (int)shaderFile.tellg();

	shaderFile.seekg(0,std::ios::beg);

	GLchar* buffer = new GLchar[length];
	shaderFile.read(buffer,length);
	shaderFile.close();

	buffer[length] = 0;	

	return buffer;
}

void Shader::loadShader(std::string vertexShaderFile, std::string fragmentShaderFile){
	GLchar* vertexText = readFromFile(vertexShaderFile);
	GLchar* fragmentText = readFromFile(fragmentShaderFile);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	

	glShaderSource(vertexShader, 1, (const GLchar **) &vertexText, NULL);
	glShaderSource(fragmentShader,1,(const GLchar **) &fragmentText, NULL);
	
	GLint status;

	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

	cout << "vertex status: " << status << endl;
	if (status == 0)
	{
		int loglen;
		char logbuffer[1000];
		glGetShaderInfoLog(vertexShader, sizeof(logbuffer), &loglen, logbuffer);
		fprintf(stderr, "OpenGL Shader Compile Error at %s:%d:\n%.*s", vertexShaderFile.data() , 0, loglen, logbuffer);
	}

	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

	cout << "fragment status: " << status << endl;
	if (status == 0)
	{
		int loglen;
		char logbuffer[1000];
		glGetShaderInfoLog(fragmentShader, sizeof(logbuffer), &loglen, logbuffer);
		fprintf(stderr, "OpenGL Shader Compile Error at %s:%d:\n%.*s", fragmentShaderFile.data() , 0, loglen, logbuffer);
	}

	//cout << "shader compile error is: 0x" << hex << glGetError() << endl;
	programRef = glCreateProgram();

	glAttachShader(programRef, vertexShader);
	glAttachShader(programRef, fragmentShader);
	//cout << "shader attach error is: 0x" << hex << glGetError() << endl;
	glLinkProgram(programRef);
	//cout << "shader link error is: 0x" << hex << glGetError() << endl;

	GLint linkStatus = 10;
	glGetProgramiv(programRef, GL_LINK_STATUS, &linkStatus);

	if(linkStatus == 0){
		cout << "shader linkage fail, l2shader n00b" << endl;
		cout << "link status was: " << linkStatus << endl;
		cout << "opengl error after linkage is: " << hex << glGetError << endl;
	}
	

	cout << "program status: " << linkStatus << endl;
	

	assert(linkStatus != 0);
	//cout << "link status is: " << linkStatus << endl;

	glUseProgram(programRef);

	//cout << "glUseProgram error is: 0x" << hex << glGetError() << endl;


	//should we deallocate memory used for shader text?
	//delete vertexText;
	//delete fragmentText;
}


Shader::~Shader(void)
{
}
