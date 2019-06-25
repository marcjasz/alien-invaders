// Example 1: Load and Print
//
// Load the data from the .obj then print it into a file
//	called e1Out.txt

// Iostream - STD I/O Library
#include <iostream>

// fStream - STD File I/O Library
#include <fstream>

// OBJ_Loader - .obj Loader
#include "obj_loader.h"

// Main function
int main(int argc, char* argv[])
{
	// Initialize Loader
	objl::Loader Loader;

	// Load .obj File
	bool loadout = Loader.LoadFile("SwordfishII.obj");

	// Check to see if it loaded

	// If so continue
	if (loadout)
	{
		// Create/Open e1Out.txt
		std::ofstream file("SwordfishII.h");

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			// Copy one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			// Print Mesh Name

			// Print Vertices
			file << "int swordfishVertexCount = " << curMesh.Vertices.size() << ";\n";

            file << "float swordfishVertices[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].Position.X << "," << curMesh.Vertices[j].Position.Y << "," << curMesh.Vertices[j].Position.Z;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";

            file << "float swordfishNormals[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].Normal.X << "," << curMesh.Vertices[j].Normal.Y << "," << curMesh.Vertices[j].Normal.Z;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";

            file << "float swordfishTexCoords[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].TextureCoordinate.X << "," << curMesh.Vertices[j].TextureCoordinate.Y;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";
		}

		// Close File
		file.close();
	}
    else{
		// Create/Open
		std::ofstream file("SwordfishII.h");

		// Output Error
		file << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";

		// Close File
		file.close();
	}
}
