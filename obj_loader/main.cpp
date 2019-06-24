// Example 1: Load and Print
//
// Load the data from the .obj then print it into a file
//	called e1Out.txt

// Iostream - STD I/O Library
#include <iostream>

// fStream - STD File I/O Library
#include <fstream>

// OBJ_Loader - .obj Loader
#include "tiny_obj_loader.h"

// Main function
int main(int argc, char* argv[])
{
	// Initialize Loader
	objl::Loader Loader;

	// Load .obj File
	bool loadout = Loader.LoadFile("vicviper12.obj");

	// Check to see if it loaded

	// If so continue
	if (loadout)
	{
		// Create/Open e1Out.txt
		std::ofstream file("vicviper12.h");

		// Go through each loaded mesh and out its contents
		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
		{
			// Copy one of the loaded meshes to be our current mesh
			objl::Mesh curMesh = Loader.LoadedMeshes[i];

			// Print Mesh Name
			file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

			// Print Vertices
			file << "int viperVertexCount = " << curMesh.Vertices.size() << "\n";

            file << "float viperVertices[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].Position.X << "," << curMesh.Vertices[j].Position.Y << "," << curMesh.Vertices[j].Position.Z;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";

            file << "float viperNormals[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].Normal.X << "," << curMesh.Vertices[j].Normal.Y << "," << curMesh.Vertices[j].Normal.Z;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";

            file << "float viperTexCoords[] = { \n";
			for (int j = 0; j < curMesh.Vertices.size(); j++)
			{
					file << curMesh.Vertices[j].TextureCoordinate.X << "," << curMesh.Vertices[j].TextureCoordinate.Y;
					if(j != curMesh.Vertices.size())
                        file << ", \n";
			}
			file << "}; \n";

//			// Print Indices
//			file << "Indices:\n";
//
//			// Go through every 3rd index and print the
//			//	triangle that these indices represent
//			for (int j = 0; j < curMesh.Indices.size(); j += 3)
//			{
//				file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
//			}
//
//			// Print Material
//			file << "Material: " << curMesh.MeshMaterial.name << "\n";
//			file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
//			file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
//			file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
//			file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
//			file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
//			file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
//			file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
//			file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
//			file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
//			file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
//			file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
//			file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";
//
//			// Leave a space to separate from the next mesh
//			file << "\n";
		}

		// Close File
		file.close();
	}
    else{
		// Create/Open e1Out.txt
		std::ofstream file("vicviper12.h");

		// Output Error
		file << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";

		// Close File
		file.close();
	}
}
