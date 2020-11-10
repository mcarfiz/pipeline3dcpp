#include "render.h"
// Structure for the view frustum, so that its values can be supplied by the user to the projection matrix
class ProjectionMatrix{
    private: 
        double left_, right_, top_, bottom_, near_, far_ = 0.0f;
    
    public:
        ProjectionMatrix(double left, double right, double top, double bottom, double near, double far) : left_(left), right_(right), top_(top), bottom_(bottom), near_(near), far_(far){}

        // Default constructor
        ProjectionMatrix() = default;

        // Getters methods
        double getLeft(){return left_;}
        double getRight(){return right_;}
        double getTop(){return top_;}
        double getBottom(){return bottom_;}
        double getNear(){return near_;}
        double getFar(){return far_;}
};