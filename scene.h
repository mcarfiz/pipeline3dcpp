/*  
Giacomo Arrigo 860022
Marco Carfizzi 860149
*/

#include "vertex.h"

// Arrays (size_t elements) with size 3 are called Triangle, but are in fact "coordinates" and NOT sets of Vertex objects, as shown later
using Triangle = std::array<size_t, 3>;


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