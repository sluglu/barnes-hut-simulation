#include "particle.h"

using namespace std;

struct Node {
    vec2 pos = vec2(0,0);
    float data;
    Node(vec2 _pos, float _data)
    {
        pos = _pos;
        data = _data;
    }
    Node() { data = 0; }
};

class Quad {
public:
    float Mass = 0;
    vec2 CenterOfMass = vec2(0,0);
    // Hold details of the boundary of this node
    vec2 topLeft;
    vec2 botRight;
 
    // Contains details of node
    Node* n;
    vec2 N = vec2(0,0);

    // Children of this tree
    Quad* topLeftTree;
    Quad* topRightTree;
    Quad* botLeftTree;
    Quad* botRightTree;

    Quad()
    {
        topLeft = vec2(0.0f, 0.0f);
        botRight = vec2(0.0f, 0.0f);
        n = NULL;
        topLeftTree = NULL;
        topRightTree = NULL;
        botLeftTree = NULL;
        botRightTree = NULL;
    }
    Quad(vec2 topL, vec2 botR)
    {
        n = NULL;
        topLeftTree = NULL;
        topRightTree = NULL;
        botLeftTree = NULL;
        botRightTree = NULL;
        topLeft = topL;
        botRight = botR;
    }
    void insert(Node*,float);
    Node* search(vec2);
    bool inBoundary(vec2);
    void ComputeMassDistribution();
    glm::vec2 calculateForce(particle&, const float&, const float&);
    void dellocate();
};