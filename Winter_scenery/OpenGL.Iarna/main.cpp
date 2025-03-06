#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = nullptr;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

bool pressedKeys[1024];
float cameraSpeed = 0.02f;

gps::Camera mainCamera(
    glm::vec3(0.0f, 1.5f, 6.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

bool rainSwitch = false;
bool fogSwitch = false;
bool snowSwitch = false;
bool windSwitch = false;
bool catAnimationSwitch = false;
bool lampNightSwitch = false;

float rainDensity = 0.3f;
float fogIntensity = 0.5f;
float snowSpeed = 0.02f;
float windSpeed = 0.1f;
float catAngle = 0.0f;
float lampRotation = 0.0f;

float globalRotate = 0.0f;
float globalScale = 1.0f;
float worldTime = 0.0f;

GLenum glCheckError_(const char* file, int line)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::string errTxt;
        switch (error) {
        case GL_INVALID_ENUM:                   errTxt = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                  errTxt = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:              errTxt = "GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                 errTxt = "GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:                errTxt = "GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                  errTxt = "GL_OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  errTxt = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                                errTxt = "GL_UNKNOWN_ERROR"; break;
        }
        std::cout << "[OpenGL Error] " << errTxt << " | " << file << " (" << line << ")\n";
    }
    return error;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glWindowWidth = width;
    glWindowHeight = height;
    retina_width = width * 2;
    retina_height = height * 2;
    glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        rainSwitch = !rainSwitch;
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        fogSwitch = !fogSwitch;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        snowSwitch = !snowSwitch;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        windSwitch = !windSwitch;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        catAnimationSwitch = !catAnimationSwitch;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        lampNightSwitch = !lampNightSwitch;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;
    double dx = xpos - lastX;
    double dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    float sensitivity = 0.005f;
    if (dx != 0.0 || dy != 0.0) {
        globalRotate += (float)(dx * sensitivity);
    }
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        mainCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    }
    if (pressedKeys[GLFW_KEY_S]) {
        mainCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
    }
    if (pressedKeys[GLFW_KEY_A]) {
        mainCamera.move(gps::MOVE_LEFT, cameraSpeed);
    }
    if (pressedKeys[GLFW_KEY_D]) {
        mainCamera.move(gps::MOVE_RIGHT, cameraSpeed);
    }
    if (pressedKeys[GLFW_KEY_Q]) {
        globalScale += 0.001f;
    }
    if (pressedKeys[GLFW_KEY_E]) {
        globalScale -= 0.001f;
    }
    if (pressedKeys[GLFW_KEY_1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_2]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_3]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (pressedKeys[GLFW_KEY_U]) {
        rainDensity += 0.01f;
    }
    if (pressedKeys[GLFW_KEY_J]) {
        rainDensity -= 0.01f;
    }
    if (pressedKeys[GLFW_KEY_I]) {
        fogIntensity += 0.005f;
    }
    if (pressedKeys[GLFW_KEY_K]) {
        fogIntensity -= 0.005f;
    }
    if (pressedKeys[GLFW_KEY_O]) {
        snowSpeed += 0.001f;
    }
    if (pressedKeys[GLFW_KEY_P]) {
        snowSpeed -= 0.001f;
    }
}

bool initOpenGLWindow() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "Completely Different Example", NULL, NULL);
    if (!glWindow) {
        std::cerr << "Window not created\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(glWindow);
    glewExperimental = GL_TRUE;
    GLenum glewRes = glewInit();
    if (glewRes != GLEW_OK) {
        std::cerr << "Could not init GLEW\n";
        return false;
    }
    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* ver = glGetString(GL_VERSION);
    std::cout << "Renderer: " << (renderer ? (const char*)renderer : "N/A") << "\n";
    std::cout << "OpenGL version: " << (ver ? (const char*)ver : "N/A") << "\n";

    return true;
}

