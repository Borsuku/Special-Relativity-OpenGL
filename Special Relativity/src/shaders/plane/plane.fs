#version 410 core
out vec4 FragColor;

in vec3 FragPosition;

uniform vec3 camera_position;

const float spacing = 0.5f;
const float width = 0.01f;
const float width_thick = 0.03f;
const float fade = 0.0005f;

void main() {
    float x = (FragPosition+camera_position).x;
    float z = (FragPosition+camera_position).z;
    float radius_2 = FragPosition.x*FragPosition.x+FragPosition.z*FragPosition.z;
    float m_x = mod(x+spacing, spacing*2);
    float m_z = mod(z+spacing, spacing*2);
    float grade = (m_x > spacing - width && m_x < spacing + width || m_z > spacing - width && m_z < spacing + width) ? 1.0f : 0.0f;
    float spacing_thick = spacing*10;
    m_x = mod(x+spacing_thick, spacing_thick*2);
    m_z = mod(z+spacing_thick, spacing_thick*2);
    if(m_x > spacing_thick - width_thick && m_x < spacing_thick + width_thick || m_z > spacing_thick - width_thick && m_z < spacing_thick + width_thick) {
        grade = 1.0f;
        if(abs(x) < width_thick) {
            if(abs(z) < width_thick) FragColor = vec4(1.0f, 1.0f, 0.0f, 0.75f*grade*exp(-radius_2*fade));
            else FragColor = vec4(0.0f, 0.0f, 1.0f, 0.75f*grade*exp(-radius_2*fade));
        }
        else if(abs(z) < width_thick) FragColor = vec4(1.0f, 0.0f, 0.0f, 0.75f*grade*exp(-radius_2*fade));
        else FragColor = vec4(1.0f, 1.0f, 1.0f, 0.75f*grade*exp(-radius_2*fade));
    } else FragColor = vec4(1.0f, 1.0f, 1.0f, 0.75f*grade*exp(-radius_2*fade));
}
