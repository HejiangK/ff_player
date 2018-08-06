#version 330
in vec2 textureOut;
out vec4 fragColor;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

void main(void)
{
    float r,g,b,y,u,v;

    y = texture(tex_y, textureOut).r;
    u = texture(tex_u, textureOut).r;
    v = texture(tex_v, textureOut).r;

    y = 1.164*(y - 0.0625);
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.596023559570*v;
    g = y - 0.3917694091796875*u - 0.8129730224609375*v;
    b = y + 2.017227172851563*u;

    fragColor = vec4(r,g,b, 1.0);
}

