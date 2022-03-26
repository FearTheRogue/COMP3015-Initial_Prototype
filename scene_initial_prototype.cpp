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
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

Scene_InitialPrototype::Scene_InitialPrototype() : angle(0.0f), plane(50.0f, 50.0f, 1, 1)
{
    parkLight = ObjMesh::load("media/park-light.obj", true);
    parkPlane = ObjMesh::load("media/park-plane.obj", true);
    parkBench = ObjMesh::load("media/park-bench.obj", true);
    parkTree = ObjMesh::load("media/park-tree.obj", true);
}

void Scene_InitialPrototype::initScene()
{
    compile();
    
    glEnable(GL_DEPTH_TEST);
    
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0);

    prog.setUniform("Fog.MaxDist", 30.0f); // 30
    prog.setUniform("Fog.MinDist", 0.01f); // 1
    prog.setUniform("Fog.Colour", vec3(0.4)); // 0.5

    prog.setUniform("Spot.L", vec3(1.0f)); // 0.9
    prog.setUniform("Spot.La", vec3(0.1f)); // 0.5
    prog.setUniform("Spot.Exponent", 15.0f); // 200
    prog.setUniform("Spot.Cutoff", glm::radians(35.0f)); //10

    parkLightTex = Texture::loadTexture("media/Texture/park_light-texture.jpg");
    parkPlaneTex = Texture::loadTexture("media/Texture/park-texture.jpg");
    parkBenchTex = Texture::loadTexture("media/Texture/park_bench-texture.jpg");
    parkTreeTex = Texture::loadTexture("media/Texture/park_tree-texture.jpg");
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

    // setting the spotlight position
    vec4 lightPos = vec4(4.0f, 7.0f, 2.0f, 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));

    // setting the spotlight direction
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    /// <summary>
    /// 
    /// PARK LIGHT MODEL
    /// 
    /// </summary>

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(-0.7f, 0.7f, -1.0f));

    // BIND PARK LIGHT TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkLightTex);

    setMatrices();
    parkLight->render();

    /// <summary>
    /// 
    /// PARK PLANE MODEL
    /// 
    /// </summary>//

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.4f, 0.2f, 0.7f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(5.0f));
    //model = glm::rotate(model, glm::radians(180.0f), vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, vec3(-0.7f, -0.5f, -1.0f));

    // BIND PARK PLANE TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkPlaneTex);

    setMatrices();
    parkPlane->render();

    /// <summary>
    /// 
    /// PARK BENCH MODEL
    /// 
    /// </summary>

    prog.setUniform("Material.Kd", 0.3f, 0.1f, 0.2f);
    prog.setUniform("Material.Ks", 0.3f, 0.1f, 0.3f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 10.0f);

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(2.0f, -0.5f, 0.0f));

    // BIND PARK BENCH TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkBenchTex);

    setMatrices();
    parkBench->render();

    /// <summary>
    /// 
    /// PARK TREE 1 MODEL
    /// 
    /// </summary>

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(12.7f, 0.7f, 0.0f));

    // BIND PARK TREE TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkTreeTex);

    setMatrices();
    parkTree->render();

    /// <summary>
    /// 
    /// PARK TREE 2 MODEL
    /// 
    /// </summary>

    model = mat4(1.0f);
    model = glm::scale(model, vec3(1.2f));
    model = glm::rotate(model, glm::radians(140.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(17.0f, 0.7f, -6.5f));

    setMatrices();
    parkTree->render();

    /// <summary>
    /// 
    /// PARK TREE 3 MODEL
    /// 
    /// </summary>

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(85.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(8.7f, 0.7f, -4.0f));

    setMatrices();
    parkTree->render();

    /// <summary>
    /// 
    /// PARK TREE 4 MODEL
    /// 
    /// </summary>

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(-3.7f, 0.7f, 6.0f));

    setMatrices();
    parkTree->render();
}

void Scene_InitialPrototype::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f); // 0.1 = minClipping
}