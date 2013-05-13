//#include "stdafx.h"
#include "myLoadObj.h"

using namespace std;

void myLoadObj(char* filename, vis::Model* model)
{
	ifstream in(filename); // , ios::in);
	if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

	string line;
	istringstream ss;
	cv::Vec3f vec3f;
	cv::Vec3i vec3i;
	cv::Vec2f vec2f;

	while (getline(in, line))
	{
		// Vertex
		if (line.substr(0, 2) == "v ")
		{
			ss = istringstream(line.substr(2));

			// x
			ss >> vec3f[0];
			// y
			ss >> vec3f[1];
			// z
			ss >> vec3f[2];

			model->vertexArray.push_back(vec3f);

			model->numberOfVertices++;
		}
		// Face
		else if (line.substr(0, 2) == "f ")
		{
			ss = istringstream(line.substr(2));
			unsigned int a,b,c;
			string crap;

			// First Vertex
			ss >> a;

			// Ta bort allt tom nästa blankslag
			if (ss.peek() == '/')
			{
				ss >> crap; 
			}

			// Second Vertex
			ss >> b;

			// Ta bort allt tom nästa blankslag
			if (ss.peek() == '/')
			{
				ss >> crap; 
			}

			// Third Vertex
			ss >> c;

			// Ta bort allt tom nästa blankslag
			if (ss.peek() == '/')
			{
				ss >> crap; 
			}

			// .obj indeces start at 1. Vector start at 0
			a--;
			b--;
			c--;

			vec3i[0] = a;
			vec3i[1] = b;
			vec3i[2] = c;

			model->indexArray.push_back(vec3i);

			model->numberOfIndices += 3;
		}
		// Vertex Normal
		else if (line.substr(0, 2) == "vn")
		{
			ss = istringstream(line.substr(3));
			// x
			ss >> vec3f[0];
			// y
			ss >> vec3f[1];
			// z
			ss >> vec3f[2];
			
			model->normalArray.push_back(vec3f);
		}
		// Vertex Texture
		else if (line.substr(0, 2) == "vt")
		{
			ss = istringstream(line.substr(2));
			// u
			ss >> vec2f[0];
			// v
			ss >> vec2f[1];
			
			model->texCoordArray.push_back(vec2f);
		}
		else if (line[0] == '#')
		{
			// This line is a comment
		}
		else
		{
			// Some case not suported at the moment
		}
	}

	cout << filename << " is succsessfully loaded" << endl;
}