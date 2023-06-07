#include <thread>
#include <mutex>
#include <vector>
#include "quad.h"
#include "GLContext.h"
#include "ComputeShader.h"



class simulation {
	GLuint buffIN;
	GLuint buffOUT;
	ComputeShader shader = ComputeShader();
	particle p;
public:
	int particleN = 100;
	bool computeOnGpu = false;
	bool blachHole = true;
	const float G = 0.00000000006743f;
	const float theta = 0.5f;
	const float minQuad = 0.02f;
	const float minDst = 0.02f;
	std::vector<particle> part = std::vector<particle>(particleN, p);
	particle bHole = particle(true, true, 100000.0f, 5, vec4(255, 255, 255, 255), vec2(0, 0), vec2(0, 0));
	Quad center = Quad(vec2(-1.0f, 1.0f), vec2(1.0f, -1.0f));
	void updateParameters(void(*f)(particle&));
	void createQuad();
	void calculateForcesThread();
	void calculateForcesGPU();
	void updateParticles();
	void update();
	void createBuffer();
	void updateGPU();
	void updateCPU();
	void drawQuad();
	void drawQuad(Quad*);
	void drawParticle();
	void drawParticle(particle*);
};