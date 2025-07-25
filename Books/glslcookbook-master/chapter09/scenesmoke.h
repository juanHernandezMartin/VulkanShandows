#ifndef SCENESMOKE_H
#define SCENESMOKE_H

#include "scene.h"
#include "glslprogram.h"
#include "cookbookogl.h"

#include <glm/glm.hpp>

class SceneSmoke : public Scene
{
private:
    GLSLProgram prog;

    GLuint posBuf[2], velBuf[2];
    GLuint particleArray[2];
    GLuint feedback[2], initVel, startTime[2];
    GLuint drawBuf;
    GLuint renderSub, updateSub;

    int nParticles;
    float angle;
    float time, deltaT;

    void setMatrices();
    void compileAndLinkShader();
    void initBuffers();
    float randFloat();

public:
    SceneSmoke();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENESMOKE_H
