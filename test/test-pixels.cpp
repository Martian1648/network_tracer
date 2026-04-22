#include "pixels.h"
#include "color.h"


// write a function to set all the pixels to the same color
void set_color(Pixels& pixels, Color color) {
    for(int i = 0; i < pixels.columns; ++i){
        for(int j = 0; j < pixels.rows; j++){
            pixels(j, i) = color;
        }
    }
}

// write a function that produces a single color gradient
void gradient(Pixels& pixels) {
    for (int i = 0; i < pixels.rows; ++i) {
        for (int j = 0; j < pixels.columns; ++j) {
            double intensity = static_cast<double>(j) / (pixels.columns - 1);
            pixels(i, j) = Color(intensity, intensity, intensity);
        }
    }
}

struct block{
    block(int cols, int rows, Color color)
    :cols{cols}, rows{rows}, color{color}{

    }
    int cols;
    int rows;
    Color color;
};

// write a function to color a pretty picture!
void pretty(Pixels& pixels) {
    std::vector<Color> colors = {
            Red, Red_Orange, Orange, Deep_Orange, Yellow_Orange,
            Yellow, Golden_Yellow, Yellow_Green, Chartreuse, Light_Green,
            Green, Spring_Green, Bright_Aqua, Light_Turquoise, Deep_Cyan,
            Cyan, Soft_Cyan, Sky_Blue, Deep_Sky_Blue, True_Blue,
            Blue, Blue_Violet, Soft_Purple, Deep_Purple, Violet,
            Magenta, Pinkish_Magenta, Hot_Pink, Deep_Pink, Scarlet
    };

    std::vector<Color> greyscale ={
            Black, White, Gray, Lightgray
    };
    std::vector<block> blocks = {};
    for(Color color : colors){
       blocks.emplace_back(256, 120, color);
    }
    int index = 0;

    for (int i = 0; i < pixels.rows; i++) {

            for (int j = 0; j < pixels.columns; ++j) {
                int block_col = j / blocks[0].cols;
                int block_row = i / blocks[0].rows;
                index = block_row * (pixels.columns / blocks[0].cols) + block_col;
                pixels(i, j) = blocks[index].color;
            }


    }
}


int main() {
  const unsigned columns = 1280;
  const unsigned rows = 720;
  Pixels pixels{columns, rows};


  // example:
  /*set_color(pixels, White);
  std::string filename{"white.png"};
  pixels.save_png(filename);
  std::cout << "Wrote: " << filename << '\n';*/

    /*gradient(pixels);
    std::string filename2{"gradient.png"};
    pixels.save_png(filename2);
    std::cout<<"Wrote: "<< filename2<<'\n';*/

    pretty(pixels);
    std::string filename3{"pretty.png"};
    pixels.save_png(filename3);
    std::cout<<"Wrote: "<< filename3<<'\n';
}
