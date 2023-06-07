#include "glm/glm.hpp"
using namespace glm;



class particle{
    public:
        particle();
        particle(bool, bool, float, float, vec4, vec2, vec2);
        bool drawMode;
        bool locked;
        bool edgesReset;
        float maxSpeed;
        float mass;
        float size;
        vec4 color;
        vec2 position;
        vec2 oldPosi;
        vec2 speed;
        vec2 acceleration;
        void update();
        void applyForce(vec2&);
        void edges();
};