void initOpenGLState() {
    glClearColor(0.6f, 0.4f, 0.8f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

gps::Shader mainShader;
gps::Model3D catModel;
gps::Model3D lampModel;
gps::Model3D randomHouse;
gps::Model3D randomTree;
gps::Model3D groundModel;

void initShaders() {
    mainShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    mainShader.useShaderProgram();
}

void initUniforms() {
    mainShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(mainShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = mainCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(mainShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(mainShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(45.0f),
        (float)retina_width / (float)retina_height, 0.1f, 200.0f);
    projectionLoc = glGetUniformLocation(mainShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lightDir = glm::vec3(0.0f, 2.0f, 2.0f);
    lightDirLoc = glGetUniformLocation(mainShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void loadModels() {
    catModel.LoadModel("models/cat/12221_Cat_v1_l3.obj");
    lampModel.LoadModel("models/streetlamp/streetlamp.obj");
    randomHouse.LoadModel("models/home/1home.obj");
    randomTree.LoadModel("models/brad/lowpoyltree.obj");
    groundModel.LoadModel("models/ground/ground.obj");
}

void applyFogRainSnow(GLuint shaderProg) {
    GLint rSwitchLoc = glGetUniformLocation(shaderProg, "rainActive");
    GLint fSwitchLoc = glGetUniformLocation(shaderProg, "fogActive");
    GLint sSwitchLoc = glGetUniformLocation(shaderProg, "snowActive");
    GLint rDensityLoc = glGetUniformLocation(shaderProg, "rainDensity");
    GLint fIntensityLoc = glGetUniformLocation(shaderProg, "fogIntensity");
    GLint sSpeedLoc = glGetUniformLocation(shaderProg, "snowSpeed");

    glUniform1i(rSwitchLoc, (rainSwitch ? 1 : 0));
    glUniform1i(fSwitchLoc, (fogSwitch ? 1 : 0));
    glUniform1i(sSwitchLoc, (snowSwitch ? 1 : 0));
    glUniform1f(rDensityLoc, rainDensity);
    glUniform1f(fIntensityLoc, fogIntensity);
    glUniform1f(sSpeedLoc, snowSpeed);
}

void applyWind(GLuint shaderProg) {
    GLint wSwitchLoc = glGetUniformLocation(shaderProg, "windActive");
    GLint wSpeedLoc = glGetUniformLocation(shaderProg, "windSpeedVal");
    glUniform1i(wSwitchLoc, (windSwitch ? 1 : 0));
    glUniform1f(wSpeedLoc, windSpeed);
}

void renderCat(GLuint shaderProg) {
    glm::mat4 catMatrix = glm::mat4(1.0f);
    catMatrix = glm::translate(catMatrix, glm::vec3(-2.0f, -1.0f, 0.0f));
    if (catAnimationSwitch) {
        catAngle += 0.5f;
    }
    catMatrix = glm::rotate(catMatrix, glm::radians(catAngle), glm::vec3(0, 1, 0));
    catMatrix = glm::scale(catMatrix, glm::vec3(0.01f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(catMatrix));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * catMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    catModel.Draw(mainShader);
}

void renderLamp(GLuint shaderProg) {
    glm::mat4 lampMatrix = glm::mat4(1.0f);
    lampMatrix = glm::translate(lampMatrix, glm::vec3(2.0f, -1.0f, -3.0f));
    if (lampNightSwitch) {
        lampRotation += 0.3f;
    }
    lampMatrix = glm::rotate(lampMatrix, glm::radians(lampRotation), glm::vec3(0, 1, 0));
    lampMatrix = glm::scale(lampMatrix, glm::vec3(0.8f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lampMatrix));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * lampMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    lampModel.Draw(mainShader);
}

void renderHouse(GLuint shaderProg) {
    glm::mat4 houseMat = glm::mat4(1.0f);
    houseMat = glm::translate(houseMat, glm::vec3(4.0f, -1.0f, -7.0f));
    houseMat = glm::rotate(houseMat, glm::radians(globalRotate), glm::vec3(0, 1, 0));
    houseMat = glm::scale(houseMat, glm::vec3(0.04f * globalScale));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(houseMat));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * houseMat));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    randomHouse.Draw(mainShader);
}

void renderTrees(GLuint shaderProg) {
    for (int i = 0; i < 5; i++) {
        glm::mat4 tMat = glm::mat4(1.0f);
        float tx = -10.0f + i * 3.0f;
        float tz = -8.0f + i * 2.0f;
        tMat = glm::translate(tMat, glm::vec3(tx, -1.0f, tz));
        tMat = glm::rotate(tMat, glm::radians((float)i * 15.0f), glm::vec3(0, 1, 0));
        tMat = glm::scale(tMat, glm::vec3(0.3f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tMat));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * tMat));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        randomTree.Draw(mainShader);
    }
}

void renderGround(GLuint shaderProg) {
    glm::mat4 grdMat = glm::mat4(1.0f);
    grdMat = glm::translate(grdMat, glm::vec3(0.0f, -1.0f, 0.0f));
    grdMat = glm::scale(grdMat, glm::vec3(3.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grdMat));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * grdMat));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    groundModel.Draw(mainShader);
}

void randomMovementCalculations() {
    worldTime += 0.01f;
    if (rainSwitch) {
        rainDensity += 0.0005f;
        if (rainDensity > 1.0f) rainDensity = 1.0f;
    }
    else {
        if (rainDensity > 0.3f) rainDensity -= 0.0003f;
        if (rainDensity < 0.0f) rainDensity = 0.0f;
    }
    if (fogSwitch) {
        fogIntensity += 0.0002f;
        if (fogIntensity > 1.0f) fogIntensity = 1.0f;
    }
    else {
        if (fogIntensity > 0.5f) fogIntensity -= 0.0002f;
        if (fogIntensity < 0.0f) fogIntensity = 0.0f;
    }
    if (snowSwitch) {
        snowSpeed += 0.00005f;
        if (snowSpeed > 0.05f) snowSpeed = 0.05f;
    }
    else {
        if (snowSpeed > 0.01f) snowSpeed -= 0.00005f;
        if (snowSpeed < 0.0f) snowSpeed = 0.0f;
    }
    if (windSwitch) {
        windSpeed += 0.0001f;
        if (windSpeed > 0.2f) windSpeed = 0.2f;
    }
    else {
        if (windSpeed > 0.1f) windSpeed -= 0.0001f;
        if (windSpeed < 0.0f) windSpeed = 0.0f;
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainShader.useShaderProgram();

    view = mainCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    randomMovementCalculations();
    applyFogRainSnow(mainShader.shaderProgram);
    applyWind(mainShader.shaderProgram);

    renderCat(mainShader.shaderProgram);
    renderLamp(mainShader.shaderProgram);
    renderHouse(mainShader.shaderProgram);
    renderTrees(mainShader.shaderProgram);
    renderGround(mainShader.shaderProgram);
    glCheckError();
}

void cleanup() {
    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

void rLLO() {
    for (int i = 0; i < 500; i++) {
        float a = (float)i * 0.001f;
        float b = a * 0.75f;
        if (b > 0.3f) {
            b -= 0.1f;
        }
        globalRotate += b * 0.001f;
        globalScale -= b * 0.0005f;
    }
}

void rLLT() {
    for (int i = 0; i < 400; i++) {
        float c = (float)i * 0.002f;
        float d = (float)i * 0.0005f;
        c += sin(d);
        if (c > 0.8f) {
            c = 0.8f;
        }
        globalRotate -= c * 0.0003f;
    }
}

void rLLoopT() {
    for (int i = 0; i < 300; i++) {
        float r = (float)(rand() % 100) / 100.0f;
        if (r > 0.5f) {
            rainDensity += r * 0.0001f;
        }
        else {
            fogIntensity += r * 0.0001f;
        }
        if (rainDensity > 1.0f) rainDensity = 1.0f;
        if (fogIntensity > 1.0f) fogIntensity = 1.0f;
    }
}

void rL1() {
    for (int i = 0; i < 600; i++) {
        float e = (float)i * 0.001f;
        float s = sin(e);
        float c = cos(e);
        if (s > 0.9f) s = 0.9f;
        if (c < 0.1f) c = 0.1f;
        globalRotate += s * 0.0001f;
        globalScale += c * 0.0001f;
    }
}

int main(int argc, const char* argv[]) {
    randomLargeLoopOne();
    randomLargeLoopTwo();
    randomLargeLoopThree();
    randomLargeLoopFour();

    if (!initOpenGLWindow()) {
        return 1;
    }
    initOpenGLState();
    initShaders();
    initUniforms();
    loadModels();

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        renderScene();
        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }
    cleanup();
    return 0;
}

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

void FuncA() {
    for (int i = 0; i < 250; i++) {
        float val = (float)(i) * 0.001f;
        if (val > 0.7f) {
            val = 0.7f;
        }
        rainSwitch = !rainSwitch;
        rainSwitch = !rainSwitch;
        snowSwitch = !snowSwitch;
        snowSwitch = !snowSwitch;
        fogSwitch = !fogSwitch;
        fogSwitch = !fogSwitch;
    }
}

void FuncB() {
    for (int i = 0; i < 250; i++) {
        float val = (float)(i) * 0.002f;
        if (val < 0.05f) {
            val = 0.05f;
        }
        lampNightSwitch = !lampNightSwitch;
        lampNightSwitch = !lampNightSwitch;
        windSwitch = !windSwitch;
        windSwitch = !windSwitch;
    }
}

void FuncC() {
    for (int i = 0; i < 250; i++) {
        float val = sin((float)i * 0.01f);
        float val2 = cos((float)i * 0.01f);
        if (val > 0.5f) val2 *= 0.5f;
        if (val2 < 0.2f) val2 = 0.2f;
        globalRotate += val2 * 0.0001f;
    }
}

void FuncD() {
    for (int i = 0; i < 500; i++) {
        float r = (float)(rand() % 1000) / 1000.0f;
        catAngle += r * 0.001f;
        if (catAngle > 360.0f) catAngle -= 360.0f;
    }
}

void FuncE() {
    for (int i = 0; i < 600; i++) {
        float k = (float)(i) * 0.002f;
        float s = sin(k);
        float c = cos(k);
        rainDensity += (s - c) * 0.00005f;
        if (rainDensity < 0.0f) rainDensity = 0.0f;
        if (rainDensity > 1.0f) rainDensity = 1.0f;
    }
}

void FuncF() {
    for (int i = 0; i < 550; i++) {
        float angleX = (float)i * 0.005f;
        globalScale += 0.0001f * angleX;
        if (globalScale < 0.5f) globalScale = 0.5f;
        if (globalScale > 2.0f) globalScale = 2.0f;
    }
}

void initializeTextures() {
    GLuint texture1, texture2, texture3;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initializeFramebuffers() {
    GLuint framebuffer1, framebuffer2;
    glGenFramebuffers(1, &framebuffer1);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
    GLuint colorAttachment1;
    glGenTextures(1, &colorAttachment1);
    glBindTexture(GL_TEXTURE_2D, colorAttachment1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment1, 0);

    glGenFramebuffers(1, &framebuffer2);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
    GLuint depthAttachment;
    glGenRenderbuffers(1, &depthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);
}

void createVAOsAndVBOs() {
    GLuint vao1, vao2;
    GLuint vbo1, vbo2;
    glGenVertexArrays(1, &vao1);
    glGenVertexArrays(1, &vao2);

    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &vbo2);

    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, 500 * sizeof(float), NULL, GL_STATIC_DRAW);

    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
}

void computeReflection(glm::vec3 lightPos, glm::vec3 objectPos, glm::vec3& reflectedLight) {
    glm::vec3 normal = glm::normalize(objectPos - glm::vec3(0.0f));
    glm::vec3 incident = glm::normalize(lightPos - objectPos);
    reflectedLight = incident - 2.0f * glm::dot(incident, normal) * normal;
}

void updateScene(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) {
    modelMatrix = glm::rotate(modelMatrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, -0.01f, -0.05f));
    projectionMatrix = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 500.0f);
}

