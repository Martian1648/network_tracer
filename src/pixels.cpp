#include "pixels.h"
#include "lodepng.h"
#include <fstream>
#include <cmath>
#include "color.h"
#include <iostream>

Pixels::Pixels(int columns, int rows)
  :columns{columns}, rows{rows}, values(columns*rows) {}

const Color& Pixels::operator()(int row, int col) const {
    // implement me
    return values[row * columns + col];
}
Color& Pixels::operator()(int row, int col) {
  // implement me
    return values[row * columns + col];
}

void Pixels::save_ppm(const std::string& filename) {
  // use to_color<unsigned>(color.x) for printing RGB values to file
  // write the pixel values to a file using the NetBPM ppm image format
}

void Pixels::save_png(const std::string& filename) {
  // lodepng expects pixels to be in a vector of unsigned char, where
  // the elements are R, G, B, alpha, R, G, B, alpha, etc.  Use
  // to_color<unsigned char>(color.x) for putting colors in this
  // vector, and lodepng::encode(filename, vector, width, height)
  // (make sure you check the error code returned by this function
  // using lodepng_error_text(unsigned error)).
  std::ofstream output(filename);
  if(!output) {
      throw std::runtime_error("Cannot open output file "+ filename);
  }
  std::vector<unsigned char> data;
  for(auto c :values){
      data.push_back(to_color<unsigned char>(c.x));
      data.push_back(to_color<unsigned char>(c.y));
      data.push_back(to_color<unsigned char>(c.z));
      data.push_back(255);
  }
  unsigned error = lodepng::encode(filename, data, columns, rows );
  if (error){
      throw std::runtime_error(lodepng_error_text(error));
  }
}

std::vector<unsigned char> Pixels::get_bytes() {
    std::vector<unsigned char> data;
    for(auto c :values){
        data.push_back(to_color<unsigned char>(c.x));
        data.push_back(to_color<unsigned char>(c.y));
        data.push_back(to_color<unsigned char>(c.z));
        data.push_back(255);
    }
    return data;
}

double gamma_correction(double value) {
  double gamma = 2.2;
  //double gamma = 1.0;
  return std::pow(value, 1/gamma);
}

Pixels::Pixels(const std::string &filename) {
    std::vector<unsigned char> data;
    unsigned width, height;
    unsigned error = lodepng::decode(data, width, height, filename);
    if (error) {
        throw std::runtime_error(lodepng_error_text(error));
    }

    rows = height;
    columns = width;

    for (int i = 0; i < rows*columns; ++i) {
        int n = 4 * i;
        double r = static_cast<double>(data.at(n))/255;
        double g = static_cast<double>(data.at(n + 1))/255;
        double b = static_cast<double>(data.at(n + 2))/255;
        values.push_back({r, g, b});
    }
}


