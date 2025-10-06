#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// g++ -o main main.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./main

// g++ -o main main.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo && ./main

float cameraYaw = -90.0f;  
float cameraPitch = 0.0f;
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;

void drawPlane() {
    glBegin(GL_QUADS);

//  glColor3f(0.5f, 0.5f, 0.5f);
    glColor3f(0.0f, 0.5f, 0.0f); 

    glVertex3f(-10.0f, -0.5f, -10.0f);
    glVertex3f(10.0f, -0.5f, -10.0f);
    glVertex3f(10.0f, -0.5f, 10.0f);
    glVertex3f(-10.0f, -0.5f, 10.0f);

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

void drawTriangle() {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.5f, 0.2f); 
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
}

void drawCube() {
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
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

int main() {

//  double lastMouseX = 400, lastMouseY = 300;
//  bool firstMouse = true;
//  float cameraSensitivity = 0.1f;

    if (!glfwInit()) {
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle - macOS", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);
    
//  while (!glfwWindowShouldClose(window)) {
//      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//      
//      glMatrixMode(GL_PROJECTION);
//      glLoadIdentity();
//      glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
//      glMatrixMode(GL_MODELVIEW);
//      glLoadIdentity();
//      
//    //static float rotationAngle = 0.0f;
//    //rotationAngle += 1.0f;
//    //glRotatef(rotationAngle, 1.0f, 1.0f, 0.5f);
//     	drawCube();
//      glfwSwapBuffers(window);
//      glfwPollEvents();
//  }
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Sky blue
                                                 //
//      glClearColor(0.68f, 0.85f, 0.90f, 1.0f); // Light sky blue
//      glClearColor(0.0f, 0.75f, 1.0f, 1.0f);   // Deep sky blue

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        float aspect = 800.0f / 600.0f;
        glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 100.0f);
        
//      glMatrixMode(GL_MODELVIEW);
//      glLoadIdentity();
        
        glRotatef(-cameraPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-cameraYaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-cameraX, -cameraY, -cameraZ);
        
        drawPlane();
        drawCube();
        drawAxes();
        
        glfwSwapBuffers(window);

//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//         cameraZ -= 0.1f;
//     }
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//         cameraZ += 0.1f;
//     }
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//         cameraX -= 0.1f;
//     }
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//         cameraX += 0.1f;
//     }
//     if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
//         cameraY -= 0.1f;
//     }
//     if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//         cameraY += 0.1f;
//     }

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
    glfwTerminate();
}
