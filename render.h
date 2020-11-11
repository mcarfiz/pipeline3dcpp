/*  
Giacomo Arrigo 860022
Marco Carfizzi 860149
*/

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
    // Makes the container "blank"
        inline void clear_container(){
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

        //Copy assignment
        void operator = (const Render<target_t, C, R> &t){
            container_ = t.container_;
        }

        // Move assignment
        void operator = (Render<target_t, C, R> &&t){
            container_ = std::move(t.container_);
        }

        // Getter methods
        size_t getWidth()   {return C;}
        size_t getHeight()  {return R;}
        std::array<target_t, C * R>& getTarget()    {return container_;}

        // Overloading of () operator, this allows for direct access to location (x, y) of the target
        // hiding the fact that the matrix is in fact represented in one dimension
        target_t& operator()(const size_t y, const size_t x){
            if (x >= R || y >= C)
                throw std::out_of_range("Target indices out of range");
            return container_[C*x + y];
        }

        // Setting up the overload of the << operator for the print 
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
            std::cout << "File " << filename << ".dat correctly saved in current directory.\n";
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