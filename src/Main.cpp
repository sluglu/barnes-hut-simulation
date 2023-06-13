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

int particleN = 100;
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
    p.drawMode = false;
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

void updateParameters(simulation& sim) {
    for (particle& p : sim.part) {
        partParameters(p);
    }
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


void  drawParticle(const particle& p, GLContext& context) {
    if (p.drawMode) {
        context.drawPoint(p.position, p.size, p.color);
    }
    else {
        context.drawLine(p.oldPosi, p.position, p.size, p.color);
    }
}

void drawParticles(const simulation& sim, GLContext& context) {
    for (const particle& p : sim.part) {
        drawParticle(p, context);
    }
    if (sim.blachHole) {
        drawParticle(sim.bHole, context);
    }

}



void onDraw(GLContext& context) {
    //simu.updateParameters(partParameters);
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
    default: return;
    }
}

int main() {
    context.window_name = "barnes-hut";
    context.onDraw = onDraw;
    context.onInput = onInput;
    context.fullscreen = false;
    simu.particleN = particleN;
    simu.blachHole = false;
    simu.computeOnGpu = false;
    updateParameters(simu);
    context.init(1500, 1000);
    return 0;
}

//TODO : bug flickering alpha (a +10k)
//TODO : dynamic switch BH/NB
//TODO : bug BH/NB at 10k mauvaise dir
//TODO : sim/simu structure a refaire
//todo : probleme GPU