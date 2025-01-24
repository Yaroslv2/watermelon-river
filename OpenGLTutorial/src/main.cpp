/*
    CODE
    ENGINE (Scene)
    OPENGL
    GPU
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

#include "graphics/memory/framememory.hpp"
#include "graphics/memory/uniformmemory.hpp"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"
#include "graphics/models/plane.hpp"
#include "graphics/models/map.hpp"
#include "graphics/models/tree.hpp"
#include "graphics/models/watermelon.hpp"

#include "graphics/objects/model.h"

#include "graphics/rendering/shader.h"
#include "graphics/rendering/texture.h"
#include "graphics/rendering/light.h"
#include "graphics/rendering/cubemap.h"
#include "graphics/rendering/text.h"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"

#include "algorithms/states.hpp"
#include "algorithms/ray.h"

#include "scene.h"

Scene scene;

void processInput(double dt);
void renderScene(Shader shader);

Camera cam;

//Joystick mainJ(0);

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

Watermelon watermelon(10);
Watermelon watermelon1(1);
glm::vec3 watermelonPos = glm::vec3(0.f, -1.5f, 3.f);
RigidBody* wmrb;
float wspeed = 2.f;
Lamp lamp1(4);
Lamp lamp2(4);
Map map(4);
Tree tree = Tree(4);
Tree trees[] = { Tree(4), Tree(4), Tree(4), Tree(4), Tree(4), Tree(4), Tree(4), Tree(4), Tree(4), Tree(4),
                 Tree(4), Tree(4) };

std::string Shader::defaultDirectory = "assets/shaders";

#include "physics/collisionmesh.h"

int main() {
    std::srand(std::time(NULL));
    // construct scene
    scene = Scene(3, 3, "watermelone-river", 1200, 720);
    // test if GLFW successfully started and created window
    if (!scene.init()) {
        std::cout << "Could not open window" << std::endl;
        scene.cleanup();
        return -1;
    }

    // set camera
    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;

    // SHADERS===============================
    Shader::loadIntoDefault("defaultHead.gh");

    Shader shader(true, "instanced/instanced.vs", "object.fs");
    Shader boxShader(false, "instanced/box.vs", "instanced/box.fs");
    
    Shader dirShadowShader(false, "shadows/dirSpotShadow.vs",
        "shadows/dirShadow.fs");
    Shader spotShadowShader(false, "shadows/dirSpotShadow.vs",
        "shadows/pointSpotShadow.fs");
    Shader pointShadowShader(false, "shadows/pointShadow.vs",
        "shadows/pointSpotShadow.fs",
        "shadows/pointShadow.gs");

    Shader::clearDefault();

    // FONTS===============================
    TextRenderer font(32);
    if (!scene.registerFont(&font, "comic", "assets/fonts/comic.ttf")) {
        std::cout << "Could not load font" << std::endl;
    }

    // MODELS==============================
    scene.registerModel(&lamp1);
    scene.registerModel(&lamp2);

    scene.registerModel(&map);

    scene.registerModel(&watermelon);
    scene.registerModel(&watermelon1);

    for (int i = 0; i < 12; i++)
        scene.registerModel(&trees[i]);

    //scene.registerModel(&cube);

    Box box;
    box.init();

    // load all model data
    scene.loadModels();

    // LIGHTS==============================

    // directional light
    DirLight dirLight(glm::vec3(0.f, 0.0f, 0.0f),
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
        glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
        BoundingRegion(glm::vec3(-20.0f, -20.0f, -0.5f), glm::vec3(20.0f, 20.0f, 50.0f)));
    scene.dirLight = &dirLight;

    // point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, 30.0f, 0.0f),
        glm::vec3(-30.0f, 50.0f, -30.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.0014f;
    float k2 = 0.000007f;

    PointLight pointLights[4];
    for (int i = 0; i < 1; i++) {
        pointLights[i] = PointLight(
            pointLightPositions[i],
            k0, k1, k2,
            ambient, diffuse, specular,
            0.5f, 50.0f
        );
        // create physical model for each lamp
        scene.generateInstance(lamp1.id, glm::vec3(0.0f, 0.0f, 0.0f), 0.25f, pointLightPositions[i], { -0.0f, 1.f, -0.0f });
        // add lamp to scene's light source
        scene.pointLights.push_back(&pointLights[i]);
        // activate lamp in scene
        States::activateIndex(&scene.activePointLights, i);
        //pointLights[1] = PointLight(
        //    pointLightPositions[1],
        //    k0, k1, k2,
        //    ambient, diffuse, specular,
        //    0.5f, 50.0f
        //);
        //// create physical model for each lamp
        //scene.generateInstance(lamp2.id, glm::vec3(10.0f, 0.25f, 10.0f), 0.25f, pointLightPositions[1], { -.5f, 0.f, 0.0f });
        //// add lamp to scene's light source
        //scene.pointLights.push_back(&pointLights[1]);
        //// activate lamp in scene
        //States::activateIndex(&scene.activePointLights, 1);
    }
    

    // spot light
    SpotLight spotLight(
        //glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        cam.cameraPos, cam.cameraFront, cam.cameraUp,
        glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
        1.0f, 0.0014f, 0.000007f,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f),
        0.1f, 100.0f
    );
    scene.spotLights.push_back(&spotLight);
    //scene.activeSpotLights = 1; // 0b00000001

    //scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));
    glm::vec3 cubePositions[] = {
        { 1.0f, 3.0f, -5.0f },
        { -7.25f, 2.1f, 1.5f },
        { -15.0f, 2.55f, 9.0f },
        { 4.0f, -3.5f, 5.0f },
        { 2.8f, 1.9f, -6.2f },
        { 3.5f, 6.3f, -1.0f },
        { -3.4f, 10.9f, -5.5f },
        { 0.0f, 11.0f, 0.0f },
        { 0.0f, 5.0f, 0.0f }
    };
    for (unsigned int i = 0; i < 9; i++) {
        //scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
    }

    scene.generateInstance(map.id, glm::vec3(2.0f), 1.0f, glm::vec3(0.f));

    wmrb = scene.generateInstance(watermelon1.id, glm::vec3(1.f), 1.f, watermelonPos);

    for (int i = 0; i < 12; i++)
    {
        glm::vec3 pos = { (std::rand() % 70) - 35.f, -.4f, (std::rand() % 70) - 35.f };
        glm::vec3 rot = { 1.5f, (std::rand() % 40), 0.f };
        scene.generateInstance(trees[i].id, glm::vec3(0.3f), 1.f, pos, rot);
    }

    // instantiate instances
    scene.initInstances();

    // finish preparations (octree, etc)
    scene.prepare(box, { shader });

    scene.variableLog["time"] = (double)0.0;

    scene.defaultFBO.bind(); // bind default framebuffer

    while (!scene.shouldClose()) {
        // calculate dt
        double currentTime = glfwGetTime();
        dt = currentTime - lastFrame;
        lastFrame = currentTime;

        scene.variableLog["time"] += dt;
        scene.variableLog["fps"] = 1 / dt;

        // update screen values
        scene.update();

        // process input
        processInput(dt);

        // activate the directional light's FBO

        // remove launch objects if too far
        for (int i = 0; i < watermelon.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - watermelon.instances[i]->pos) > 250.0f) {
                scene.markForDeletion(watermelon.instances[i]->instanceId);
            }
        }

        //// render scene to dirlight FBO
        /*dirLight.shadowFBO.activate();
        scene.renderDirLightShader(dirShadowShader);
        renderScene(dirShadowShader);*/

        //// render scene to point light FBOs
        for (unsigned int i = 0, len = scene.pointLights.size(); i < len; i++) {
            if (States::isIndexActive(&scene.activePointLights, i)) {
                scene.pointLights[i]->shadowFBO.activate();
                scene.renderPointLightShader(pointShadowShader, i);
                renderScene(pointShadowShader);
            }
        }

        // render scene to spot light FBOs
        for (unsigned int i = 0, len = scene.spotLights.size(); i < len; i++) {
            if (States::isIndexActive(&scene.activeSpotLights, i)) {
                scene.spotLights[i]->shadowFBO.activate();
                scene.renderSpotLightShader(spotShadowShader, i);
                renderScene(spotShadowShader);
            }
        }

        // render scene normally
        scene.defaultFBO.activate();
        scene.renderShader(shader);
        renderScene(shader);

        // render boxes
        scene.renderShader(boxShader, false);
        box.render(boxShader);

        // send new frame to window
        scene.newFrame(box);

        // clear instances that have been marked for deletion
        scene.clearDeadInstances();
    }

    // clean up objects
    scene.cleanup();
    return 0;
}

