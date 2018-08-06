//
// Created by Ken on 2018/5/22.
//

#ifndef ATHENAAPP_ES_VIDEO_RENDER2_H
#define ATHENAAPP_ES_VIDEO_RENDER2_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QGLContext>
#include <QGLWidget>
#include <QOpenGLTexture>
#include <QFile>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QMutex>
#include <QMutexLocker>
#include <memory>
#include <QMessageBox>
#include <QOpenGLShaderProgram>

extern "C"
{
#include <libavformat/avformat.h>
};


class OpenGLVideoRender : public QOpenGLWidget,protected QOpenGLExtraFunctions
{
public:
    OpenGLVideoRender(QWidget *parent);

    virtual ~OpenGLVideoRender();

    void onRenderFrame(std::shared_ptr<AVFrame> frame);

    void setScale(bool scale);

protected:

    virtual void initializeGL() override ;

    virtual void resizeGL( int w, int h ) override;

    virtual void paintGL() override;

private:

    QOpenGLShaderProgram        *m_program;
    std::shared_ptr<AVFrame>    curFrame;

    GLuint          vbo;
    GLuint          vao;

    GLuint          texture_y;
    GLuint          texture_u;
    GLuint          texture_v;

    GLint           textureUniformY;
    GLint           textureUniformU;
    GLint           textureUniformV;

    bool            scale;

};


#endif //ATHENAAPP_ES_VIDEO_RENDER2_H
