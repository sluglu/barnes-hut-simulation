#include <math.h>
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <tuple>

//#include "GLContext.h"
#include "simulation.h"

simulation simu;

bool pause = false;
bool saveToImg = false;
bool showQuad = false;
bool showParticle = true;

string message = " ";

using namespace GLContext;



std::tuple<int, int, int> valueToRGB(double value) {
    double r, g, b;
    if (value < 0.5) {
        r = 1 - 2 * value + 2 * pow(value, 2);
        g = 2 * value - 2 * pow(value, 2);
        b = 0;
    }
    else {
        r = 0;
        g = 2 - 2 * value + 2 * pow(value, 2);
        b = 2 * (value - 0.5) - 2 * pow(value - 0.5, 2);
    }
    return { static_cast<int>(round(r * 255)), static_cast<int>(round(g * 255)), static_cast<int>(round(b * 255)) };
}

void applyColor(particle& p) {
    tuple<int, int, int> color = valueToRGB(p.mass / 1000.0f);
    p.color = vec4(get<0>(color), get<1>(color), get<2>(color), 1);
}

bool randomMass = true;
float mass = 1.0f;
bool sizeEqualToMass = true;
float pSize = 1.0f;
float radialInitialVelocity = 1.0f;
vec4 pColor = vec4(255, 255, 255, 255);

void partParameters(particle& p) {
    if (randomMass) {
        p.mass = float(rand() % 1001);
    }
    else {
        p.mass = mass;
    }
    if (sizeEqualToMass) {
        p.size = abs(p.mass * 0.004f);
    }
    else {
        p.size = pSize;
    }
    p.color = pColor;
    p.position = vec2(cos(rand()) * 0.9f, sin(rand()) * 0.9f);
    p.oldPosi = p.position;
    p.speed = radialInitialVelocity*vec2(p.position.y * 0.005f, -p.position.x * 0.005f);
    p.acceleration = vec2();
    //applyColor(p);
}

void reset() {
    simu.part.clear();
}

void drawQuad(const Quad& q) {
    vec2 topRight = vec2(q.botRight.x, q.topLeft.y);
    vec2 botLeft = vec2(q.topLeft.x, q.botRight.y);

    drawLine(q.topLeft, topRight);
    drawLine(topRight, q.botRight);
    drawLine(q.botRight, botLeft);
    drawLine(botLeft, q.topLeft);

    if (q.botRightTree != NULL) {
        drawQuad(*q.botRightTree);
    }
    if (q.topLeftTree != NULL) {
        drawQuad(*q.topLeftTree);
    }
    if (q.botLeftTree != NULL) {
        drawQuad(*q.botLeftTree);
    }
    if (q.topRightTree != NULL) {
        drawQuad(*q.topRightTree);
    }
}

void drawQuads(const simulation& sim) {
    drawQuad(sim.center);
}

void drawParticles(const simulation& sim) {
    for (const particle& p : sim.part) {
        drawPoint(p.position, p.size, p.color);
    }
    if (sim.blachHole) {
        drawPoint(sim.bHole.position, sim.bHole.size, sim.bHole.color);
    }

}



void Draw() {
    simu.updateParameters(partParameters);
    if (!pause) { simu.update(); }
    if (showQuad) { drawQuads(simu); }
    if (showParticle) { drawParticles(simu); } 
}

void Input(int key) {
    switch (key) {
    case GLFW_KEY_F1:pause = !pause; break;
    case GLFW_KEY_F2:alpha = !alpha; break;
    case GLFW_KEY_F3:showQuad = !showQuad; break;
    case GLFW_KEY_F4:fullscreen = !fullscreen; break;
    default: return;
    }
}


//particle bHole = particle(true, 100000.0f, 5, vec4(255, 255, 255, 255), vec2(0, 0), vec2(0, 0));


//parameter that affect next spawning particle
//bool locked;
//bool edgesReset;
//float maxSpeed;
//float mass;
//float size;
//vec4 color;
//vec2 position;
//vec2 oldPosi;
//vec2 speed;
//vec2 acceleration;

void Ui() {
    ImGui::Begin("Parameters");

    //General
    ImGui::Text("General");
    ImGui::Spacing();

    ImGui::Checkbox("Show quad", &showQuad);

    ImGui::Checkbox("Show particles", &showParticle);

    ImGui::Checkbox("Alpha blending", &alpha);

    ImGui::Checkbox("Pause", &pause);

    if (ImGui::Button("Screenshot")) {
        message = TakeScreenshot();
    }
    ImGui::Text(message.c_str());

    ImGui::Spacing();
    ImGui::Separator();

    //Simulation
    ImGui::Text("Simulation");
    ImGui::Spacing();

    ImGui::InputInt("Particles number", &simu.particleN);

    ImGui::InputFloat("Constant G", &simu.G, 0.0f, 1.0f, "%.20f");
    if (ImGui::Button("Reset G")) simu.G = 0.00000000006743f;

    ImGui::InputFloat("Simulation precision", &simu.theta);
    if (ImGui::Button("Reset precision")) simu.theta = 0.5f;

    ImGui::InputFloat("Quad minimum size", &simu.minQuad);
    if (ImGui::Button("Reset minimum size")) simu.minQuad = 0.02f;

    ImGui::Spacing();
    ImGui::Separator();

    //Particle
    ImGui::Text("Particle");
    ImGui::Spacing();

    ImGui::Checkbox("Random Mass", &randomMass);
    ImGui::InputFloat("Particles Mass", &mass);
    ImGui::Checkbox("Size Equal To Mass", &sizeEqualToMass);
    ImGui::InputFloat("Particles Size", &pSize);
    ImGui::InputFloat("Radial Initial Velocity", &radialInitialVelocity);
    ImGui::ColorEdit4("Particles Color", &pColor[0]);



    ImGui::Spacing();
    ImGui::Separator();

    //Black Hole
    ImGui::Text("Black Hole");
    ImGui::Spacing();

    ImGui::Checkbox("Enable black hole", &simu.blachHole);

    if (ImGui::Button("Reset")) {
        reset();
    }


    
    
    

    //Particles



    ImGui::End();

}

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, char* szCmdLine, int iCmdShow) {
    window_name = "barnes-hut";
    onDraw = Draw;
    onInput = Input;
    onDrawUI = Ui;
    init(1500, 1000);
    return 0;
}

//TODO : sim/simu structure a refaire
//TODO : Parameters GUI
//TODO : bug drawing quads
//TODO : clamp parameters
//TODO : updating structure
//TODO : explication paramettre (mouse temp window)
