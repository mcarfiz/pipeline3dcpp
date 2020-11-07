#include "scene.h"
/* implementation of the software target 
 * if the Render class represent a screen, the target_t is the type of the smallest part ("pixel")
 * C is the width and R is the height of the screen
 */
template<typename target_t, size_t C, size_t R>
class Render{
    private:
        // The class holds a std::array to hold the screen matrix in a contiguous memory section
        std::array<target_t, R*C> container_;

    public:
        void clear_container(){
            for (size_t i = 0; i < container_.size(); i++){
                    // std::vector container is filled with something representing void
                    container_[i] = (target_t)(' ');
            }
        }
        // Default constructor
        Render(){
            this->clear_container();
        }

        // Copy constructor
        Render(const Render<target_t, C, R> &t){
            container_ = t.container_;
        }

        // Move constructor
        Render(Render<target_t, C, R> &&t){
            container_ = std::move(t.container_);
        }


        // Getter methods
        size_t getWidth(){return C;}
        size_t getHeight(){return R;}
        std::array<target_t, C * R>& getTarget(){return container_;}

        // Overloading of () operator, this allows for direct access to location (x, y) of the target
        // hiding the fact that the matrix is in fact represented in one dimension
        target_t& operator()(const size_t y, const size_t x){
            if (x >= R || y >= C)
                throw std::out_of_range("Target indices out of range");
            return container_[C*x + y];
        }

        // Setting the overload of the << operator for the print 
        template <typename T, size_t Co, size_t Ro> 
        friend std::ostream& operator << (std::ostream& stream, Render<T, Co, Ro>& video);

        // Save the container into a file (file name is chosen by the user)
        void fileSave(std::string filename){
            std::ofstream outFile("./" + filename + ".dat");

            int col = 1;
            size_t cont = 0;
            outFile << "+";
            for (size_t i = 0; i < C; i++)
                outFile << "-";
            outFile << "+\n|";
            for (const auto &i : container_){
                // Check for space in int case
                if (i == 32)
                    outFile << ' ';
                else
                    outFile << i;
                // Check if a row has ended and eventually break the line
                if (cont == col * (C) -1){
                    col++;
                    outFile << "|\n";
                    if (col <= R)
                        outFile << "|";
                }
                cont++;
            }
            outFile << "+";
            for (size_t i = 0; i < C; i++)
                outFile << "-";
            outFile << "+\n";
            std::cout << "File " << filename << ".dat correctly saved.\n";
        }
};

// Overload of the << operator for a simpler print
template <typename target_t, size_t C, size_t R>
std::ostream& operator << (std::ostream& stream, Render<target_t, C, R>& video){
    int col = 1;
    size_t cont = 0;
    stream << "+";
    for (size_t i = 0; i < C; i++)
        stream << "-";
    stream << "+\n|";
    for (const auto& i: video.getTarget()){
        // Check for space in int case
        if (i == 32)
            stream << ' ';
        else
            stream << i;
        // Check if a row has ended and eventually break the line
        if (cont == col * (video.getWidth()) -1){
            col++;
            stream << "|\n";
            if (col <= R)
                stream << "|";
        }
        cont++;
    }
    stream << "+";
    for (size_t i = 0; i < C; i++)
        stream << "-";
    stream << "+\n";
    return stream;
};


// Structure for the view frustum, so that its values can be supplied by the user to the projection matrix
class ProjectionMatrix{
    private: 
        float left_, right_, top_, bottom_, near_, far_ = 0.0f;
    
    public:
        ProjectionMatrix(float left, float right, float top, float bottom, float near, float far) : left_(left), right_(right), top_(top), bottom_(bottom), near_(near), far_(far){}

        // Default constructor
        ProjectionMatrix() = default;

        // Getters methods
        float getLeft(){return left_;}
        float getRight(){return right_;}
        float getTop(){return top_;}
        float getBottom(){return bottom_;}
        float getNear(){return near_;}
        float getFar(){return far_;}
};


// Shader interface, application of the strategy pattern for the fragment shader
template <typename target_t>
class IFragmentShader{
    public:
        virtual target_t computeShader(float x, float y, float z, float an, float bn, float cn, float u, float v) = 0;
        virtual ~IFragmentShader() {}
};

