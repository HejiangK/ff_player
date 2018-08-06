//
// Created by Ken on 2018/5/22.
//

#include "opengl_video_render.h"


OpenGLVideoRender::OpenGLVideoRender(QWidget *parent)
    : QOpenGLWidget(parent, 0)
{
    setAutoFillBackground(false);
    vao = GL_INVALID_INDEX;
}

OpenGLVideoRender::~OpenGLVideoRender()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();
}

void OpenGLVideoRender::onRenderFrame(std::shared_ptr<AVFrame> frame)
{
    curFrame = frame;
    update();

}

void OpenGLVideoRender::setScale(bool scale)
{
    OpenGLVideoRender::scale = scale;
}

void OpenGLVideoRender::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram(this);

#ifdef Q_OS_MAC

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/mac_vertex_shader.vsh");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/mac_fragment_shader.frag");

#else

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/win_vertex_shader.vsh");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/win_fragment_shader.fsh");

#endif

    m_program->bindAttributeLocation("vertexIn", 0);
    m_program->bindAttributeLocation("textureIn", 1);

    m_program->link();
    m_program->bind();

    GLfloat vertexPoints[] = {

            -1.0f, -1.0f,0.0f,1.0f,       0.0f, 1.0f,
            1.0f, -1.0f,0.0f,1.0f,        1.0f, 1.0f,
            -1.0f, 1.0f,0.0f,1.0f,        0.0f, 0.0f,
            1.0f, 1.0f,0.0f,1.0f,         1.0f, 0.0f

    };

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPoints),vertexPoints,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);

    textureUniformY = m_program->uniformLocation("tex_y");
    textureUniformU = m_program->uniformLocation("tex_u");
    textureUniformV = m_program->uniformLocation("tex_v");

    glGenTextures(1, &texture_y);
    glBindTexture(GL_TEXTURE_2D, texture_y);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &texture_u);
    glBindTexture(GL_TEXTURE_2D, texture_u);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &texture_v);
    glBindTexture(GL_TEXTURE_2D, texture_v);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glClearColor(0.2,0.2, 0.2, 0.0);

}

void OpenGLVideoRender::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void OpenGLVideoRender::paintGL()
{
    if(curFrame != nullptr)
    {
        glClear( GL_COLOR_BUFFER_BIT );

        float wRatio = (float)this->width() / curFrame->width;
        float hRatio = (float)this->height() / curFrame->height;
        float value  = scale ? qMax(wRatio, hRatio) :qMin(wRatio, hRatio);
        float y;
        float x;

        y = (float)curFrame->height * value / this->height();
        x = (float)curFrame->width * value / this->width();

        GLfloat vertexPoints[] = {

                -x, -y,0.0f,1.0f,       0.0f, 1.0f,
                x, -y,0.0f,1.0f,        1.0f, 1.0f,
                -x, y,0.0f,1.0f,        0.0f, 0.0f,
                x, y,0.0f,1.0f,         1.0f, 0.0f

        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPoints),vertexPoints,GL_STATIC_DRAW);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_y);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, curFrame->linesize[0], curFrame->height, 0, GL_RED, GL_UNSIGNED_BYTE, curFrame->data[0]);
        glUniform1i(textureUniformY, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_u);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, curFrame->linesize[1], curFrame->height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, curFrame->data[1]);
        glUniform1i(textureUniformU, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_v);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, curFrame->linesize[2], curFrame->height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, curFrame->data[2]);
        glUniform1i(textureUniformV, 2);
        // Draw stuff
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        glFlush();

    }
}

