#include "pipeline.h"
#include <chrono>
int main(){
    //Render<char, 15, 5> video;
    //video(0,0) = '1';
    //cout << video(0, 0) << "\n";
    //cout << video;
    //video.fileSave("test");

    std::vector<Vertex> vertices;
    Vertex v1(1.0f, -1.0f, 1.5f);
    Vertex v2(1.0f, 1.0f, 1.1f);
    Vertex v3(-1.0f, 1.0, 1.5f);
    Vertex v4(-1.0, -1.0, 1.9f);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);


    //left=top=-1, right=bottom=near=1, and far=2;
    ProjectionMatrix pm1(-1, 1, -1, 1, 1, 2);

    SimpleFragmentShader sfs;
    SimpleIntShader ifs;
    X2DFragmentShader xfs;

    Pipeline<char, 150, 50> pChar (pm1, &sfs);
    Pipeline<int, 150, 50> pInt (pm1, &ifs);
    
    
    Scene scena1(vertices, {{0,1,2}, {0,2,3}});
    Scene scena2({v1, v2, v3, v4}, {{0,1,2}, {0,2,3}});
    Scene scena3({{1.0f, -1.0f, 1.5f}, {1.0f, 1.0f, 1.1f}, {-1.0f, 1.0, 1.5f}, {-1.0, -1.0, 1.9f}}, {{0,1,2}, {0,2,3}});
    
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    pChar.render(scena1).print();
    pChar.fileSave("test");
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed_time = std::chrono::duration<double>(end_time-start_time).count();
    std::cout << "multi-slice elapsed time: " << elapsed_time << '\n';
    pChar.render(scena2).print();

    pInt.render(scena1).print();
    pInt.render(scena2).print();

    pChar.render(scena3).print();

    return 0;
}
