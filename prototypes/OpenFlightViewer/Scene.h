
#pragma once

#include <vector>

class Node;
class VertexPool;

class Scene
{
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    void update();
    void updateTransform();
    void updateTransform(Node*);

    // ca serait plaisant d'avoir un genre d'iterateur
    // sur les nodes... au lieu de faire un parcour de
    //
    // graph a chaque fois...
    //
    // pour faire
    // for(it = begin(); it != end; ++it)
    // {
    //    blablabla
    // }
    
    //data
    Node* mpRoot;
};