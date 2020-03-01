//
//  main.cpp
//  Visual effects of special relativity
//
//  Created by Antoni Wójcik on 26/03/2019.
//
//  This is a simple graphics engine to simulate visual effect of special relativity.
//  OpenGL 4.1 is used to handle the graphics. Window is created with GLFW and GLAD libraries; GLM library is used in the vector calculations as it is compatible with OpenGL. In the simulation it is assumed that the other frames are moving at a constant velocities relative to the observer (the objects in those frame can perform any transformation). The effects of special relativity (Lorentz transformation and Doppler shift for light) and finite speed of propagation of light are taken into account. The physical theory is derived in the presentation - the code uses the same notation. To change the the scenario visible on the scene, modify "setUpScene" function in "scene.h".
#define RETINA
//
//
//  The code was written with the help of the following tutorials and websites:
//  https://learnopengl.com
//  The source code for "mesh.h", "model.h" was taken directly from the https://learnopengl.com tutorial. The code for "shader.h" was also taken from the website, but was slightly modified (added an option to modify the code for the shader on the go). More details in the file headers.
//
//  Controls:
//  A,W,S,D       to move the camera
//  SHIFT         to speed up the camera
//  MOUSE         to rotate the camera
//  MOUSE SCROLL  to zoom the camera
//  MOUSE CLICK   to release/show and lock/hide cursor
//  ENTER         to take a screenshot - the result: "screenshot.tga" will appear in the screenshots folder
//  1             to show/hide coordinate system
//  2             to show/hide GUI
//  3             to set the speed of propagation of light to infinity/back to normal - to show where the objects actually are at a given time
//  4             to turn off/on doppler effect
//
//
//  THIS PROGRAM HAS ONLY BEEN TESTED ON MAC OS 10.15.2
//

// include OpenGL libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

// include libraries handling shaders, loading objects and GUI
#include "src/model.h"
#include "src/shader.h"
#include "src/camera.h"
#include "src/scene.h"
#include "src/gui.h"

#include <iostream>

// function declarations
void framebufferSizeCallback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow*);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void updateGUI(float camera_time);

// function that provides a fix for Mac OS 10.14+ initial black screen
#ifdef __APPLE__
bool mac_fixed = false;
void macWindowFix(GLFWwindow* window);
#endif

#ifdef RETINA
// dimensions of the viewport (they have to be multiplied by 2 at the retina displays)
unsigned int scr_width = 1600*2;
unsigned int scr_height = 900*2;
#else
unsigned int scr_width = 1600;
unsigned int scr_height = 900;
#endif
float scr_ratio = (float)scr_width/(float)scr_height;

// variables used in the main loop
float delta_time = 0.0f;
float last_frame_time = 0.0f;

// camera declaration
Camera camera(scr_ratio, glm::vec3(0.0f, 1.0f, 0.0f));
bool show_true_position = false;
bool toggling_true_position = false;

bool turn_off_doppler = false;
bool toggling_doppler = false;

// gui declaration and controls
GUI gui(scr_width, scr_height);
bool toggling_gui = false;
bool draw_gui = true;

// variables used in callbacks
bool mouse_first_check = true;
bool mouse_hidden = true;
float mouse_last_x = scr_width / 2.0f;
float mouse_last_y = scr_width / 2.0f;

// simulation controlls

// draw the true position of the object and its velocity
bool toggling_coordinates = false;
bool draw_coords = true;

// update time and time flow speed
bool toggling_time_flow = false;
bool update_time = false;
float time_flow_speed = 1.0f;

// fps counter variables
float fps_sum = 0.0f;
const int fps_steps = 5;
int fps_steps_counter = 0;

// screenshot variable
bool taking_screenshot = false;

