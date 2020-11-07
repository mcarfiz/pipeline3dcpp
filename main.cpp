#include "pipeline.h"

int main(){

    //Instantiation of different Vertex objects, useful if some or all are used by 2 or more Scene objects
    //Otherwise, vertices can be passed to a Scene via rvalue as shown later
    Vertex v1(1.0f, -1.0f, 1.5f);
    Vertex v2(1.0f, 1.0f, 1.1f);
    Vertex v3(-1.0f, 1.0, 1.5f);
    Vertex v4(-1.0, -1.0, 1.9f);
    std::vector<Vertex> vertices {v1,v2,v3,v4};

    //left = -1, right = 1, top = -1, bottom = 1, near = 1, far=2
    ProjectionMatrix pm1(-1, 1, -1, 1, 1, 2);
    ProjectionMatrix pm2(0, 10, 7, -1, 1, 5);

    //Instantiation of different types of fragment shaders (different behaviours, strategy pattern)
    SimpleFragmentShader sfs;
    SimpleIntShader ifs;

    //Pipeline with the char shader behaviour
    Pipeline<char, 150, 50> pChar (pm1, &sfs);

    //Pipeline with the int shader behaviour
    Pipeline<int, 150, 50> pInt (pm1, &ifs);

    
    //Scene creation via lvalue array of Vertex 
    Scene scene_1(vertices, {{0,1,2}, {0,2,3}});
    //Scene creation via rvalue array of Vertex (same scene as the first one)
    Scene scene_2({v1, v2, v3, v4}, {{0,1,2}, {0,2,3}});
    //Scene creation completely via rvalues, no Vertex instantiation needed
    Scene scene_3({{0, -1, 1.5f}, {1, 1, 1.5f}, {-1, 1, 1.5}, {-0.75, -1, 1.9}, {1.5, 0.75, 1.9}, {0, 1, 1.1}}, {{0,1,2}, {3,4,5}});
    
    // Rendering, printing and saving the first scene, using the char based pipeline
    pChar.render(scene_1).print().fileSave("test").clear();

    // //Rendering and printing the second scene, using the int based pipeline
     pInt.render(scene_2).print().clear();

    //Rendering and printing the third scene, using the char based pipeline
    pChar.render(scene_3).print();



    return 0;
}
