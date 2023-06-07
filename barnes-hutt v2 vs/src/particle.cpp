#include "particle.h"

particle::particle(bool d, bool l, float m, float s, vec4 c, vec2 p, vec2 sp) {
    drawMode = d;
    locked = l;
    edgesReset = false;
    maxSpeed = 0.05f;
    mass = m;
    size = s;
    color = c;
    position = p;
    oldPosi = position;
    speed = sp;
    acceleration = vec2(0,0);
}

particle::particle() {
    drawMode = true;
    locked = false;
    edgesReset = false;
    maxSpeed = 0.05f;
    mass = 1000;
    size = 1;
    color = vec4(255, 255, 255, 255);
    position = vec2(0,0);
    oldPosi = position;
    speed = vec2(0,0);
    acceleration = vec2(0,0);
}

void particle::update(){
    if (locked == false){
        speed += acceleration;
        if (sqrt(speed.x*speed.x + speed.y*speed.y) > maxSpeed) {
	        speed = normalize(speed) * maxSpeed;
        }
        position += speed;
        acceleration = vec2(0,0);
        if (edgesReset == true){edges();}
    }
}

void particle::applyForce(vec2& force){
    acceleration += force/mass;
}

void particle::edges(){
    if (position.y > 1){ position.y = -1;oldPosi = position;}
    if (position.y < -1){ position.y = 1;oldPosi = position;}
    if (position.x > 1){ position.x = -1;oldPosi = position;}
    if (position.x < -1){ position.x = 1;oldPosi = position;}
}