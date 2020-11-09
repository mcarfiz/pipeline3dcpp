#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm> 
#include <cmath>

// Arrays (size_t elements) with size 3 are called Triangle, but are in fact "coordinates" and NOT sets of Vertex objects, as shown later
using Triangle = std::array<size_t, 3>;


// Class representing a 3d vertex and its coordinates
class Vertex{
    private:
        // Constant, not-perspective projected coordinates, used because different pipelines (and different rendering behaviours) may re-use vertices
        const double x_, y_, z_ = 0.0f;
        // Perspective projected coordinates, re-calculated every time a pipeline renders, using the Vertex
        double ndx_, ndy_, ndz_ = 0.0f;

    public:
        // Constructors
        Vertex() = default;
        Vertex(const double x, const double y, const double z): x_(x), y_(y), z_(z){}
        Vertex(const Vertex& v) : x_(v.x_), y_(v.y_), z_(v.z_), ndx_(v.ndx_), ndy_(v.ndy_), ndz_(v.ndz_){}

        // Getters of the constant coordinates
        double getX()  {return x_;}
        double getY()  {return y_;}
        double getZ()  {return z_;}

        // Getters of the pipeline-computed coordinates
        double getNdx()  {return ndx_;}
        double getNdy()  {return ndy_;}
        double getNdz()  {return ndz_;}

        // Setters
        void setNdx(const double x){ndx_ = x;}
        void setNdy(const double y){ndy_= y;}
        void setNdz(const double z){ndz_ = z;}


};

// Representation of a scene, with the unique vertices and the coordinates of the triangles' edges with respect to the vertices' array
class Scene {
    private:
        std::vector<Vertex>  vertices_;
        std::vector<Triangle> triangles_;
    public:
        Scene() = default;
        // Copy constructor 
        Scene(const std::vector<Vertex>& vertices, const std::vector<Triangle>& coordinates) : vertices_(vertices), triangles_(coordinates){}
        // Move constructor
        Scene(std::vector<Vertex>&& vertices, std::vector<Triangle>&& coordinates) : vertices_(std::move(vertices)), triangles_(std::move(coordinates)){}


        // Getter for reference of triangles' vector used by the scene
        std::vector<Triangle>& getSceneTriangles(){
            return triangles_;
        }
        // Getter for reference of vertices' vector 
        std::vector<Vertex>& getSceneVertices(){
            return vertices_;
        }

        // Operator () overload, Scene(triangle_idx, vertex_idx) returns a Vertex reference to the "vertex_idx" Vertex of the "triangle_idx" Triangle of the Scene
        // Example: scene_ex(1, 0) returns the first Vertex of the second Triangle of Scene scene_ex
        Vertex& operator()(const size_t triangle_idx, const size_t vertex_idx){
            if (triangle_idx >= triangles_.size() || vertex_idx > 2)
                throw std::out_of_range("Vertex out of range");
            return vertices_[((triangles_[triangle_idx])[vertex_idx])];
        }

};