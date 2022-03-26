#include "scene_initial_prototype_hdr.h"

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

Scene_Initial_Prototype_HDR::Scene_Initial_Prototype_HDR() : angle(0.0f)
{
    parkLight = ObjMesh::load("media/park-light.obj", false, true);
    parkPlane = ObjMesh::load("media/park-plane.obj", false, true);
    parkBench = ObjMesh::load("media/park-bench.obj", false, true);
    parkTree = ObjMesh::load("media/park-tree.obj", false, true);
}

void Scene_Initial_Prototype_HDR::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0);

    angle = glm::radians(45.0f);

    setUpFBO();

    // array for full-screen quad
    GLfloat verts[] = {
         -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    prog.setUniform("Fog.MaxDist", 30.0f); // 30
    prog.setUniform("Fog.MinDist", 0.1f); // 1
    prog.setUniform("Fog.Colour", vec3(0.9)); // 0.5

    prog.setUniform("Spot.L", vec3(5.0f)); // 0.9
    prog.setUniform("Spot.La", vec3(0.1f)); // 0.5
    prog.setUniform("Spot.Exponent", 10.0f); // 200
    prog.setUniform("Spot.Cutoff", glm::radians(50.0f)); //10

    parkLightTex = Texture::loadTexture("media/Texture/park_light-texture.jpg");
    parkPlaneTex = Texture::loadTexture("media/Texture/park-texture.jpg");
    parkBenchTex = Texture::loadTexture("media/Texture/park_bench-texture.jpg");
    parkTreeTex = Texture::loadTexture("media/Texture/park_tree-texture.jpg");
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, parkLightTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, parkPlaneTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, parkBenchTex);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, parkTreeTex);
}

void Scene_Initial_Prototype_HDR::setMatrices()
{
    mat4 mv = view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void Scene_Initial_Prototype_HDR::compile()
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

void Scene_Initial_Prototype_HDR::update(float t)
{
    //update your angle here
}

void Scene_Initial_Prototype_HDR::setUpFBO()
{
    GLuint depthBuf;

    // Create and bind the FBO
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // The depth buffer
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // The HDR color buffer
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &hdrTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach the images to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        hdrTex, 0);

    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(2, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene_Initial_Prototype_HDR::pass1()
{
    prog.setUniform("Pass", 1);

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(70.0f), (float)width / height,
        0.3f, 100.0f);

    drawScene();
}

void Scene_Initial_Prototype_HDR::pass2()
{
    prog.setUniform("Pass", 2);

    // Revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    view = mat4(1.0);
    model = mat4(1.0);
    projection = mat4(1.0);

    setMatrices();

    // Render the quad
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Scene_Initial_Prototype_HDR::computateLogAveLuminance()
{
    int size = width * height;
    std::vector<GLfloat> texData(size * 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());

    float sum = 0.0f;

    for (int i = 0; i < size; i++) {
        float lum = glm::dot(vec3(texData[i * 3 + 0], texData[i * 3 + 1],
            texData[i * 3 + 2]),
            vec3(0.2126f, 0.7152f, 0.0722f));
        sum += logf(lum + 0.00001f);
    }

    prog.setUniform("AveLum", expf(sum / size));
}

void Scene_Initial_Prototype_HDR::drawScene()
{
    prog.setUniform("Spot.L", vec3(5.0f)); // 0.9
    prog.setUniform("Spot.La", vec3(0.2f)); // 0.5

    vec4 lightPos = vec4(4.0f, 6.0f, 2.0f, 1.0f);
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
    model = glm::translate(model, vec3(-0.7f, 0.7f, -1.0f));

    // BIND PARK LIGHT TEXTURE
    prog.setUniform("Tex1", 0);


    setMatrices();
    parkLight->render();

    // PARK PLANE

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.4f, 0.2f, 0.7f);
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(5.0f));
    model = glm::translate(model, vec3(-0.7f, -0.5f, -1.0f));

    // BIND PARK PLANE TEXTURE
    prog.setUniform("Tex1", 1);

    setMatrices();
    parkPlane->render();

    // PARK BENCH

    prog.setUniform("Material.Kd", 0.3f, 0.1f, 0.2f);
    prog.setUniform("Material.Ks", 0.3f, 0.1f, 0.3f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 10.0f);

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(2.0f, -0.5f, 0.0f));

    // BIND PARK BENCH TEXTURE
    prog.setUniform("Tex1", 2);


    setMatrices();
    parkBench->render();

    // PARK TREE 1

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(12.7f, 0.7f, 0.0f));

    // BIND PARK TREE TEXTURE



    setMatrices();
    parkTree->render();

    // PARK TREE 2

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(1.2f));
    model = glm::rotate(model, glm::radians(140.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(17.0f, 0.7f, -6.5f));

    // BIND PARK TREE TEXTURE

    setMatrices();
    parkTree->render();

    // PARK TREE 3

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(85.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(8.7f, 0.7f, -4.0f));

    // BIND PARK TREE TEXTURE

    setMatrices();
    parkTree->render();

    // PARK TREE 4

    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(2.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(-3.7f, 0.7f, 6.0f));

    // BIND PARK TREE TEXTURE

    setMatrices();
    parkTree->render();
}

void Scene_Initial_Prototype_HDR::render()
{
    pass1();
    computateLogAveLuminance();
    pass2();
}

void Scene_Initial_Prototype_HDR::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f); // 0.1 = minClipping
}