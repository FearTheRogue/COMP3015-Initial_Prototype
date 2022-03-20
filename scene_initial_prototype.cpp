#include "scene_initial_prototype.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat4;

Scene_InitialPrototype::Scene_InitialPrototype() : angle(0.0f), plane(10.0f, 10.0f, 100, 100)
{
    streetlight = ObjMesh::load("../COMP3015-Initial_Prototype/media/streetlight.obj", true);
}

void Scene_InitialPrototype::initScene()
{
    compile();
    
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.5f, 0.25f, 1.0f), vec3(0.0f, 0.00f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); // right, up, direction
    projection = mat4(1.0);

    prog.setUniform("Light.Position", view * glm::vec4(1.0f, 1.2f, 1.0f, 1.0f));
    prog.setUniform("Light.L", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Light.La", vec3(0.5f, 0.5f, 0.8f));
}

void Scene_InitialPrototype::setMatrices()
{
    mat4 mv = view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void Scene_InitialPrototype::compile()
{
    try {
        prog.compileShader("shader/sceneShader.vert");
        prog.compileShader("shader/sceneShader.frag");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void Scene_InitialPrototype::update(float t)
{
    //update your angle here
}

void Scene_InitialPrototype::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //create the rotation matrix here and update the uniform in the shader 

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);

    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f, 0.05f, 0.05f));

    setMatrices();
    streetlight->render();

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));

    setMatrices();
    plane.render();
}

void Scene_InitialPrototype::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}