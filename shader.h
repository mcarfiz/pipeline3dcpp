/*  
Giacomo Arrigo 860022
Marco Carfizzi 860149
*/

#include "frustum.h"

// Shader interface, application of the strategy pattern for the fragment shader
template <typename target_t>
class IFragmentShader{
    public:
        virtual target_t computeShader(double x, double y, double z, double an, double bn, double cn, double u, double v) = 0;
        virtual ~IFragmentShader() {}
};

// Implementations of the strategy interface
// Each shader can have multiple implementations and each implementation is hot-swappable
// SimpleFragmentShader gives the first decimal of the value of z to the fragment, used when target_t is a char
class SimpleFragmentShader : public IFragmentShader<char>{
    public:
        char computeShader(double x, double y, double z, double an, double bn, double cn,  double u, double v){
            return  48 + (int)((z - floor(z))*10);
        }
};

// Shader that produces a flat output by coloring the pixels with an 'x' (char case)
class X2DFragmentShader : public IFragmentShader<char>{
    public:
        char computeShader(double x, double y, double z, double an, double bn, double cn, double u, double v){
            return 'x';
        }
};

// SimpleIntShader gives the first decimal of the value of z to the fragment, used when target_t is an int
class SimpleIntShader : public IFragmentShader<int>{
    public:
        int computeShader(double x, double y, double z, double an, double bn, double cn, double u,  double v){
            return ((z - floor(z))*10);
        }
};