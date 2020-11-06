#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm> 
#include <cmath>

using Triangle = std::array<size_t, 3>;


// Class representing a 3d vertex and its coordinates
class Vertex{
    private:
        const float x_, y_, z_ = 0.0f;
        float ndx_, ndy_, ndz_ = 0.0f;
        
        //mancano vettore 3dnormal e coordinate u,v anche se non vengono usate

    public:
        //Constructors
        Vertex() = default;
        Vertex(const float x, const float y, const float z): x_(x), y_(y), z_(z){}
        Vertex(const Vertex& v) : x_(v.x_), y_(v.y_), z_(v.z_), ndx_(v.ndx_), ndy_(v.ndy_), ndz_(v.ndz_){}

        // Getters
        float getX()  {return x_;}
        float getY()  {return y_;}
        float getZ()  {return z_;}

        float getNdx()  {return ndx_;}
        float getNdy()  {return ndy_;}
        float getNdz()  {return ndz_;}


        // Setters
        void setNdx(const float x){ndx_ = x;}
        void setNdy(const float y){ndy_= y;}
        void setNdz(const float z){ndz_ = z;}


};


class Scene {
    private:
        std::vector<Vertex>  vertices_;
        std::vector<Triangle> triangles_;
    public:
        Scene() = default;
        //Copy constructor (constructor based on a vector of unique vertices and a vector of coordinates corresponding to)
        Scene(const std::vector<Vertex>& vertices, const std::vector<Triangle>& coordinates) : vertices_(vertices), triangles_(coordinates){}
        //Move constructor
        Scene(std::vector<Vertex>&& vertices, const std::vector<Triangle>& coordinates) : vertices_(std::move(vertices)), triangles_(std::move(coordinates)){}


        //Getter for reference of triangles' vector used by the scene
        std::vector<Triangle>& getSceneTriangles(){
            return triangles_;
        }
        //Getter for reference of vertices' vector 
        std::vector<Vertex>& getSceneVertices(){
            return vertices_;
        }

        Vertex& operator()(const size_t triangle_idx, const size_t vertex_idx){
            if (triangle_idx >= triangles_.size() || vertex_idx > 2)
                throw std::out_of_range("Vertex out of range");
            return vertices_[((triangles_[triangle_idx])[vertex_idx])];
        }

};