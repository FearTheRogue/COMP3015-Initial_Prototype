#pragma once
#ifndef SCENE_INITIALPROTOTYPEHDR_H
#define SCENE_INITIALPROTOTYPEHDR_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/plane.h"
#include "helper/objmesh.h"

class Scene_Initial_Prototype_HDR : public Scene
{
private:
    GLSLProgram prog;
    float angle;
    float tPrev;

    GLuint quad, hdrTex;
    GLuint hdrFBO, aveTex;

    std::unique_ptr<ObjMesh> parkLight;
    std::unique_ptr<ObjMesh> parkPlane;
    std::unique_ptr<ObjMesh> parkBench;
    std::unique_ptr<ObjMesh> parkTree;

    GLuint parkLightTex;
    GLuint parkPlaneTex;
    GLuint parkBenchTex;
    GLuint parkTreeTex;

    void setMatrices();
    void compile();

    void pass1();
    void pass2();
    void setUpFBO();
    void computateLogAveLuminance();
    void drawScene();

public:
    Scene_Initial_Prototype_HDR();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
