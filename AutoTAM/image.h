#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cassert>
#include <string>
#include <iostream>
#include <limits>

// ====================================================================
// 24 bit color
class Color {
public:
  Color(int r_=255, int g_=255, int b_=255) : r(r_),g(g_),b(b_) {}
  bool isWhite() const { return r==255 && g==255 && b==255; }
  bool isBlack() const { return r==0 && g==0 && b==0; }
  int r,g,b;
};

// ====================================================================
// 24 bit color
class DistancePixel {
 public:
  DistancePixel() : x(-1),y(-1),distance(std::numeric_limits<double>::max()) {}
  // accessor
  int getX() const { return x; }
  int getY() const { return y; }
  double getValue() const { return distance; }
  // modifier
  void setX(int _x) { x = _x; }
  void setY(int _y) { y = _y; }
  void setValue(double v) { distance = v; }
 private:
  // representation
  int x;
  int y;
  double distance;
};

inline std::ostream& operator<< (std::ostream &ostr, const DistancePixel &a) {
  ostr << a.getX() << " " << a.getY() << " " << a.getValue();
  return ostr;
}

inline bool operator<(const DistancePixel &a, const DistancePixel &b) {
  return a.getValue() < b.getValue();
}
inline bool operator>(const DistancePixel &a, const DistancePixel &b) {
  return a.getValue() > b.getValue();
}
inline bool operator<=(const DistancePixel &a, const DistancePixel &b) {
  return a.getValue() <= b.getValue();
}
inline bool operator>=(const DistancePixel &a, const DistancePixel &b) {
  return a.getValue() >= b.getValue();
}

// ====================================================================
// ====================================================================
// TEMPLATED IMAGE CLASS
//    can be saved and loaded from standard file formats:
//      .ppm    (when T == Color)
//      .pbm    (when T == bool)
//

template <class T>
class Image {
public:
  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Image() : width(0), height(0), data(NULL), gl_data(NULL) {}
  void Allocate(int w, int h) {
    width = w;
    height = h;
    delete [] data;
    delete [] gl_data;
    gl_data = NULL;
    if (width == 0 && height == 0) {
      data = NULL;
    } else {
      assert (width > 0 && height > 0);
      data = new T[width*height]; 
    }
  }
  ~Image() {
    delete [] data; 
    delete [] gl_data;
  }

  Image(const Image &image) { 
    data=NULL;
    gl_data=NULL;
    copy_helper(image); }
  const Image& operator=(const Image &image) { 
    if (this != &image)
      copy_helper(image);
    return *this; }

  void copy_helper(const Image &image) {
    Allocate (image.Width(), image.Height());
    for (int i = 0; i < image.Width(); i++) {
      for (int j = 0; j < image.Height(); j++) {
        this->SetPixel(i,j,image.GetPixel(i,j));
      }
    }
  }

  // =========
  // ACCESSORS
  int Width() const { return width; }
  int Height() const { return height; }
  const T& GetPixel(int x, int y) const {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    return data[y*width + x]; }
  T& GetPixel(int x, int y) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    return data[y*width + x]; }
  // for use with OpenGL (not const because it builds gl_data first)
  unsigned char* getGLPixelData();

  // =========
  // MODIFIERS
  void SetAllPixels(const T &value) {
    for (int i = 0; i < width*height; i++) {
      data[i] = value; } }
  void SetPixel(int x, int y, const T &value) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    data[y*width + x] = value; }

  // ===========
  // LOAD & SAVE
  bool Load(const std::string &filename);
  bool Save(const std::string &filename) const; 
  
private:
  // ==============
  // REPRESENTATION
  int width;
  int height;
  T *data;
  unsigned char* gl_data;
};

#endif
