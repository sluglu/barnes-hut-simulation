#include "simulation.h"


void simulation::updateParameters(void(*f)(particle&)) {
    part = std::vector<particle>(particleN, p);
    for (particle& p : part) {
        f(p);
    }
    if(computeOnGpu){ createBuffer(); }
    
}


void simulation::createQuad()
{
    center.dellocate();
    center = Quad(vec2(-1.0f, 1.0f), vec2(1.0f, -1.0f));
    for (particle& p : part) {
        vec2 point(p.position.x, p.position.y);
        Node n = Node(point, p.mass);
        center.insert(&n, minQuad);
    }
    if (blachHole) {
        vec2 point(bHole.position.x, bHole.position.y);
        Node n = Node(point, bHole.mass);
        center.insert(&n, minQuad);
    }
    center.ComputeMassDistribution();
}


void calculateForces(int start, int end, simulation* s) {
    for (int i = start; i < end; i++) {
        std::mutex force_mutex;
        std::lock_guard<std::mutex> lock(force_mutex);
        vec2 force = s->center.calculateForce(s->part[i], s->G, s->theta);
        s->part[i].applyForce(force);
    }
}

void simulation::calculateForcesThread() {
    int num_threads = std::thread::hardware_concurrency();
    int chunk_size = part.size() / num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? part.size() : (i + 1) * chunk_size;
        threads.push_back(std::thread(calculateForces,start, end, this));
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}


void simulation::createBuffer() {
    shader = ComputeShader("C:/Users/eliot/Documents/programme/barnes-hutt v2 vs/barnes-hutt v2 vs/src/computeShader.comp");

    int sizeInBytesIN = sizeof(vec4) * part.size() + 1;
    int sizeInBytesOUT = sizeof(vec2) * part.size();

    glCreateBuffers(1, &buffIN);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffIN);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffIN);
    

    glCreateBuffers(1, &buffOUT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffOUT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffOUT);
    glNamedBufferStorage(buffOUT, sizeInBytesOUT, NULL, GL_DYNAMIC_STORAGE_BIT);
} 

void simulation::calculateForcesGPU() {

    std::vector<vec4> data = std::vector<vec4>(part.size() + 1, vec4(0, 0, 0, 0));
    std::vector<vec2> out = std::vector<vec2>(part.size(), vec2(0, 0));


    for (int i = 0; i < part.size(); i++) {
        data[i] = vec4(part[i].position.x, part[i].position.y, part[i].mass, 0);
    }
    if (blachHole) {
        data.push_back(vec4(bHole.position.x, bHole.position.y, bHole.mass, 0));
    }
    
    int sizeInBytesIN = sizeof(vec4) * data.size();
    int sizeInBytesOUT = sizeof(vec2) * out.size();
    
    glNamedBufferStorage(buffIN, sizeInBytesIN, NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(buffIN, 0, sizeInBytesIN, data.data());

    shader.use();
    shader.setFloat("G", G);
    shader.setFloat("A", minDst);
    glDispatchCompute(ceil(data.size() / 32.0f), ceil(data.size() / 32.0f), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
  
    glGetNamedBufferSubData(buffOUT, 0, sizeInBytesOUT, &out[0]);
    
    for (unsigned int i = 0; i < part.size(); i++) {
        part[i].applyForce(out[i]);
        std::cout << out[i].x << " | ";
    }
}

void simulation::updateParticles() {
    for (particle& p : part) {
        p.update();
    }
}

void simulation::update() {
    if (computeOnGpu) { updateGPU(); }
    else { updateCPU(); }
}

void simulation::updateGPU() {

    calculateForcesGPU();

    updateParticles();

}

void simulation::updateCPU() {

    createQuad();

    calculateForcesThread();

    updateParticles();

}

void simulation::drawQuad(Quad* q) {
    glPointSize(1);
    vec2 topRight = vec2(q->botRight.x, q->topLeft.y);
    vec2 botLeft = vec2(q->topLeft.x, q->botRight.y);
    glBegin(GL_LINES);
    glColor4f(255, 255, 255, 255);
    glVertex2f(q->topLeft.x, q->topLeft.y);
    glVertex2f(topRight.x, topRight.y);

    glVertex2f(topRight.x, topRight.y);
    glVertex2f(q->botRight.x, q->botRight.y);

    glVertex2f(q->botRight.x, q->botRight.y);
    glVertex2f(botLeft.x, botLeft.y);

    glVertex2f(botLeft.x, botLeft.y);
    glVertex2f(q->topLeft.x, q->topLeft.y);
    glEnd();
    if (q->botRightTree != NULL) {
        simulation::drawQuad(q->botRightTree);
    }
    if (q->topLeftTree != NULL) {
        simulation::drawQuad(q->topLeftTree);
    }
    if (q->botLeftTree != NULL) {
        simulation::drawQuad(q->botLeftTree);
    }
    if (q->topRightTree != NULL) {
        simulation::drawQuad(q->topRightTree);
    }
}

void simulation::drawQuad() {
    simulation::drawQuad(&center);
}


void  simulation::drawParticle(particle* p) {
    glPointSize(p->size);
    if (p->drawMode) {
        glBegin(GL_POINTS);
        glColor4f(p->color.x, p->color.y, p->color.z, p->color.w);
        glVertex2f(p->position.x, p->position.y);
    }
    else {
        glBegin(GL_LINES);
        glColor4f(p->color.x, p->color.y, p->color.z, p->color.w);
        glVertex2f(p->position.x, p->position.y);
        glVertex2f(p->oldPosi.x, p->oldPosi.y);
    }
    glEnd();
    p->oldPosi = p->position;
}

void simulation::drawParticle() {
    for (particle& p : part) {
        simulation::drawParticle(&p);
    }
    if (blachHole) {
        simulation::drawParticle(&bHole);
    }
    
}