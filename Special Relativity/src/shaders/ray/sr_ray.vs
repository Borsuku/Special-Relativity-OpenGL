#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragmentPos;
out float velocity_sq;

uniform mat4 PV;

// x component - time at which the camera is observing (t_c), yzw components - position of the camera at this time (r_c) (IN S FRAME)
uniform vec4 camera;

uniform bool show_true_position; //if false - show apparent position, if true - show true position (with light propagation speed taken to be infinite)

uniform vec3 initial_pos; // position of the object (r_0) at time (t = 0) (IN S FRAME)
uniform vec3 velocity; // velocity of the object
uniform mat4 custom; // hold custom data to be use at "pos_local" function, it's use is specified in "scene.h"

uniform float speed_of_light;

const float PRECISION = 0;
const int ITERATION_MAX = 1000;
const float MAX_TIME_DISTANCE = 2000;

// rotate a vector by a specified angle around a specified axis
mat3 rotate(in vec3 axis, float angle) {
    mat3 rot;
    axis = normalize(axis);
    float c = cos(angle), s = sin(angle);
    rot[0] = vec3(c + axis.x*axis.x*(1-c), axis.x*axis.y*(1-c)-axis.z*s, axis.x*axis.z*(1-c)+axis.y*s);
    rot[1] = vec3(axis.y*axis.x*(1-c)+axis.z*s, c + axis.y*axis.y*(1-c), axis.y*axis.z*(1-c)-axis.x*s);
    rot[2] = vec3(axis.z*axis.x*(1-c)-axis.y*s, axis.z*axis.y*(1-c)+axis.x*s, c + axis.z*axis.z*(1-c));
    return rot;
}
// scale a vector by a specified size (around center)
mat3 scale(in vec3 size) {
    mat3 sc;
    sc[0][0] = size.x;
    sc[1][1] = size.y;
    sc[2][2] = size.z;
    return sc;
}

// gives a position of the vertex (IN S' FRAME)
vec3 pos_local(float t_local) {
    return aPos; //<->//
}
// gives 4-position of the vertex (IN S FRAME) at a given time (t')
vec4 lorentz_transform(float t_local) {
    vec4 transform;
    float v_dot_r_local = dot(pos_local(t_local), velocity);
    float c_2_inv = 1/(speed_of_light*speed_of_light);
    float gamma = 1/sqrt(1-velocity_sq*c_2_inv);

    float t_observer = gamma*(t_local+v_dot_r_local*c_2_inv);
    vec3 pos_observer = pos_local(t_local) + velocity*((gamma-1)*v_dot_r_local/velocity_sq+gamma*t_local) + initial_pos - camera.yzw;
    transform.x = t_observer;
    transform.yzw = pos_observer;
    return transform;
}
// sets the eqution to find the time (t_c') at which light was emitted to reach the camera at (t_c, r_c)
float equation(float t_local) {
    vec4 vec4_at_t_local = lorentz_transform(t_local);
    float LHS = speed_of_light*(camera.x - vec4_at_t_local.x);
    float RHS = length(vec4_at_t_local.yzw);
    return LHS - RHS;
}
// sets the eqution to find the time maximum possible time (t_c'(MAX)) at which the light could be emitted to reach the camera at (t_c, r_c)
float boudary_equation(float t_local) {
    float v_dot_r_local = dot(pos_local(t_local), velocity);
    float c_2_inv = 1/(speed_of_light*speed_of_light);
    float gamma = 1/sqrt(1-velocity_sq*c_2_inv);

    return gamma*(t_local+v_dot_r_local*c_2_inv) - camera.x;
}
// solve the equation to find (t_c'(MAX)) using regula-falsi method
float find_boundary() {
    float inf = MAX_TIME_DISTANCE;
    float start = -inf, end = inf;
    float c;
    float a_prev, b_prev;
    int counter = 0;
    do {
        a_prev = start;
        b_prev = end;
        float f_a = boudary_equation(start), f_b = boudary_equation(end);
        c = (start*f_b-end*f_a)/(f_b - f_a);
        float f_c = boudary_equation(c);
        if(f_c == 0) break;
        if(f_c*f_a > 0) start = c;
        else end = c;
        counter++;
    } while(counter < ITERATION_MAX && c-a_prev > PRECISION && b_prev-c > PRECISION);
    return c;
}
// solve the equation to find (t_c') using regula-falsi method
float solve(float start, float end) {
    float c;
    float a_prev, b_prev;
    int counter = 0;
    do {
        a_prev = start;
        b_prev = end;
        float f_a = equation(start), f_b = equation(end);
        c = (start*f_b-end*f_a)/(f_b - f_a);
        float f_c = equation(c);
        if(f_c == 0) break;
        if(f_c*f_a > 0) start = c;
        else end = c;
        counter++;
    } while(counter < ITERATION_MAX && c-a_prev > PRECISION && b_prev-c > PRECISION);
    return c;
}
// main program
void main() {
    TexCoords = aTexCoords;

    // calculate velocity_sq for the fragment shader
    velocity_sq = dot(velocity, velocity);
    // calculate (t_c'(MAX))
    float t_camera_local_max = find_boundary();
    // calculate the position (x(t)) of the vertex (IN S FRAME) and send it to the fragment shader
    if(!show_true_position)
        FragmentPos = lorentz_transform(solve(t_camera_local_max-MAX_TIME_DISTANCE, t_camera_local_max)).yzw;
    else FragmentPos = lorentz_transform(t_camera_local_max).yzw;
    gl_Position = PV * vec4(FragmentPos, 1.0);
}