int main(int argc, const char * argv[]) {
    
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // open GLFW window and make it the current context
    #ifdef RETINA
    GLFWwindow* window = glfwCreateWindow(scr_width/2, scr_height/2, "Special Relativity", NULL, NULL);
    #else
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "Special Relativity", NULL, NULL);
    #endif
    if(window == NULL) {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // set the viewport size and apply changes every time it is changed by a user
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // initalize GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // load the scene containing objects, test models and their shaders
    Scene scene;
    
    // load a font to the GUI
    gui.loadFont("assets/fonts/hack.ttf", 28);
    
    // optimise the program (turned off, as we want to see how some of the objects look from inside)
    //glEnable(GL_CULL_FACE);
    
    glEnable(GL_DEPTH_TEST);
    
    // render loop
    while(!glfwWindowShouldClose(window)) {
        #ifdef __APPLE__
        macWindowFix(window);
        #endif
        
        float currentFrameTime = glfwGetTime();
        delta_time = currentFrameTime - last_frame_time;
        last_frame_time = currentFrameTime;
        if(fps_steps_counter == fps_steps) {
            gui.updateText(FPS, std::to_string(int(glm::round(1.0f/(fps_sum/float(fps_steps))))));
            fps_steps_counter = 0;
            fps_sum = 0;
        }
        fps_sum += delta_time;
        fps_steps_counter++;
        
        processInput(window);
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        updateGUI(scene.time);
        
        if(!update_time) delta_time = 0.0f;
        
        scene.draw(&camera, scr_ratio, delta_time * time_flow_speed, show_true_position, turn_off_doppler);
        
        if(draw_coords) scene.drawPos(&camera, scr_ratio, show_true_position);
        
        if(draw_gui) gui.draw();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

std::string vec3_to_string(const glm::vec3 v) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

void updateGUI(float camera_time) {
    gui.updateText(CAMERA_POSITION, vec3_to_string(camera.position));
    gui.updateText(CAMERA_TIME, std::to_string(camera_time));
    gui.updateText(CAMERA_ORIENTATION, vec3_to_string(camera.getDirection()));
    gui.updateText(CAMERA_ANGLE, std::to_string(int(camera.getFov())));
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
    scr_ratio = (float)width/(float)height;
    camera.setAspectRatio(scr_ratio);
    gui.resize(width, height);
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.setRunning(true);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) camera.setRunning(false);
    
    if(glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) time_flow_speed += 0.1f;
    if(glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) time_flow_speed -= 0.1f;
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.move(FORWARD, delta_time);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.move(BACKWARD, delta_time);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.move(RIGHT, delta_time);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.move(LEFT, delta_time);
    
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if(!toggling_time_flow) update_time = !update_time;
        toggling_time_flow = true;
    } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        toggling_time_flow = false;
    
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if(!toggling_coordinates) draw_coords = !draw_coords;
        toggling_coordinates = true;
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
        toggling_coordinates = false;
    
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if(!toggling_gui) draw_gui = !draw_gui;
        toggling_gui = true;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
        toggling_gui = false;
    
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if(!toggling_true_position) show_true_position = !show_true_position;
        toggling_true_position = true;
    } else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
        toggling_true_position = false;
    
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if(!toggling_doppler && !show_true_position) turn_off_doppler = !turn_off_doppler;
        toggling_doppler = true;
    } else if(glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
        toggling_doppler = false;
    
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if(!taking_screenshot) camera.takeScreenshot(scr_width, scr_height);
        taking_screenshot = true;
    } else if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
        taking_screenshot = false;
}

void mouseCallback(GLFWwindow* window, double x_pos, double y_pos) {
    if(mouse_first_check) {
        mouse_last_x = x_pos;
        mouse_last_y = y_pos;
        mouse_first_check = false;
    }
    
    float x_offset = x_pos - mouse_last_x;
    float y_offset = mouse_last_y - y_pos;
    
    mouse_last_x = x_pos;
    mouse_last_y = y_pos;
    
    if(mouse_hidden) camera.rotate(x_offset, y_offset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if(mouse_hidden) {
            //glfwSetCursorPos(window, scr_width/2, scr_height/2); does not work
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        mouse_hidden = !mouse_hidden;
    }
}

void scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    camera.zoom(y_offset);
}

//for some reason on Mac OS 10.14+ OpenGL window will only display black color until it is resized for the first time. This function does that automatically
#ifdef __APPLE__
void macWindowFix(GLFWwindow* window) {
    if(!mac_fixed) {
        int x, y;
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowPos(window, ++x, y);
        
        glViewport(0, 0, scr_width, scr_height);
        mac_fixed = true;
    }
}
#endif
