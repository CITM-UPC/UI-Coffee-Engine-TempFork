#[vertex]

#version 450 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}

#[fragment]

#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
    float alpha = texture(text, TexCoords).r;
    FragColor = vec4(textColor, alpha);
}
