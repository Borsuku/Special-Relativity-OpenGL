#version 410 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

in float velocity_sq;
in vec3 FragmentPos;

uniform sampler2D texture_diffuse1;

uniform vec4 camera;
uniform vec3 velocity;
uniform float speed_of_light;

uniform bool show_true_position; //if false - show apparent position and doppler effect, if true - show true position (with light propagation speed taken to be infinite) and turn off doppler
uniform bool turn_off_doppler; //if false - apply doppler effect, if true - turn off doppler effect

//wavelengths of light
const float R = 700.0f;
const float G = 523.0f;
const float B = 436.0f;

const vec3 black = vec3(0.0f, 0.0f, 0.0f); // > 780 or < 350
const vec3 red = vec3(0.757f, 0.0f, 0.004f); //700
const vec3 orange = vec3(0.98f, 0.447f, 0.059f); //616
const vec3 yellow = vec3(0.969f, 0.945f, 0.122f); //580
const vec3 green = vec3(0.255f, 0.988f, 0.043f); //523
const vec3 cyan = vec3(0.184f, 0.882f, 0.992f); //488
const vec3 blue = vec3(0.02f, 0.012f, 0.988f); //436
const vec3 violet = vec3(0.38f, 0.004f, 0.424f); //390

//const float wavelengths[10] = {350.0f, 400.0f, 450.0f, 500.0f, 550.0f, 600.0f, 650.0f, 700.0f, 800.0f}; array initialization doesnt work in GLSL!


float dopplerShift(float lambda) {
    float angle_times_speed = dot(velocity, normalize(FragmentPos));
    return lambda / sqrt(1.0f - velocity_sq/(speed_of_light * speed_of_light)) * (1.0f + angle_times_speed/speed_of_light);
}

vec3 interpolate(vec3 col1, vec3 col2, float low, float high, float val) {
    return mix(col1, col2, (val-low)/(high-low));
}

vec3 lambdaToRGB(float lambda) {
    vec3 color;
    if(lambda <= 350.0f) color = black;
    else if(lambda > 350.0f && lambda <= 390.0f) color = interpolate(black, violet, 350.0f, 390.0f, lambda);
    else if(lambda > 390.0f && lambda <= 436.0f) color = interpolate(violet, blue, 390.0f, 436.0f, lambda);
    else if(lambda > 436.0f && lambda <= 488.0f) color = interpolate(blue, cyan, 436.0f, 488.0f, lambda);
    else if(lambda > 488.0f && lambda <= 523.0f) color = interpolate(cyan, green, 488.0f, 523.0f, lambda);
    else if(lambda > 523.0f && lambda <= 580.0f) color = interpolate(green, yellow, 523.0f, 580.0f, lambda);
    else if(lambda > 580.0f && lambda <= 616.0f) color = interpolate(yellow, orange, 580.0f, 616.0f, lambda);
    else if(lambda > 616.0f && lambda <= 700.0f) color = interpolate(orange, red, 616.0f, 700.0f, lambda);
    else if(lambda > 700.0f && lambda <= 780.0f) color = interpolate(red, black, 700.0f, 780.0f, lambda);
    else color = black;
    return color;
}
                               
vec3 transformColor(vec3 color) {
    vec3 newRed = lambdaToRGB(dopplerShift(R));
    vec3 newGreen = lambdaToRGB(dopplerShift(G));
    vec3 newBlue = lambdaToRGB(dopplerShift(B));
    
    return color.x * newRed + color.y * newGreen + color.z * newBlue;
}

void main() {
    if(!show_true_position && !turn_off_doppler) FragColor = vec4(transformColor(texture(texture_diffuse1, TexCoords).xyz), 1.0f);
    else FragColor = texture(texture_diffuse1, TexCoords);
}
