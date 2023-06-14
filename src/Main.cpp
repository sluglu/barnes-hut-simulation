#include <math.h>
#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <tuple>

//#include "GLContext.h"
#include "simulation.h"


GLContext context;
simulation simu;

int particleN = 10000;
bool pause = false;
bool saveToImg = false;
bool showQuad = false;
bool showParticle = true;

int imgCount = 0;


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
    p.mass = float(rand() % 1001 - 200);
    p.size = abs(p.mass*0.004f);
    p.color = vec4(255, 255, 255, 255);
    p.position = vec2(cos(rand()) * 0.9f, sin(rand()) * 0.9f);
    p.oldPosi = p.position;
    //p.speed = vec2(p.position.y * 0.005f, -p.position.x * 0.005f);
    p.acceleration = vec2();
    applyColor(p);
}

void drawQuad(const Quad& q, GLContext& context) {
    vec2 topRight = vec2(q.botRight.x, q.topLeft.y);
    vec2 botLeft = vec2(q.topLeft.x, q.botRight.y);

    context.drawLine(q.topLeft, topRight);
    context.drawLine(topRight, q.botRight);
    context.drawLine(q.botRight, botLeft);
    context.drawLine(botLeft, q.topLeft);

    if (q.botRightTree != NULL) {
        drawQuad(*q.botRightTree, context);
    }
    if (q.topLeftTree != NULL) {
        drawQuad(*q.topLeftTree, context);
    }
    if (q.botLeftTree != NULL) {
        drawQuad(*q.botLeftTree, context);
    }
    if (q.topRightTree != NULL) {
        drawQuad(*q.topRightTree, context);
    }
}

void drawQuads(const simulation& sim, GLContext& context) {
    drawQuad(sim.center, context);
}

void drawParticles(const simulation& sim, GLContext& context) {
    for (const particle& p : sim.part) {
        context.drawPoint(p.position, p.size, p.color);
    }
    if (sim.blachHole) {
        context.drawPoint(sim.bHole.position, sim.bHole.size, sim.bHole.color);
    }

}



void onDraw(GLContext& context) {
    if (!pause) { simu.update(); }
    
    if (saveToImg) { 
        imgCount += 1; 
        string filePath = string("C:/Users/eliot/Desktop/test/") + to_string(imgCount) + ".png";
        //const char* filePath = n.c_str();
        context.TakeScreenshot(filePath); 
    }
    if (showQuad) { drawQuads(simu, context); }
    if (showParticle) { drawParticles(simu, context); } 
}

void onInput(GLContext& context, int key) {
    switch (key) {
    case GLFW_KEY_F1:pause = !pause; break;
    case GLFW_KEY_F2:context.alpha = !context.alpha; break;
    case GLFW_KEY_F3:showQuad = !showQuad; break;
    case GLFW_KEY_F4:context.fullscreen = !context.fullscreen; break;
    case GLFW_KEY_F5:simu.computeOnGpu = !simu.computeOnGpu; break;
    default: return;
    }
}

void initialize(GLContext& context) {
    simu.particleN = particleN;
    simu.updateParameters(partParameters);
    
}

int main() {
    context.window_name = "barnes-hut";
    context.onDraw = onDraw;
    context.onInput = onInput;
    context.initialize = initialize;
    context.fullscreen = false;
    context.init(1500, 1000);
    return 0;
}

//TODO : bug BH/NB at 10k mauvaise dir avec mass negative
//TODO : sim/simu structure a refaire
//TODO : runtime particleN updating + create buffer call one time
