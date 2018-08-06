#version 330

layout (location = 0) in vec4 vertexIn;
layout (location = 1) in vec2 textureIn;
out vec2 textureOut;

void main(void)
{
    gl_Position = vertexIn;
    textureOut  = textureIn;
}
