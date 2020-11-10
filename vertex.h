#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm> 
#include <cmath>

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