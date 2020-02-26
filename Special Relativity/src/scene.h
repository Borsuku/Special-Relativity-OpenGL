//
//  scene.h
//  Special Relativity
//
//  Created by Antoni Wójcik on 05/04/2019.
//
//  To change the scenario of the scene, modify the code in "setUpScene" function. The functions have to be used in a sequence. The added object uses the last added model and last added shader (there has to be at least one loaded shader and at least one model loaded). Useful functions:
//
//  *** "addRelativisticShader(const char* custom_vertex_fragment = nullptr)":
//  - used to add a relativistic shader to the object. If no input in the argument - the object will perform no transformations in it's own frame. Optional argument - a GLSL code which changes the transformation in the local frame of the object. Use mat4 "custom" to transfer additional data to the function. To make the code shorter use two pre-made functions:
//  * "mat3 rotate(in vec3 axis, float angle)":
//  - rotate a vector by a specified angle around a specified axis
//  * "mat3 scale(in vec3 size)":
//  - scale a vector by a specified size (around center)
//
//  *** "addModel(const std::string &path)":
//  - used to add a model of an object at a given path.
//
//  *** "addObject(float pos_x, float pos_y, float pos_z, float v_x, float v_y, float v_z, const glm::mat4& custom)" OR "addObject(float pos_x, float pos_y, float pos_z, float v_x, float v_y, float v_z, float c_x = 0, float c_y = 0, float c_z = 0, float c_w = 0)":
//  - used to add an object to the scene. The arguments are self explanatory.
//
//
// EXAMPLE SCENARIO 1 - adds 201 boxes next to each other which perform sinusoidal synchronized oscillations in their own frame. The frame moves at 90% of speed of light in x-direction, relative to the camera.
//
// addRelativisticShader("return aPos+custom[0].xyz*sin(t_local*custom[0].w)+custom[1].xyz;");
// addModel("assets/objects/cube_textured_complex/cube.obj");
// for(int i = -100; i < 100; i++) {
//    glm::mat4 custom(glm::vec4(0, 1.0f, 0, 0.5f), glm::vec4(i*3.0f, 0, 0, 0), glm::vec4(0), glm::vec4(0));
//    addObject(0.0f, 10, 0, 0.9f, 0.0f, 0.0f, custom);
// }
//
// EXAMPLE SCENARIO 2 - adds a rotating sphere moving in z direction with 75% of speed of light
//
// addRelativisticShader("return rotate(custom[0].xyz, custom[0].w*t_local)*aPos+custom[1].xyz;");
// addModel("assets/objects/wheel/wheel.obj");
// glm::mat4 custom(glm::vec4(0, 0, 1, 0.8f), glm::vec4(0), glm::vec4(0), glm::vec4(0));
// addObject(0.0f, 0.01f, 2.0f, 0.7f, 0.0f, 0.0f, custom);
//

#ifndef scene_h
#define scene_h

#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "plane.h"

#include <vector>

class Scene {
private:
    struct Object {
        glm::vec3 position; // initial position (at t_c = 0) (IN S FRAME)
        unsigned int model_id;
        unsigned int shader_id;
        
        glm::vec3 velocity;
        glm::mat4 custom_data;
    };

    std::vector<Object> objects;
    std::vector<Model> models;
    std::vector<Shader> shaders;
    
    Plane plane;
    
    float speed_of_light = 1.0f;
    