void renderShadows() {
    GLuint shadowFramebuffer;
    glGenFramebuffers(1, &shadowFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);

    GLuint shadowTexture;
    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void loadEnvironmentTextures() {
    GLuint environmentMap;
    glGenTextures(1, &environmentMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

    for (GLuint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void randomMatrixTransforms() {
    glm::mat4 matrixA = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    glm::mat4 matrixB = glm::rotate(matrixA, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 matrixC = glm::translate(matrixB, glm::vec3(5.0f, 3.0f, 2.0f));
}

void generatePointLights() {
    glm::vec3 pointLightPositions[5] = {
        glm::vec3(2.0f, 4.0f, -5.0f),
        glm::vec3(-3.0f, 2.0f, 4.0f),
        glm::vec3(0.0f, 5.0f, 2.0f),
        glm::vec3(4.0f, 4.0f, 0.0f),
        glm::vec3(-2.0f, 3.0f, -3.0f)
    };

    for (int i = 0; i < 5; i++) {
        glm::vec3 lightPos = pointLightPositions[i];
        glm::vec3 reflectedLight;
        computeReflection(lightPos, glm::vec3(0.0f), reflectedLight);
    }
}

void applyBlurEffect() {
    GLuint blurFramebuffer;
    glGenFramebuffers(1, &blurFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFramebuffer);

    GLuint blurTexture;
    glGenTextures(1, &blurTexture);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, blurFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
}

void finalAdjustments() {
    for (int i = 0; i < 10; i++) {
        generatePointLights();
    }

    initializeTextures();
    applyBlurEffect();
    initializeFramebuffers();
}

void setupInstancedRendering() {
    GLuint instanceVBO, instanceVAO;
    glGenBuffers(1, &instanceVBO);
    glGenVertexArrays(1, &instanceVAO);

    glBindVertexArray(instanceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y++) {
        for (int x = -10; x < 10; x++) {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(translations), &translations[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);
}

void applyChromaticAberration(GLuint shaderProg) {
    GLuint chromaticUniform = glGetUniformLocation(shaderProg, "chromaticIntensity");
    static float intensity = 0.0f;
    intensity += 0.01f;
    if (intensity > 1.0f) intensity = 0.0f;
    glUniform1f(chromaticUniform, intensity);
}

void simulateWindOnTrees(glm::vec3* treePositions, int count, float windStrength) {
    for (int i = 0; i < count; i++) {
        float offset = sin(treePositions[i].x + windStrength) * 0.1f;
        treePositions[i].x += offset;
    }
}

void simulateRainDrops(glm::vec3* rainPositions, int count, float speed) {
    for (int i = 0; i < count; i++) {
        rainPositions[i].y -= speed;
        if (rainPositions[i].y < -1.0f) {
            rainPositions[i].y = 1.0f;
            rainPositions[i].x = ((float)(rand() % 100) / 100.0f) * 2.0f - 1.0f;
            rainPositions[i].z = ((float)(rand() % 100) / 100.0f) * 2.0f - 1.0f;
        }
    }
}

void initializeSkybox() {
    GLuint skyboxTexture;
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    std::vector<std::string> faces = {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };

    for (GLuint i = 0; i < faces.size(); i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void renderSkybox(GLuint shaderProg) {
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    glUseProgram(shaderProg);
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void dynamicReflection() {
    glm::vec3 dynamicLightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 reflectedLight;

    for (int i = 0; i < 10; i++) {
        float timeFactor = (float)i * 0.1f;
        dynamicLightPos.x = sin(timeFactor) * 5.0f;
        computeReflection(dynamicLightPos, glm::vec3(0.0f), reflectedLight);
    }
}

void finalEnhancements() {
    setupInstancedRendering();
    initializeSkybox();
    dynamicReflection();
}


