precision highp float;
varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

void main(void)
{
    vec3 yuv;
    vec3 rgb;

    yuv.x = texture2D(tex_y, textureOut).r;
    yuv.y = texture2D(tex_u, textureOut).r;
    yuv.z = texture2D(tex_v, textureOut).r;

    yuv.x = 1.164*(yuv.x - 0.0625);
    yuv.y = yuv.y - 0.5;
    yuv.z = yuv.z - 0.5;

    rgb.x = yuv.x + 1.596023559570*yuv.z;
    rgb.y = yuv.x - 0.3917694091796875*yuv.y - 0.8129730224609375*yuv.z;
    rgb.z = yuv.x + 2.017227172851563*yuv.y;

    gl_FragColor = vec4(rgb, 1);

}
