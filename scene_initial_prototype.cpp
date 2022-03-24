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
}

void Scene_InitialPrototype::initScene()
{
    compile();
    
    glEnable(GL_DEPTH_TEST);
    
    isFoggy = true;

    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0);

    angle = glm::radians(45.0f);

    prog.setUniform("Fog.MaxDist", 30.0f); // 30
    prog.setUniform("Fog.MinDist", 0.1f); // 1
    prog.setUniform("Fog.Colour", vec3(0.55)); // 0.5

    prog.setUniform("Spot.L", vec3(0.7f)); // 0.9
    prog.setUniform("Spot.La", vec3(0.35f)); // 0.5
    prog.setUniform("Spot.Exponent", 7.0f); // 200
    prog.setUniform("Spot.Cutoff", glm::radians(30.0f)); //10

    parkLightTex = Texture::loadTexture("media/Texture/park_light-texture.jpg");
    parkPlaneTex = Texture::loadTexture("media/Texture/park-texture.jpg");
    parkBenchTex = Texture::loadTexture("media/Texture/park_bench-texture.jpg");

    // ORIGINAL SETTINGS
    /*
    view = glm::lookAt(vec3(0.0f, 0.5f, 1.25f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.5f, 0.0f)); // right, up, direction
    projection = mat4(1.0);

    prog.setUniform("Fog.MaxDist", 30.0f); // 30
    prog.setUniform("Fog.MinDist", 1.0f); // 1
    prog.setUniform("Fog.Colour", vec3(0.1f, 0.1f, 0.1f)); // 0.5

    prog.setUniform("Light.Position", view * glm::vec4(1.0f, 1.2f, 1.0f, 1.0f));
    prog.setUniform("Light.L", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Light.La", vec3(0.5f, 0.5f, 0.8f));

    prog.setUniform("Spot.L", vec3(0.7f)); // 0.9
    prog.setUniform("Spot.La", vec3(0.35f)); // 0.5
    prog.setUniform("Spot.Exponent", 600.0f); // 200
    prog.setUniform("Spot.Cutoff", glm::radians(5.0f)); //10

    parkLightTex = Texture::loadTexture("media/Texture/park_light-texture.jpg");
    parkPlaneTex = Texture::loadTexture("media/Texture/park-texture.jpg");
    parkBenchTex = Texture::loadTexture("media/Texture/park_bench-texture.jpg");
    */
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

    float deltaT = t - tPrev;

    if (tPrev == 0.0f)
        deltaT = 0.0f;

    tPrev = t;

    angle += 0.25f * deltaT;

    if (angle > glm::two_pi<float>())
        angle -= glm::two_pi<float>();
}

void Scene_InitialPrototype::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //create the rotation matrix here and update the uniform in the shader 

    /*
    vec4 lightPos = vec4(4.0f, 10.0f, 2.0f, 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    //vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
    //prog.setUniform("Light.Position", view * lightPos);

    // PARK LIGHT

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.3f, 0.3f, 0.3f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(-1.0f, 0.7f, 0.1f));
    
    // BIND PARK LIGHT TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkLightTex);

    setMatrices();
    parkLight->render();

    // PARK PLANE

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.5, 0.5, 0.5));
    //model = glm::rotate(model, glm::radians(180.0f), vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, vec3(0.0f, -0.5f, -1.0f));

    // BIND PARK PLANE TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkPlaneTex);

    setMatrices();
    parkPlane->render();

    // PARK BENCH

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.1, 0.1, 0.1));
    model = glm::rotate(model, glm::radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(2.0f, -0.5f, 0.0f));

    // BIND PARK BENCH TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkBenchTex);

    setMatrices();
    parkBench->render();

    //prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    //prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    //prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    //prog.setUniform("Material.Shininess", 180.0f);

    //setMatrices();
    //plane.render();
    */

    // ORIGINAL SETTINGS

    //vec3 cameraPos = vec3(5.0f * cos(angle), 5.0f, 7.5f * sin(angle));
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    vec4 lightPos = vec4(4.0f, 10.0f, 2.0f, 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    // PARK LIGHT

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(-1.0f, 0.7f, 0.1f));

    // BIND PARK LIGHT TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkLightTex);

    setMatrices();
    parkLight->render();

    // PARK PLANE

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.4f, 0.2f, 0.7f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(5.0f));
    //model = glm::rotate(model, glm::radians(180.0f), vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, vec3(-1.0f, -0.5f, -1.0f));

    // BIND PARK PLANE TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkPlaneTex);

    setMatrices();
    parkPlane->render();

    // PARK BENCH

    prog.setUniform("Material.Kd", 0.3f, 0.1f, 0.6f);
    prog.setUniform("Material.Ks", 0.3f, 0.6f, 0.3f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.7f));
    model = glm::rotate(model, glm::radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(2.0f, -0.5f, 0.0f));

    // BIND PARK BENCH TEXTURE

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkBenchTex);

    setMatrices();
    parkBench->render();
}

void Scene_InitialPrototype::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f); // 0.1 = minClipping
}