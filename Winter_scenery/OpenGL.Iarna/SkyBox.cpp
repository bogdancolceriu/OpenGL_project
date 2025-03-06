#include "SkyBox.hpp"

namespace gps {

    SkyBox::SkyBox() {

    }

    void SkyBox::Load(std::vector<const GLchar*> skyDomeFaces) {
        skyTexture = LoadSkyDomeTextures(skyDomeFaces);
        InitSkyDome();
    }

    void SkyBox::Draw(gps::Shader shader, glm::mat4 viewMatrix, glm::mat4 projMatrix) {
        shader.useShaderProgram();

        // Adjust view matrix for SkyDome
        glm::mat4 alteredView = glm::mat4(glm::mat3(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(alteredView));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

        glDepthFunc(GL_EQUAL);

        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader.shaderProgram, "skyTexture"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 48);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }

    GLuint SkyBox::LoadSkyDomeTextures(std::vector<const GLchar*> domeFaces) {
        GLuint texID;
        glGenTextures(1, &texID);
        glActiveTexture(GL_TEXTURE0);

        int width, height, numChannels;
        unsigned char* imgData;
        int enforceChannels = 4;

        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
        for (GLuint i = 0; i < domeFaces.size(); i++) {
            imgData = stbi_load(domeFaces[i], &width, &height, &numChannels, enforceChannels);
            if (!imgData) {
                fprintf(stderr, "FAILED: Unable to load %s\n", domeFaces[i]);
                return 0;
            }
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData
            );
            stbi_image_free(imgData);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return texID;
    }

    void SkyBox::InitSkyDome() {
        GLfloat skyVertices[] = {
            -1.5f,  1.5f, -1.5f,
            -1.5f, -1.5f, -1.5f,
             1.5f, -1.5f, -1.5f,
             1.5f,  1.5f, -1.5f,

             1.5f,  1.5f,  1.5f,
             1.5f, -1.5f,  1.5f,
            -1.5f, -1.5f,  1.5f,
            -1.5f,  1.5f,  1.5f,

            0.0f,  2.0f,  0.0f
        };

        GLuint domeIndices[] = {
            0, 1, 3, 3, 1, 2,
            3, 2, 4, 4, 2, 5,
            4, 5, 7, 7, 5, 6,
            7, 6, 0, 0, 6, 1,
            0, 3, 8, 3, 4, 8, 4, 7, 8, 7, 0, 8
        };

        glGenVertexArrays(1, &skyVAO);
        glGenBuffers(1, &skyVBO);
        glGenBuffers(1, &skyEBO);

        glBindVertexArray(skyVAO);

        glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(domeIndices), &domeIndices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glBindVertexArray(0);
    }

    GLuint SkyBox::GetTextureId() {
        return skyTexture;
    }
}