// Implementations of the strategy interface
// Each shader can have multiple implementations and each implementation is hot-swappable
// this shader in gives the first decimal of the value of z to the fragment
class SimpleFragmentShader : public IFragmentShader<char>{
    public:
        char computeShader(float x, float y, float z, float an, float bn, float cn,  float u, float v){
            /*std::cout << "x_interp: "<< x<< ", y_interp: " << y << ", z_interp: "<<z<<"\n";
            std::cout << "Stampa: " << 48 + (int)((z - floor(z))*10)<< "\n";*/
            return  48 + (int)((z - floor(z))*10);
        }
};

// Shader that produces a flat output by coloring the pixels with an 'x'
class X2DFragmentShader : public IFragmentShader<char>{
    public:
        char computeShader(float x, float y, float z, float an, float bn, float cn, float u, float v){return 'x';}
};

class SimpleIntShader : public IFragmentShader<int>{
    public:
        int computeShader(float x, float y, float z, float an, float bn, float cn, float u,  float v){return ((z - floor(z))*10);}
};

// Context of the strategy pattern, the actual pipeline
template <typename target_t, size_t C, size_t R>
class Pipeline{
    private:
        Render<target_t, C, R> video_;
        ProjectionMatrix pm_;
        IFragmentShader<target_t> *fs_;
        Render<float, C, R> z_buffer_;
        std::array<float, 3> scalars;

    public:
        // Constructor allows to set a projection matrix and the desired fragment shader
        // it also initializes the z-buffer to +infinite
        Pipeline(ProjectionMatrix pm, IFragmentShader<target_t> *fs) : pm_(pm), fs_(fs){
            for (int i = 0; i < R; i++){
                for (int j = 0; j < C; j++)
                    z_buffer_(j, i) = std::numeric_limits<float>::infinity();
            }
        }

        Pipeline(const Pipeline<target_t, C, R> & pp) = default;
        Pipeline(Pipeline<target_t, C, R> && pp) = default;
        
        // Destructor not used since no new is called
        //~Pipeline() {delete vs_;delete fs_;}

        // Setter method for the shader
        void setFragmentShader(IFragmentShader<target_t> *fs){/*delete fs_;*/ fs_ = fs;}

        // Convert a point coordinate from ndc to screen space to be printable
        size_t x_to_screen(float x){return floor(((x - pm_.getLeft()) * C / (pm_.getRight() - pm_.getLeft())));}
        size_t y_to_screen(float y){return floor(((y - pm_.getTop()) * R / (pm_.getBottom() - pm_.getTop())));}

        // Apply the perspective projection and overwrite the ndc values to the old coordinates
        void computeNdc(std::vector<Vertex>& vertices){
            float x_, y_, z_, w_;
            // Vertex& vertex = vertices[0];
            for (Vertex& vertex : vertices){
                // compute the matrix * vector multiplication
                x_ = vertex.getX() * ((2*pm_.getNear())/(pm_.getRight()-pm_.getLeft())) + (vertex.getZ() * (-(pm_.getRight()+pm_.getLeft())/(pm_.getRight()-pm_.getLeft())));
                y_ = vertex.getY() * ((2*pm_.getNear())/(pm_.getBottom()-pm_.getTop())) + vertex.getZ() * ((-(pm_.getBottom()+pm_.getTop()))/(pm_.getBottom()-pm_.getTop()));
                z_ = vertex.getZ() * ((pm_.getFar()+pm_.getNear())/(pm_.getFar()-pm_.getNear())) + ((-2*pm_.getNear()*pm_.getFar())/(pm_.getFar()-pm_.getNear()));
                w_ = vertex.getZ();
                
                // normalize for w and replace original coordinates since they aren't needed anymore
                vertex.setNdx(x_/w_);
                vertex.setNdy(y_/w_);
                // z needs to be "traslated" from (-1, 1) to (0,2) and divided by 2 to get it in (0, 1). Needed for the z interpolation 
                vertex.setNdz((z_/w_ + 1)*0.5f);
            }
        }

        // Check if a point (x, y) is inside triangle t
        bool isInside(Scene& scene, size_t triangle_index, size_t x, size_t y){
            
            // Convert coordinates (x, y) from ndc to screen
            float x1 = x_to_screen(scene(triangle_index, 0).getNdx());
            float x2 = x_to_screen(scene(triangle_index, 1).getNdx());
            float x3 = x_to_screen(scene(triangle_index, 2).getNdx());
            float y1 = y_to_screen(scene(triangle_index, 0).getNdy());
            float y2 = y_to_screen(scene(triangle_index, 1).getNdy());
            float y3 = y_to_screen(scene(triangle_index, 2).getNdy());

            // Compute the scalars of the convex combination for barycentric coordinates, of point (x, y) based on its triangle  
            // save them because we need to correct them interpolating the verteces           
            scalars[0] = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
            scalars[1] = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
            scalars[2] = 1.0f - scalars[0] - scalars[1];
            
            return (scalars[0]>=0 && scalars[0]<=1) && (scalars[1]>=0 && scalars[1]<=1 ) && (scalars[2]>=0 && scalars[2]<=1);
        }