    void setUpScene() {
        glm::mat4 custom;
        
        
        //SCENARIO 1 - Terell rotation, Lorentz contraction - BOX
        addRelativisticShader("return aPos+custom[0].xyz;");
        addModel("assets/objects/die/die.obj");
        for(int i = -5; i <= 5; i++) {
            custom = glm::mat4(glm::vec4(i*3.0f, 0, 0, 0), glm::vec4(0), glm::vec4(0), glm::vec4(0));
            addObject(0, 0, 0, 0.99f, 0.0f, 0.0f, custom);
            addObject(0, 3, 0, 0.0f, 0.0f, 0.0f, custom);
        }
        
        
        //SCENARIO 2 - Time dilation - CLOCKS
        /*addModel("assets/objects/clock/clock.obj");
        addRelativisticShader("return rotate(custom[0].xyz, t_local*custom[0].w)*custom[1].x*aPos;");
        custom = glm::mat4(glm::vec4(0), glm::vec4(0.575, 0, 0, 0), glm::vec4(0), glm::vec4(0));
        for(int i = 0; i < 3; i++) {
            addObject(0.0f, i*2.0f, -20, 0.33f*i, 0.0f, 0.0f, custom);
        }
        addObject(0.0f, 6.0f, -20, 0.9, 0.0f, 0.0f, custom);
        addModel("assets/objects/clock/clock_tick.obj");
        custom = glm::mat4(glm::vec4(0, 0, 1.0f, 0.5257f), glm::vec4(0.575, 0, 0, 0), glm::vec4(0), glm::vec4(0));
        for(int i = 0; i < 3; i++) {
            addObject(0.0f, i*2.0f, -20, 0.33f*i, 0.0f, 0.0f, custom);
        }
        addObject(0.0f, 6.0f, -20, 0.9, 0.0f, 0.0f, custom);*/
        
        
        //SCENARIO 3 - BOX SEQUENCE
        /*addRelativisticShader("return aPos+custom[0].xyz*sin(t_local*custom[0].w)+custom[1].xyz;");
        addModel("assets/objects/die/die.obj");
        for(int i = -50; i < 50; i++) {
            custom = glm::mat4(glm::vec4(0, 1.0f, 0, 0.5f), glm::vec4(i*3.0f, 0, 0, 0), glm::vec4(0), glm::vec4(0));
            addObject(0.0f, 5, 0, 0.0f, 0.0f, 0.0f, custom);
        }
        for(int i = -50; i < 50; i++) {
            custom = glm::mat4(glm::vec4(0, 1.0f, 0, 0.5f), glm::vec4(i*3.0f, 0, 0, 0), glm::vec4(0), glm::vec4(0));
            addObject(0.0f, 0, 0, 0.9f, 0.0f, 0.0f, custom);
        }*/
        
        
        //SCENARIO 4 - Bike - WHEELS
        /*addRelativisticShader("return rotate(custom[0].xyz, custom[0].w*t_local)*aPos+custom[1].xyz;");
        addModel("assets/objects/wheel/wheel.obj");
        addObject(-3.0f, 1, -2.0f, 0.0f, 0.0f, 0.0f, custom);
        custom = glm::mat4(glm::vec4(0, 0, 1, 0.9f), glm::vec4(0), glm::vec4(0), glm::vec4(0));
        addObject(3.0f, 1, 0.0f, 0.9f, 0.0f, 0.0f, custom);
        addObject(3.0f, 1, -2.0f, 0.0f, 0.0f, 0.0f, custom);*/
        
        
        //SCENARIO 5 - SPHERES
        /*addRelativisticShader();
        addModel("assets/objects/sphere/sphere.obj");
        addObject(0.0f, 1, 4, 0.999, 0.0f, 0.0f);
        addObject(0.0f, 1, 0, 0.9, 0.0f, 0.0f);
        addObject(0.0f, 1, -4, 0, 0.0f, 0.0f);*/
        
        
        //SCENARIO 6 - RELATIVISTIC ABERRATION
        /*addRelativisticShader("return aPos*100.0f;");
        addModel("assets/objects/universe/universe.obj");
        addObject(0.0f, 1, 0, 0, 0, 0.99f);*/
        
    }
public:
    
    float time = 0;
    
    Scene() {
        //load coords shader first
        addShader("src/shaders/default/default.vs", "src/shaders/default/default.fs");
        addModel("assets/objects/coords/coords2.obj");
        addModel("assets/objects/coords/arrow.obj");
        
        setUpScene();
    }
    
   void draw(Camera* camera, float ratio, float delta_time, bool show_true_position, bool turn_off_doppler){
       unsigned int current_object = 0;
       time += delta_time;
       
       for(unsigned int i = 1; i < shaders.size(); i++) {
           shaders[i].use();
           
           shaders[i].setBool("show_true_position", show_true_position);
           shaders[i].setBool("turn_off_doppler", turn_off_doppler);
           
           camera->transferData(shaders[i]);
           shaders[i].setVec4("camera", glm::vec4(time, camera->position));
           shaders[i].setFloat("speed_of_light", speed_of_light);
           
           for(unsigned int j = current_object; j < objects.size(); j++) {
               if(i == objects[j].shader_id) {
                   setRelativisticParameters(camera, &objects[j], &shaders[i]);
                   
                   models[objects[j].model_id].draw(shaders[i]);
                   
                   current_object++;
               } else break;
           }
       }
    }
    
