#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <string>
#include <vector>
#include <fbxsdk.h>

struct SimpleFBXMesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
SimpleFBXMesh fbxModel;
bool fbxLoaded = false;0


float cameraYaw = -90.0f;  
float cameraPitch = 0.0f;
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;

GLuint cubeTexture;  
GLuint planeTexture;
GLuint frontTex, backTex, topTex, bottomTex, leftTex, rightTex, planeTex;
GLuint skyTex;

GLuint skyFront, skyBack, skyTop, skyBottom, skyLeft, skyRight;

GLuint loadTexture(const char* path) {

      int width, height, nrChannels;

      unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
      
      if (!data) {
          printf("Failed to load texture: %s\n", path);
          return 0;
      }

      size_t memoryUsage = width * height * nrChannels;
      printf("Texture %s: %dx%d, %d channels, ~%zu bytes\n", 
              path, width, height, nrChannels, memoryUsage);

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      
      stbi_image_free(data);
      return texture;
}

void drawSkybox() {
    float skySize = 10.0f;
    float textureRepeat = 0.650f;
    
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);  
    glColor3f(1.0f, 1.0f, 1.0f);

    // Front
    glBindTexture(GL_TEXTURE_2D, skyFront);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, -skySize);
    glEnd();

    // Back  
    glBindTexture(GL_TEXTURE_2D, skyBack);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glEnd();

    // Top
    glBindTexture(GL_TEXTURE_2D, skyTop);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, skySize, -skySize);
    glEnd();

    // Bottom
    glBindTexture(GL_TEXTURE_2D, skyBottom);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(-skySize, -skySize, skySize);
    glEnd();

    // Right
    glBindTexture(GL_TEXTURE_2D, skyRight);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(skySize, -skySize, -skySize);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(skySize, skySize, -skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(skySize, skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(skySize, -skySize, skySize);
    glEnd();

    // Left
    glBindTexture(GL_TEXTURE_2D, skyLeft);
    glBegin(GL_QUADS);
    glTexCoord2f(textureRepeat, textureRepeat); glVertex3f(-skySize, -skySize, -skySize);
    glTexCoord2f(0.0f, textureRepeat); glVertex3f(-skySize, -skySize, skySize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySize, skySize, skySize);
    glTexCoord2f(textureRepeat, 0.0f); glVertex3f(-skySize, skySize, -skySize);
    glEnd();

    glPopAttrib();
}
void drawPlane() {
    glBindTexture(GL_TEXTURE_2D, planeTex); 
    
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f); 
    
    float tileAmount = 10.0f;
    
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -0.5f, -10.0f);
    glTexCoord2f(tileAmount, 0.0f); glVertex3f(10.0f, -0.5f, -10.0f);
    glTexCoord2f(tileAmount, tileAmount); glVertex3f(10.0f, -0.5f, 10.0f);
    glTexCoord2f(0.0f, tileAmount); glVertex3f(-10.0f, -0.5f, 10.0f);
    
    glEnd();
}

void drawAxes() {
    glBegin(GL_LINES);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -10.0f, 0.0f);
    glVertex3f(0.0f, 10.0f, 0.0f);
    
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -10.0f);
    glVertex3f(0.0f, 0.0f, 10.0f);
    
    glEnd();
}


void drawCube(float cubeSize = 0.5f) {
    float s = cubeSize;
    
    glBindTexture(GL_TEXTURE_2D, frontTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, backTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, topTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, -s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, bottomTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, -s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, -s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, rightTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(s, s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, s);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, leftTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
    glEnd();
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = 400, lastY = 300;
    static bool firstMouse = true;
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    cameraYaw += xoffset;
    cameraPitch += yoffset;
    
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

void checkBufferObject(GLuint buffer) {
    GLint size;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    printf("Buffer size: %d bytes\n", size);
    
    GLint usage;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    printf("Buffer usage: %d\n", usage);
}

int main() {

    if (!glfwInit()) {
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Opengl sandbox", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwMakeContextCurrent(window);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);  
    glDisable(GL_CULL_FACE);

    std::string p = "assets/textures/block/";

    frontTex = loadTexture((p+"grass_block_side.png").c_str());
    backTex = loadTexture((p+"grass_block_side.png").c_str()); 
    topTex = loadTexture((p+"grass_block_side.png").c_str());
    bottomTex = loadTexture((p+"grass_block_side.png").c_str()); 
    leftTex = loadTexture((p+"grass_block_side.png").c_str());
    rightTex = loadTexture((p+"grass_block_side.png").c_str());

    planeTex = loadTexture((p+"cobblestone.png").c_str());
    skyTex = loadTexture("assets/textures/environment/clouds.png");


    std::string skyPath = "assets/textures/environment/";
    skyFront = loadTexture((skyPath+"clouds.png").c_str());
    skyBack = loadTexture((skyPath+"clouds.png").c_str());
    skyTop = loadTexture((skyPath+"clouds.png").c_str());
    skyBottom = loadTexture((skyPath+"clouds.png").c_str());
    skyLeft = loadTexture((skyPath+"clouds.png").c_str());
    skyRight = loadTexture((skyPath+"clouds.png").c_str());

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        float aspect = 800.0f / 600.0f;
        glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);
        
        glRotatef(-cameraPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-cameraYaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-cameraX, -cameraY, -cameraZ);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        drawSkybox();

        drawPlane();
        drawCube();
        drawAxes();
        
        glfwSwapBuffers(window);

        float cameraSpeed = 0.1f;
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraX += sin(glm::radians(cameraYaw)) * cameraSpeed;
            cameraZ -= cos(glm::radians(cameraYaw)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraX -= sin(glm::radians(cameraYaw)) * cameraSpeed;
            cameraZ += cos(glm::radians(cameraYaw)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraX -= cos(glm::radians(cameraYaw)) * cameraSpeed;
            cameraZ -= sin(glm::radians(cameraYaw)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraX += cos(glm::radians(cameraYaw)) * cameraSpeed;
            cameraZ += sin(glm::radians(cameraYaw)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            cameraY -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            cameraY += cameraSpeed;
        } 
        glfwPollEvents();
    }
}
