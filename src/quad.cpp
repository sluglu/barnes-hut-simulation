#include "quad.h"
using namespace std;

// Insert a node into the quadtree
void Quad::insert(Node* node, float minQuad)
{
    if (node == NULL){
        return;
    }
        
 
    // Current quad cannot contain it
    if (!inBoundary(node->pos)){
        return;
    }
        
 
    // We are at a quad of unit area
    // We cannot subdivide this quad further
    if (abs(botRight.x - topLeft.x) <= minQuad
        && abs(topLeft.y - botRight.y) <= minQuad) {
        if (n == NULL)
            n = node;
            N = vec2(node->pos.x, node->pos.y);
        return;
    }
    //left
    if ((topLeft.x + botRight.x) / 2.0f >= node->pos.x) {
        // Indicates topLeftTree
        if ((topLeft.y + botRight.y) / 2.0f <= node->pos.y) {
            if (topLeftTree == NULL){
                topLeftTree = new Quad(
                    vec2(topLeft.x, topLeft.y),
                    vec2((topLeft.x + botRight.x) / 2.0f,
                          (topLeft.y + botRight.y) / 2.0f));
            }
            topLeftTree->insert(node, minQuad);
            
        }
 
        // Indicates botLeftTree
        else {
            if (botLeftTree == NULL){
                botLeftTree = new Quad(
                    vec2(topLeft.x,
                          (topLeft.y + botRight.y) / 2.0f),
                    vec2((topLeft.x + botRight.x) / 2.0f,
                          botRight.y));
            }
            
            botLeftTree->insert(node, minQuad);
        }
    }
    //right
    else{
        // Indicates topRightTree
        if ((topLeft.y + botRight.y) / 2.0f <= node->pos.y) {
            if (topRightTree == NULL){
                topRightTree = new Quad(
                    vec2((topLeft.x + botRight.x) / 2.0f,
                          topLeft.y),
                    vec2(botRight.x,
                          (topLeft.y + botRight.y) / 2.0f));
            }
            topRightTree->insert(node, minQuad);
        }
 
        // Indicates botRightTree
        else {
            if (botRightTree == NULL){
                botRightTree = new Quad(
                    vec2((topLeft.x + botRight.x) / 2.0f,
                          (topLeft.y + botRight.y) / 2.0f),
                    vec2(botRight.x, botRight.y));
            }
            botRightTree->insert(node, minQuad);
        }
    }
}
 

void Quad::dellocate(){
    if(botRightTree != NULL){
        botRightTree->dellocate();
    }
    if(topLeftTree != NULL){
        topLeftTree->dellocate();
    }
    if(botLeftTree != NULL){
        botLeftTree->dellocate();
    }
    if(topRightTree != NULL){
        topRightTree->dellocate();
    }
    delete topLeftTree;
    delete botLeftTree;
    delete topRightTree;
    delete botRightTree;
}
// Find a node in a quadtree
Node* Quad::search(vec2 p)
{
    // Current quad cannot contain it
    if (!inBoundary(p))
        return NULL;
    // We are at a quad of unit length
    // We cannot subdivide this quad further
    if (n != NULL)
        return n;
 
    if ((topLeft.x + botRight.x) / 2 >= p.x) {
        // Indicates topLeftTree
        if ((topLeft.y + botRight.y) / 2 >= p.y) {
            if (topLeftTree == NULL)
                return NULL;
            return topLeftTree->search(p);
        }
 
        // Indicates botLeftTree
        else {
            if (botLeftTree == NULL)
                return NULL;
            return botLeftTree->search(p);
        }
    }
    else {
        // Indicates topRightTree
        if ((topLeft.y + botRight.y) / 2 >= p.y) {
            if (topRightTree == NULL)
                return NULL;
            return topRightTree->search(p);
        }
 
        // Indicates botRightTree
        else {
            if (botRightTree == NULL)
                return NULL;
            return botRightTree->search(p);
        }
    }
};
 
// Check if current quadtree contains the point
bool Quad::inBoundary(vec2 p){
    return (p.x >= topLeft.x && p.x <= botRight.x
            && p.y <= topLeft.y && p.y >= botRight.y);
}


void  Quad::ComputeMassDistribution()
{   
    vec2 cm = vec2(0,0);
    Mass = 0;
    
    if (n != NULL)
    {
        Mass = n->data;
        CenterOfMass = N;
    }
    else
    {
        if(botRightTree != NULL){
            botRightTree->ComputeMassDistribution();
            Mass += botRightTree->Mass;
            cm += vec2(botRightTree->CenterOfMass.x, botRightTree->CenterOfMass.y)* botRightTree->Mass;
        }
        if(topLeftTree != NULL){
            topLeftTree->ComputeMassDistribution();
            Mass += topLeftTree->Mass;
            cm += vec2(topLeftTree->CenterOfMass.x, topLeftTree->CenterOfMass.y)* topLeftTree->Mass;
        }
        if(botLeftTree != NULL){
            botLeftTree->ComputeMassDistribution();
            Mass += botLeftTree->Mass;
            cm += vec2(botLeftTree->CenterOfMass.x, botLeftTree->CenterOfMass.y)* botLeftTree->Mass;
        }
        if(topRightTree != NULL){
            topRightTree->ComputeMassDistribution();
            Mass += topRightTree->Mass;
            cm += vec2(topRightTree->CenterOfMass.x, topRightTree->CenterOfMass.y)* topRightTree->Mass;
        }
        cm = cm/Mass;
        CenterOfMass = vec2(cm.x, cm.y);
    }
}

float calcInvSqRoot(const float& n ) {
   
   const float threehalfs = 1.5F;
   float y = n;
   
   long i = * ( long * ) &y;

   i = 0x5f3759df - ( i >> 1 );
   y = * ( float * ) &i;
   
   y = y * ( threehalfs - ( (n * 0.5F) * y * y ) );
   
   return y;
}

vec2 Quad::calculateForce(particle& p, const float& G, const float& theta){
    vec2 force = vec2(0,0);
    vec2 cm = vec2(CenterOfMass.x, CenterOfMass.y);
    vec2 u = cm - p.position;
    float r = calcInvSqRoot((float)(pow(u.x, 2.0f) + pow(u.y, 2.0f)));
    float w = abs((botRight.x - topLeft.x));
    if (w*r < theta){
        float g = (G * p.mass * Mass) * pow(r, 2.0f);
        force += g * (u*r);
        return force;
    }
    else{
        if(botRightTree != NULL){
        force += botRightTree->calculateForce(p,G,theta);
        }
        if(topLeftTree != NULL){
        force += topLeftTree->calculateForce(p,G,theta);
        }
        if(botLeftTree != NULL){
        force += botLeftTree->calculateForce(p,G,theta);
        }
        if(topRightTree != NULL){
        force += topRightTree->calculateForce(p,G,theta);
        }
        return force;
    }
}