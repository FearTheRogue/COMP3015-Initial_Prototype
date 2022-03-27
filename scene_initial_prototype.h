#pragma once
#ifndef SCENE_INITIALPROTOTYPE_H
#define SCENE_INITIALPROTOTYPE_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/objmesh.h"

class Scene_InitialPrototype : public Scene
{
private:
    GLSLProgram prog;

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

public:
    Scene_InitialPrototype();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
