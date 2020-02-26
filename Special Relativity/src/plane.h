//
//  plane.h
//  Special Relativity
//
//  Created by Antoni Wójcik on 12/01/2020.
//
//  Creates a plane of xz coordinate system, which floats under the camera.
//

#ifndef plane_h
#define plane_h

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "gtx/string_cast.hpp"

#include "shader.h"
#include "camera.h"

class Plane {
private:
    float vertices[12];
    unsigned int indices[6];
    unsigned int VAO;
    Shader plane_shader;
public:
    Plane() : vertices {
         1.0f,  0.0f,  1.0f,  // top right
         1.0f,  0.0f, -1.0f,  // bottom right
        -1.0f,  0.0,  -1.0f,  // bottom left
        -1.0f,  0.0f,  1.0f   // top left
    }, indices {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    }, plane_shader("src/shaders/plane/plane.vs", "src/shaders/plane/plane.fs") {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
    }
    ~Plane() {
        glDeleteVertexArrays(1, &VAO);
    }
    void draw(Camera* camera) {
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        plane_shader.use();
        
        camera->transferData(plane_shader);
        plane_shader.setVec3("camera_position", camera->position);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glDisable(GL_BLEND);
    }
};

#endif /* plane_h */
