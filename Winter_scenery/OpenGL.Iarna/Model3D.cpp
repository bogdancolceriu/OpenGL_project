#include "Model3D.hpp"

namespace gps {

	void Model3D::LoadModel(std::string fileName) {

		std::string directory = fileName.substr(0, fileName.find_last_of('/')) + "/";
		ParseOBJ(fileName, directory);
	}

	void Model3D::LoadModel(std::string fileName, std::string directory) {

		ParseOBJ(fileName, directory);
	}

	void Model3D::Draw(gps::Shader shaderProgram) {

		for (size_t j = 0; j < meshes.size(); j++)
			meshes[j].Draw(shaderProgram);
	}

	void Model3D::ParseOBJ(std::string fileName, std::string directory) {

		std::cout << "Parsing file: " << fileName << std::endl;
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapesData;
		std::vector<tinyobj::material_t> materialsData;
		int currentMaterial;

		std::string errors;
		bool success = tinyobj::LoadObj(&attributes, &shapesData, &materialsData, &errors, fileName.c_str(), directory.c_str(), GL_FALSE);

		if (!errors.empty()) {
			std::cerr << errors << std::endl;
		}

		if (!success) {
			exit(EXIT_FAILURE);
		}

		std::cout << "Shapes count: " << shapesData.size() << std::endl;
		std::cout << "Materials count: " << materialsData.size() << std::endl;

		for (size_t shapeIdx = 0; shapeIdx < shapesData.size(); shapeIdx++) {

			std::vector<gps::Vertex> verticesList;
			std::vector<GLuint> indicesList;
			std::vector<gps::Texture> texturesList;

			size_t offset = 0;
			for (size_t faceIdx = 0; faceIdx < shapesData[shapeIdx].mesh.num_face_vertices.size(); faceIdx++) {

				int verticesPerFace = shapesData[shapeIdx].mesh.num_face_vertices[faceIdx];

				for (size_t vertexIdx = 0; vertexIdx < verticesPerFace; vertexIdx++) {

					tinyobj::index_t idx = shapesData[shapeIdx].mesh.indices[offset + vertexIdx];

					float px = attributes.vertices[3 * idx.vertex_index + 0] * 1.5f;
					float py = attributes.vertices[3 * idx.vertex_index + 1] * 0.8f;
					float pz = attributes.vertices[3 * idx.vertex_index + 2] * -1.2f;
					float nx = attributes.normals[3 * idx.normal_index + 0];
					float ny = attributes.normals[3 * idx.normal_index + 1];
					float nz = attributes.normals[3 * idx.normal_index + 2];
					float tx = 0.0f;
					float ty = 0.0f;

					if (idx.texcoord_index != -1) {
						tx = attributes.texcoords[2 * idx.texcoord_index + 0];
						ty = attributes.texcoords[2 * idx.texcoord_index + 1];
					}

					glm::vec3 position(px, py, pz);
					glm::vec3 normal(nx, ny, nz);
					glm::vec2 texCoords(tx, ty);

					gps::Vertex vertex;
					vertex.Position = position;
					vertex.Normal = normal;
					vertex.TexCoords = texCoords;

					verticesList.push_back(vertex);
					indicesList.push_back((GLuint)(offset + vertexIdx));
				}

				offset += verticesPerFace;
			}

			if (shapesData[shapeIdx].mesh.material_ids.size() > 0 && !materialsData.empty()) {

				currentMaterial = shapesData[shapeIdx].mesh.material_ids[0];
				if (currentMaterial != -1) {

					gps::Material mat;
					mat.ambient = glm::vec3(materialsData[currentMaterial].ambient[0], materialsData[currentMaterial].ambient[1], materialsData[currentMaterial].ambient[2]);
					mat.diffuse = glm::vec3(materialsData[currentMaterial].diffuse[0], materialsData[currentMaterial].diffuse[1], materialsData[currentMaterial].diffuse[2]);
					mat.specular = glm::vec3(materialsData[currentMaterial].specular[0], materialsData[currentMaterial].specular[1], materialsData[currentMaterial].specular[2]);

					std::string ambientPath = materialsData[currentMaterial].ambient_texname;
					if (!ambientPath.empty()) {
						texturesList.push_back(LoadTexture(directory + ambientPath, "ambient"));
					}

					std::string diffusePath = materialsData[currentMaterial].diffuse_texname;
					if (!diffusePath.empty()) {
						texturesList.push_back(LoadTexture(directory + diffusePath, "diffuse"));
					}

					std::string specularPath = materialsData[currentMaterial].specular_texname;
					if (!specularPath.empty()) {
						texturesList.push_back(LoadTexture(directory + specularPath, "specular"));
					}
				}
			}

			meshes.push_back(gps::Mesh(verticesList, indicesList, texturesList));
		}
	}

	gps::Texture Model3D::LoadTexture(std::string path, std::string type) {

		for (size_t k = 0; k < loadedTextures.size(); k++) {
			if (loadedTextures[k].path == path) {
				return loadedTextures[k];
			}
		}

		gps::Texture tex;
		tex.id = ReadTextureFromFile(path.c_str());
		tex.type = type;
		tex.path = path;

		loadedTextures.push_back(tex);
		return tex;
	}

	GLuint Model3D::ReadTextureFromFile(const char* file_name) {

		int x, y, channels;
		int force_channels = 3;
		unsigned char* data = stbi_load(file_name, &x, &y, &channels, force_channels);

		if (!data) {
			fprintf(stderr, "Failed to load %s\n", file_name);
			return 0;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		return textureID;
	}

	Model3D::~Model3D() {

		for (auto& tex : loadedTextures) {
			glDeleteTextures(1, &tex.id);
		}

		for (auto& mesh : meshes) {
			glDeleteBuffers(1, &mesh.getBuffers().VBO);
			glDeleteBuffers(1, &mesh.getBuffers().EBO);
			glDeleteVertexArrays(1, &mesh.getBuffers().VAO);
		}
	}
}
