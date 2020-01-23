#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 FragPosition;

uniform mat4 PV;

uniform vec3 camera_position;

const float scale = 100.0f;

void main() {
    FragPosition = aPos * scale - vec3(0.0f, camera_position.y, 0.0f);
    gl_Position = PV * vec4(FragPosition, 1.0f);
}