        // Wrapper methods for print, save and clear screen
        Pipeline<target_t, C, R>& print(){
            std::cout << video_;
            return *this;
        }
        Pipeline<target_t, C, R>& fileSave(std::string filename){
            video_.fileSave(filename);
            return *this;
        }

        Pipeline<target_t, C, R>& clear(){
            video_.clear_container();
            return *this;
        }

        //Render method
        Pipeline<target_t, C, R>& render(Scene scene){
            
            float x_interp, y_interp, z_interp;

           // For each Vertex of the Scene, transform coordinates into ndc
            computeNdc(scene.getSceneVertices());
            
            // Rasterize
            for (size_t triangle_idx=0; triangle_idx < scene.getSceneTriangles().size(); triangle_idx++){
                
                //Avoid redundancy by creating 3 Vertex references to the vertices of the triangle currently looped on
                Vertex& vertex_0 = scene(triangle_idx, 0);
                Vertex& vertex_1 = scene(triangle_idx, 1);
                Vertex& vertex_2 = scene(triangle_idx, 2);
                

                // setup inside-outside test by computing the rectangle coordinates and converting them into screen mode
                size_t x_r_screen_min = x_to_screen(std::min({vertex_0.getNdx(), vertex_1.getNdx(), vertex_2.getNdx()}));
                size_t y_r_screen_min = y_to_screen(std::min({vertex_0.getNdy(), vertex_1.getNdy(), vertex_1.getNdy()}));
                size_t x_r_screen_max = x_to_screen(std::max({vertex_0.getNdx(), vertex_1.getNdx(), vertex_2.getNdx()}));
                size_t y_r_screen_max = y_to_screen(std::max({vertex_0.getNdy(), vertex_1.getNdy(), vertex_1.getNdy()}));


                // inside-outside test for each point in the rectangle
                for (size_t i = std::min({x_r_screen_min, x_r_screen_max}); i <= std::max({x_r_screen_min, x_r_screen_max}); i++){
                    for (size_t j = std::min({y_r_screen_min, y_r_screen_max}); j <= std::max({y_r_screen_min, y_r_screen_max}); j++){
                        
                        if (isInside(scene, triangle_idx, i, j)){
                            //interpolate point
                            x_interp = ( (scalars[0]/vertex_0.getNdz())*vertex_0.getNdx() +  (scalars[1]/vertex_1.getNdz())*vertex_1.getNdx() + (scalars[2]/vertex_2.getNdz())*vertex_2.getNdx()) / (scalars[0]/vertex_0.getNdz() + scalars[1]/vertex_1.getNdz() + scalars[2]/vertex_2.getNdz());
                            y_interp = ( (scalars[0]/vertex_0.getNdz())*vertex_0.getNdy() +  (scalars[1]/vertex_1.getNdz())*vertex_1.getNdy() + (scalars[2]/vertex_2.getNdz())*vertex_2.getNdy()) / (scalars[0]/vertex_0.getNdz() + scalars[1]/vertex_1.getNdz() + scalars[2]/vertex_2.getNdz());;
                            z_interp = ( (scalars[0]/vertex_0.getNdz())*vertex_0.getNdz() +  (scalars[1]/vertex_1.getNdz())*vertex_1.getNdz() + (scalars[2]/vertex_2.getNdz())*vertex_2.getNdz()) / (scalars[0]/vertex_0.getNdz() + scalars[1]/vertex_1.getNdz() + scalars[2]/vertex_2.getNdz());;
                            
                            // update z_buff and pass the interpolated vertex of the fragment to fragmentshader (it returns a target_t)
                            if (z_buffer_(i, j) > z_interp){
                                z_buffer_(i, j) = z_interp;
                                video_(i, j) = fs_->computeShader(x_interp, y_interp, z_interp, 0, 0, 0, 0, 0);  
                            }    
                        }
                    } 
                }
            }
            return *this;
        }
};