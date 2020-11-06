#include <iostream>
#include "pipeline.h"


int main(){
    //Render<char, 15, 5> video;
    //video(0,0) = '1';
    //cout << video(0, 0) << "\n";
    //cout << video;
    //video.fileSave("test");

    // using a pointer since the coordinates will be overwritten
    std::vector<Vertex*> vertices;
    Vertex v1(1.0f, -1.0f, 1.5f);
    vertices.push_back(&v1);
    Vertex v2(1.0f, 1.0f, 1.1f);
    vertices.push_back(&v2);
    Vertex v3(-1.0f, 1.0, 1.5f);
    vertices.push_back(&v3);
    Vertex v4(-1.0, -1.0, 1.9f);
    vertices.push_back(&v4);

    std::vector<Triangle> triangles;
    Triangle t1(&v1 ,&v2, &v3);
    triangles.push_back(t1);
    Triangle t2(&v1, &v3, &v4);
    triangles.push_back(t2);
    //cout << v1.getZ() << "\n";

    //left=top=-1, right=bottom=near=1, and far=2;
    ProjectionMatrix pm1(-1, 1, -1, 1, 1, 2);

    SimpleFragmentShader sfs;
    SimpleIntShader ifs;
    X2DFragmentShader xfs;

    Pipeline<char, 150, 50> p1 (pm1, &sfs);
    // Pipeline<char, 150, 50> p2 (p1);
    Pipeline<int, 150, 50> p3 (pm1, &ifs);
    
    p1.render(vertices, triangles);
    p1.print();
    p1.fileSave("test");

    p3.render(vertices, triangles);
    p3.print();
    return 0;
}
