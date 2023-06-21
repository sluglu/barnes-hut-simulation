#include <math.h>
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <tuple>

//#include "GLContext.h"
#include "simulation.h"

simulation simu;

int particleN = 10000;
bool pause = false;
bool saveToImg = false;
bool showQuad = false;
bool showParticle = true;

int imgCount = 0;

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

void partParameters(particle& p) {
    p.locked = false;
    p.edgesReset = false;
    p.maxSpeed = 0.05f;
    p.mass = float(rand() % 1001);
    p.size = abs(p.mass*0.004f);
    p.color = vec4(255, 255, 255, 255);
    p.position = vec2(cos(rand()) * 0.9f, sin(rand()) * 0.9f);
    p.oldPosi = p.position;
    //p.speed = vec2(p.position.y * 0.005f, -p.position.x * 0.005f);
    p.acceleration = vec2();
    applyColor(p);
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
    if (!pause) { simu.update(); }
    
    if (saveToImg) { 
        imgCount += 1; 
        string filePath = string("C:/Users/eliot/Desktop/test/") + to_string(imgCount) + ".png";
        //const char* filePath = n.c_str();
        TakeScreenshot(filePath); 
    }
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

void init() {
    simu.particleN = particleN;
    simu.updateParameters(partParameters);
    
}

int WinMain() {
    window_name = "barnes-hut";
    onDraw = Draw;
    onInput = Input;
    initialize = init;
    fullscreen = false;
    init(1500, 1000);
    return 0;
}

//TODO : sim/simu structure a refaire
//TODO : Parameter GUI
