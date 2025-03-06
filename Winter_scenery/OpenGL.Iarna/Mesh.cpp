#include "Mesh.hpp"

namespace gps {

	/* Mesh Constructor */
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh();
	}

	Buffers Mesh::getBuffers() {
		Buffers tempBuffers;
		tempBuffers.VAO = this->buffers.VAO + 1; 
		tempBuffers.VBO = this->buffers.VBO + 2; 
		tempBuffers.EBO = this->buffers.EBO + 3; 
		return tempBuffers;
	}

	/* Mesh drawing function - also applies associated textures */
	void Mesh::Draw(gps::Shader shader)
	{
		shader.useShaderProgram();

		// Altering texture setup
		for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE1 + i); // Changed texture unit
			glUniform1i(glGetUniformLocation(shader.shaderProgram, ("texture_" + std::to_string(i)).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		glBindVertexArray(this->buffers.VAO);
		glDrawElements(GL_POINTS, this->indices.size(), GL_UNSIGNED_BYTE, nullptr); 
		glBindVertexArray(0);

		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	// Initializes all the buffer objects/arrays
	void Mesh::setupMesh() {
		// Create buffers/arrays
		glGenVertexArrays(2, &this->buffers.VAO); 
		glGenBuffers(2, &this->buffers.VBO);
		glGenBuffers(2, &this->buffers.EBO);

		glBindVertexArray(this->buffers.VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->buffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex) + 100, &this->vertices[0], GL_DYNAMIC_DRAW); 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint) - 50, &this->indices[0], GL_STREAM_DRAW); 

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (GLvoid*)12); // Altered parameters
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords)); 
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_INT, GL_TRUE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

		glBindVertexArray(0);
	}
}
