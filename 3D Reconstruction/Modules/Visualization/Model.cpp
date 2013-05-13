#include "Model.h"

namespace vis{

	Model::Model(){
		texture0 = 0;
		numberOfIndices = 0;
		numberOfVertices = 0;
	}

	Model::Model(enum model_type parameter){
		texture0 = 0;
		numberOfIndices = 0;
		numberOfVertices = 0;
		
		switch(parameter){
		case sphere:
			generate_sphere();
			cout << "generating sphere" << endl;
			break;
		case cube:
			generate_cube();
			cout << "generating cube" << endl;
			break;
		default:
			cout << "no type selected, FAILING" << endl;
			assert(false);
			break;
		}

		upload();
		
	}

	void Model::generate_cube(void){

		//front face
		vertexArray.emplace_back(cv::Vec3f(0,0,0));
		vertexArray.emplace_back(cv::Vec3f(1,0,0));
		vertexArray.emplace_back(cv::Vec3f(1,1,0));
		vertexArray.emplace_back(cv::Vec3f(0,1,0));

		//back face
		vertexArray.emplace_back(cv::Vec3f(0,1,1));
		vertexArray.emplace_back(cv::Vec3f(1,1,1));
		vertexArray.emplace_back(cv::Vec3f(1,0,1));
		vertexArray.emplace_back(cv::Vec3f(0,0,1));

		//front
		indexArray.emplace_back(cv::Vec3i(0,1,2));
		indexArray.emplace_back(cv::Vec3i(0,3,2));

		//bottom
		indexArray.emplace_back(cv::Vec3i(3,2,4));
		indexArray.emplace_back(cv::Vec3i(2,5,4));

		//back
		indexArray.emplace_back(cv::Vec3i(4,5,6));
		indexArray.emplace_back(cv::Vec3i(6,7,4));

		//left
		indexArray.emplace_back(cv::Vec3i(4,7,0));
		indexArray.emplace_back(cv::Vec3i(0,3,4));

		//top
		indexArray.emplace_back(cv::Vec3i(0,7,1));
		indexArray.emplace_back(cv::Vec3i(1,6,7));

		//right
		indexArray.emplace_back(cv::Vec3i(1,2,5));
		indexArray.emplace_back(cv::Vec3i(5,6,1));

		//normals
		normalArray.emplace_back(cv::Vec3f(0,0,1));
		normalArray.emplace_back(cv::Vec3f(0,0,1));

		normalArray.emplace_back(cv::Vec3f(0,0,-1));
		normalArray.emplace_back(cv::Vec3f(0,0,-1));

		//textureCoordinates
		texCoordArray.emplace_back(cv::Vec2f(0,0));
		texCoordArray.emplace_back(cv::Vec2f(1,1));

		numberOfIndices = indexArray.size() * 3;
	}

	void Model::generate_sphere(){

	cv::Vec3f vertex;
	cv::Vec2f texCoord;
	cv::Vec3i face;

	// Create a unit sphere with number of subdivisions

	// Auxilary varialbes
	double phi = 0, theta = 0;
	double pi = 3.1415;

	int subdivisions = 10;
	int numberOfVertices = 0;
	


	//Wrap it up with the top vertex
	vertex[0] = 0;
	vertex[1] = 1;
	vertex[2] = 0;
	vertexArray.push_back(vertex);
	normalArray.push_back(vertex);
	texCoord[0] = 0.5;
	//texCoord[1] = 0.975; // TexCoords larger than 0.975 ends up outside texture
	texCoord[1] = 1.0; // TexCoords larger than 0.975 ends up outside texture
	texCoordArray.push_back(texCoord);
	
	// Place vertices on sphere
	for (int i = 1; i < subdivisions - 1; i++)
	{
		// <= to get double vertives at seam 
		for (int j = 0; j <= subdivisions; j++)
		{
			// phi is angle around the y axis
			phi = j*2*pi/subdivisions;
			// theta is angle to the y axis
			theta = i*pi/subdivisions;

			vertex[0] = (float)(sin(theta)*cos(phi));
			vertex[1] = (float)cos(theta);
			vertex[2] = (float)(sin(theta)*sin(phi));

			vertexArray.push_back(vertex);
			// Due to the radius being one the normal is equal to the coresponding vertex
			normalArray.push_back(vertex);
			
			// Texture coordinates aref ound through phi and theta
			//texCoord[0] = (1 - (float)j/subdivisions) * 0.975 + 0.001;
			//texCoord[1] = (1 - (float)i/subdivisions) * 0.99;

			texCoord[0] = (1 - (float)j/subdivisions);
			texCoord[1] = (1 - (float)i/subdivisions);

			texCoordArray.push_back(texCoord);
		}
	}

	//Begin with the bottom vertex
	vertex[0] = 0;
	vertex[1] = -1;
	vertex[2] = 0;
	vertexArray.push_back(vertex);
	normalArray.push_back(vertex);
	texCoord[0] = 0.5;
	texCoord[1] = 0;
	texCoordArray.push_back(texCoord);

	// Now all vertices are placed in space
	// Combine these into Faces

	// Start with north pole
	for (int i = 0; i < subdivisions; i++)
	{
		face[0] = 0;
		face[1] = i + 1;
		face[2] = i + 2;
		indexArray.push_back(face);
		numberOfIndices += 3;
	}

	// offset is the number of indexes needed to acces the next vertex-row in the vertex vector;
	int offset = subdivisions + 1;

	// -3 to stop in before the south pole vertices get in the picture
	for (int i = 0; i < subdivisions - 3; i++)
	{
		for (int j = 0; j < subdivisions; j++)
		{
			// Face 1 - upper-left triangle
			face[0] = i*(offset) + 1 + j;
			face[1] = i*(offset) + 1 + offset + j;
			face[2] = i*(offset) + 1 + offset + j + 1;
			indexArray.push_back(face);
			numberOfIndices += 3;
			
			// Face 2 - lower-right triangle
			face[0] = i*(offset) + 1 + j;
			face[1] = i*(offset) + 1 + j + 1;
			face[2] = i*(offset) + 1 + offset + j + 1;
			indexArray.push_back(face);
			numberOfIndices += 3;
		}
	}

	// Finish with south pole
	int poleVertex = vertexArray.size() - 1;
	for (int i = 0; i < subdivisions; i++)
	{
		face[0] = poleVertex;
		face[1] = poleVertex - i - 1;
		face[2] = poleVertex - i - 2;
		indexArray.push_back(face);
		numberOfIndices += 3;
	}
	}


	void Model::upload(){

	//upload code starts here
	
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(cv::Vec3f), &vertexArray.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalArray.size()*sizeof(cv::Vec3f), &normalArray.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, texCoordArray.size()*sizeof(cv::Vec2f), &texCoordArray.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size()*sizeof(cv::Vec3i), &indexArray.front(), GL_STATIC_DRAW);

	}


	void Model::draw(GLuint program){

		if(texture0 != 0){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(program, "Tex0"), 0);
		}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(glGetAttribLocation(program, "vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, "vert"));

		glBindBuffer(GL_ARRAY_BUFFER, NBO);
		glVertexAttribPointer(glGetAttribLocation(program, "normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, "normal"));

		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		glVertexAttribPointer(glGetAttribLocation(program, "texCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, "texCoord"));
	
		glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);
	}

	

}