    void drawPos(Camera* camera, float ratio, bool show_true_position){
        plane.draw(camera);
        
        glClear(GL_DEPTH_BUFFER_BIT); //make the coords always on top
        
        shaders[0].use();
        
        camera->transferData(shaders[0]);
        
        for(unsigned int j = 0; j < objects.size(); j++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), objects[j].position+objects[j].velocity*time-camera->position);
            shaders[0].setMat4("model", model);
            models[0].draw(shaders[0]); //draw coordinates
            
            if(!show_true_position){
                model = glm::mat4(1.0f);
                model = glm::translate(model, objects[j].position+objects[j].velocity*(findTime(camera, objects[j]))-camera->position);
                model = model * rotateVelocityArrow(&objects[j]); //rotate the arrow in the direction of motion
                shaders[0].setMat4("model", model);
                models[1].draw(shaders[0]); //draw velocity vector arrow
            }
        }
    }
    
    
    
private:
    float findTime(const Camera* camera, const Object& object) {
        glm::vec3 beta = object.velocity/speed_of_light;
        glm::vec3 alpha = (camera->position - object.position)/speed_of_light;
        float beta_2 = 1-glm::dot(beta, beta);
        float alpha_2 = glm::dot(alpha, alpha);
        float adb = time-glm::dot(alpha, beta); // adb - dot product of alpha and beta
        return (adb - glm::sqrt(adb*adb+beta_2*(alpha_2-time*time)))/beta_2;
    }
    
    glm::mat4 rotateVelocityArrow(Object* object) {
        if(glm::dot(object->velocity, object->velocity) == 0.0f) return glm::mat4(1.0f);
        float angle = glm::acos(glm::normalize(object->velocity).x);
        glm::vec3 rot_axis(0, -object->velocity.z, object->velocity.y);
        if(rot_axis.z == 0.0f) rot_axis.z = 1.0f;
        return glm::rotate(glm::mat4(1.0f), angle, rot_axis);
    }
    
    inline void setRelativisticParameters(Camera* camera, Object* object, Shader* shader) {
        shader->setVec3("initial_pos", object->position);
        shader->setVec3("velocity", object->velocity);
        shader->setMat4("custom", object->custom_data);
    }
    
    void addObject(float pos_x, float pos_y, float pos_z, float v_x, float v_y, float v_z, float c_x = 0, float c_y = 0, float c_z = 0, float c_w = 0) {
        Object object;
        object.position = glm::vec3(pos_x, pos_y, pos_z);
        object.velocity = glm::vec3(v_x, v_y, v_z);
        object.custom_data = glm::mat4(glm::vec4(c_x, c_y, c_z, c_w), glm::vec4(0), glm::vec4(0), glm::vec4(0));
        
        object.model_id = (unsigned int)(models.size() - 1);
        object.shader_id = (unsigned int)(shaders.size() - 1);
        
        objects.push_back(object);
    }
    
    void addObject(float pos_x, float pos_y, float pos_z, float v_x, float v_y, float v_z, const glm::mat4& custom) {
        Object object;
        object.position = glm::vec3(pos_x, pos_y, pos_z);
        object.velocity = glm::vec3(v_x, v_y, v_z);
        object.custom_data = custom;
        
        object.model_id = (unsigned int)(models.size() - 1);
        object.shader_id = (unsigned int)(shaders.size() - 1);
        
        objects.push_back(object);
    }
    
    void addModel(const std::string &path) {
        Model model = Model(path);
        models.push_back(model);
    }
    
    void addShader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr) {
        Shader shader = Shader(vertex_path, fragment_path, geometry_path);
        shaders.push_back(shader);
    }
    
    void addRelativisticShader(const char* custom_vertex_fragment = nullptr) {
        Shader shader = Shader("src/shaders/ray/sr_ray.vs", "src/shaders/ray/sr_ray.fs", custom_vertex_fragment);
        shaders.push_back(shader);
    }
};

#endif /* scene_h */
