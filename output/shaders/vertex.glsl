#version 460 core

// Per-vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;  // not used but must stay

// Per-instance attributes
layout(location = 2) in vec3 instanceOffset;
layout(location = 3) in vec3 instanceColor;

out vec3 vColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Move cube according to instance offset
    vec3 worldPos = aPos + instanceOffset;

    gl_Position = projection * view * vec4(worldPos, 1.0);

    // Send per-instance color to fragment shader
    vColor = instanceColor;
}
