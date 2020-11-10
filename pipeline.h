#include "shader.h"


// Context of the strategy pattern, the actual pipeline
// The pipeline contains the computations and data needed to represent an object in a 2D space, such as the screen or a file
template <typename target_t, size_t C, size_t R>
class Pipeline{
    private:
        Render<target_t, C, R> video_;
        ProjectionMatrix pm_;
        IFragmentShader<target_t> *fs_;
        Render<double, C, R> z_buffer_;
        std::array<double, 3> scalars;

        // Sets the z_buffer container to infinity values
        inline void clear_z_buffer_(){
            for (int i = 0; i < R; i++){
                for (int j = 0; j < C; j++)
                    z_buffer_(j, i) = std::numeric_limits<double>::infinity();
            }
        }

        // Convert a point coordinate from ndc to screen space to be printable
        inline size_t x_to_screen(double x){return floor(((x - pm_.getLeft()) * C / (pm_.getRight() - pm_.getLeft())));}
        inline size_t y_to_screen(double y){return floor(((y - pm_.getTop()) * R / (pm_.getBottom() - pm_.getTop())));}

        // Apply the perspective projection and overwrite the ndc values to the old coordinates
        void computeNdc(std::vector<Vertex>& vertices){
            double x_, y_, z_, w_;
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

        inline double area(double x1, double y1, double x2, double y2, double x3, double y3) { 
            return std::abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0); 
        } 

        // Check if a point (x, y) is inside triangle t
        // also compute and store the barycentric coefficients, needed for the vertices interpolation later
        bool isInside(double x, double y, double x1, double x2, double x3, double y1, double y2, double y3){

            /* Calculate area of triangle ABC */
            double A = area (x1, y1, x2, y2, x3, y3); 
            
            /* Calculate area of triangle PBC */   
            double A1 = area (x, y, x2, y2, x3, y3); 
            
            /* Calculate area of triangle PAC */   
            double A2 = area (x1, y1, x, y, x3, y3); 
            
            /* Calculate area of triangle PAB */    
            double A3 = area (x1, y1, x2, y2, x, y); 
                
            /* Check if sum of A1, A2 and A3 is same as A */ 
            return (A == A1 + A2 + A3); 
        }


    public:

        // Constructor allows to set a projection matrix and the desired fragment shader
        // it also initializes the z-buffer to +infinite
        Pipeline(ProjectionMatrix pm, IFragmentShader<target_t> *fs) : pm_(pm), fs_(fs){
            this->clear_z_buffer_();
        }

        Pipeline(const Pipeline<target_t, C, R> & pp) = default;
        Pipeline(Pipeline<target_t, C, R> && pp) = default;
        
        // Destructor not used since no new (keyword) is called
        //~Pipeline() {delete vs_;delete fs_;}

        // Setter method for the shader
        void setFragmentShader(IFragmentShader<target_t> *fs){/*delete fs_;*/ fs_ = fs;}

        // The render method contains the step execution needed to do the drawing of the object
        Pipeline<target_t, C, R>& render(Scene scene){
            video_.clear_container();
            clear_z_buffer_();
            double x_interp = 0, y_interp=0, z_interp;

           // For each Vertex of the Scene, transform coordinates into ndc
            computeNdc(scene.getSceneVertices());
            
            // Rasterize
            for (size_t triangle_idx=0; triangle_idx < scene.getSceneTriangles().size(); triangle_idx++){
                
                // Avoid redundancy by creating variables that will be used massively later
                double x1 = x_to_screen(scene(triangle_idx, 0).getNdx());
                double x2 = x_to_screen(scene(triangle_idx, 1).getNdx());
                double x3 = x_to_screen(scene(triangle_idx, 2).getNdx());
                double y1 = y_to_screen(scene(triangle_idx, 0).getNdy());
                double y2 = y_to_screen(scene(triangle_idx, 1).getNdy());
                double y3 = y_to_screen(scene(triangle_idx, 2).getNdy());
                double z1 = scene(triangle_idx, 0).getNdz();
                double z2 = scene(triangle_idx, 1).getNdz();
                double z3 = scene(triangle_idx, 2).getNdz();

                // setup inside-outside test by computing the rectangle coordinates and converting them into screen mode
                size_t x_r_screen_min = std::min({x1, x2, x3});
                size_t x_r_screen_max = std::max({x1, x2, x3});
                size_t y_r_screen_min = std::min({y1, y2, y3});
                size_t y_r_screen_max = std::max({y1, y2, y3});


                // inside-outside test for each point in the rectangle
                for (size_t x = x_r_screen_min; x <= x_r_screen_max; x++){
                    for (size_t y = y_r_screen_min; y <= y_r_screen_max; y++){
                        
                        if (isInside(x, y, x1, x2, x3, y1, y2, y3)){

                            // Compute the scalars of the convex combination for barycentric coordinates, of point (x, y) based on its triangle  
                            // save them because we need to correct them interpolating the verteces           
                            scalars[0] = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
                            scalars[1] = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
                            scalars[2] = 1.0f - scalars[0] - scalars[1];

                            //interpolate point
                            x_interp = ( (scalars[0]/z1)*scene(triangle_idx, 0).getNdx() +  (scalars[1]/z2)*scene(triangle_idx, 1).getNdx() + (scalars[2]/z3)*scene(triangle_idx, 2).getNdx()) / (scalars[0]/z1 + scalars[1]/z2 + scalars[2]/z3);
                            y_interp = ( (scalars[0]/z1)*scene(triangle_idx, 0).getNdy() +  (scalars[1]/z2)*scene(triangle_idx, 1).getNdy() + (scalars[2]/z3)*scene(triangle_idx, 2).getNdy()) / (scalars[0]/z1 + scalars[1]/z2 + scalars[2]/z3);;
                            z_interp = ( (scalars[0]/z1)*z1 +  (scalars[1]/z2)*z2 + (scalars[2]/z3)*z3) / (scalars[0]/z1 + scalars[1]/z2 + scalars[2]/z3);;

                            // update z_buff and pass the interpolated vertex of the fragment to fragmentshader (it returns a target_t)
                            if (z_buffer_(x, y) > z_interp){
                                z_buffer_(x, y) = z_interp;
                                video_(x, y) = fs_->computeShader(x_interp, y_interp, z_interp, 0, 0, 0, 0, 0);  
                            }    
                        }
                    } 
                }
            }
            return *this;
        }

        //Getter for Render, useful when the current rendered image has to be saved in an external Render object
        Render<target_t, C, R>& getRender(){
            return video_;
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

};