void renderScene(Shader shader) {
    if (watermelon.currentNoInstances > 0) {
        scene.renderInstances(watermelon.id, shader, dt);
    }

    //scene.renderInstances(cube.id, shader, dt);
    
    scene.renderInstances(lamp1.id, shader, dt);
    scene.renderInstances(lamp2.id, shader, dt);

    scene.renderInstances(map.id, shader, dt);

    scene.renderInstances(watermelon1.id, shader, dt);

    for (int i = 0; i < 12; i++)
    {
        scene.renderInstances(trees[i].id, shader, dt);
    }
}

void launchItem(float dt) {
    RigidBody* rb = scene.generateInstance(watermelon.id, glm::vec3(0.1f), 1.0f, cam.cameraPos);
    if (rb) {
        // instance generated successfully
        rb->transferEnergy(25.0f, cam.cameraFront);
        rb->applyAcceleration(Environment::gravitationalAcceleration);
    }
}

void processInput(double dt) {
    // process input with cameras
    scene.processInput(dt);

    // close window
    if (Keyboard::key(GLFW_KEY_ESCAPE)) {
        scene.setShouldClose(true);
    }

    // update flash light
    if (States::isIndexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
        scene.spotLights[0]->up = scene.getActiveCamera()->cameraUp;
        scene.spotLights[0]->updateMatrices();
    }

    if (Keyboard::key(GLFW_KEY_UP))
    {
        wmrb->pos += glm::vec3(wspeed * dt, 0.f, 0.f);
    }
    if (Keyboard::key(GLFW_KEY_DOWN))
    {
        wmrb->pos -= glm::vec3(wspeed * dt, 0.f, 0.f);
    }
    if (Keyboard::key(GLFW_KEY_LEFT))
    {
        wmrb->pos -= glm::vec3(0.f, 0.f, wspeed * dt);
    }
    if (Keyboard::key(GLFW_KEY_RIGHT))
    {
        wmrb->pos += glm::vec3(0.f, 0.f, wspeed * dt);
    }


    // launch sphere
    if (Keyboard::keyWentDown(GLFW_KEY_F)) {
        launchItem(dt);
    }
}