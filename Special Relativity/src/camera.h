//
//  camera.h
//  Special Relativity
//
//  Created by Antoni Wójcik on 05/04/2019.
//  Copyright © 2019 Antoni Wójcik. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include <glad/glad.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

const float SPEED_NORMAL = 3.0f;
const float SPEED_FAST = 30.0f;
const float ZOOM_MIN = 1.0f;
const float ZOOM_MAX = 90.0f;

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.4f;
const float ZOOM = 45.0f;


enum CameraMovementDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
private:
    glm::vec3 u, v, w; //axes of the camera lens
    glm::vec3 global_y_dir;
    
    float yaw, pitch;
    
    float fov, aspect_ratio;
    
    bool run = false;
    
    int screenshot_counter = 0;
    
    glm::mat4 projection, view;
    
    inline void updateVectors() {
        w.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        w.y = sin(glm::radians(pitch));
        w.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        u = glm::normalize(glm::cross(w, global_y_dir));
        v = glm::normalize(glm::cross(u, w));
    }
    
    inline void updateMatrices() {
        projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 1000.0f);
        view = glm::lookAt(glm::vec3(0), w, v);
    }
public:
    glm::vec3 position;
    
    Camera(float ratio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : aspect_ratio(ratio), w(glm::vec3(0.0f, 0.0f, -1.0f)), fov(ZOOM) {
        this->position = position;
        global_y_dir = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateVectors();
        updateMatrices();
    }
    
    inline glm::vec3 getDirection() const {
        return w;
    }
    
    inline float getFov() const {
        return fov;
    }
    
    inline void transferData(Shader& shader) {
        shader.setMat4("PV", projection*view);
    }
    
    inline void move(CameraMovementDirection direction, float delta_time) {
        float displacement = (run ? SPEED_FAST : SPEED_NORMAL) * delta_time;
        if(direction == FORWARD)  position += w * displacement;
        if(direction == BACKWARD) position -= w * displacement;
        if(direction == RIGHT)    position += u * displacement;
        if(direction == LEFT)     position -= u * displacement;
        
        updateMatrices();
    }
    
    inline void setRunning(bool state) {
        run = state;
    }
    
    inline void rotate(float x_offset, float y_offset) {
        yaw +=   x_offset * SENSITIVITY * fov / 45.0f;
        pitch += y_offset * SENSITIVITY * fov / 45.0f;
    
        if(pitch > 89.0f)       pitch = 89.0f;
        else if(pitch < -89.0f) pitch = -89.0f;
        
        yaw = fmod(yaw, 360.0f);
        
        updateVectors();
        updateMatrices();
    }
    
    inline void zoom(float scroll) {
        if(fov >= ZOOM_MIN && fov <= ZOOM_MAX) fov -= scroll;
        if(fov <= ZOOM_MIN)      fov = ZOOM_MIN;
        else if(fov >= ZOOM_MAX) fov = ZOOM_MAX;
        
        updateMatrices();
    }
    
    inline void setAspectRatio(float ratio) {
        aspect_ratio = ratio;
    }
    
    // take a screenshot of a current frame and save it to a file
    inline void takeScreenshot(const short screenshot_width, const short screenshot_height, std::string name = "screenshot", bool show_image = false) {
        name += std::to_string(screenshot_counter);
        screenshot_counter++;
        
        std::cout << "Taking screenshot: " << name << ".tga" << std::endl;
        
        short TGA_header[] = {0, 2, 0, 0, 0, 0, screenshot_width, screenshot_height, 24};
        char* pixel_data = new char[3*screenshot_width*screenshot_height]; //there are 3 colors (RGB) for each pixel
        std::ofstream file("screenshots/" + name + ".tga", std::ios::out | std::ios::binary);
        if(!pixel_data || !file) {
            std::cerr << "ERROR: COULD NOT TAKE THE SCREENSHOT" << std::endl;
            exit(-1);
        }
        glReadBuffer(GL_FRONT);
        glReadPixels(0, 0, screenshot_width, screenshot_height, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);
        file.write((char*)TGA_header, 9*sizeof(short));
        file.write(pixel_data, 3*screenshot_width*screenshot_height);
        file.close();
        delete[] pixel_data;
        if(show_image) {
            std::cout << "Opening the screenshot" << std::endl;
            std::system(("open " + name + ".tga").c_str());
        }
    }
};


#endif /* camera_h */
