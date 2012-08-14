#include <cstring>
#include <cstdio>
#include "image.h"

// ====================================================================
// EXPLICIT SPECIALIZATIONS for Color images (.ppm)
// ====================================================================
template <>
bool Image<Color>::Save(const std::string &filename) const {
  int len = filename.length();
  if (!(len > 4 && filename.substr(len-4) == std::string(".ppm"))) {
    std::cerr << "ERROR: This is not a PPM filename: " << filename << std::endl;
    return false;
  }
  FILE *file = fopen(filename.c_str(), "wb");
  if (file == NULL) {
    std::cerr << "Unable to open " << filename << " for writing\n";
    return false;
  }

  // misc header information
  fprintf (file, "P6\n");
  fprintf (file, "%d %d\n", width,height);
  fprintf (file, "255\n");

  // the data
  // flip y so that (0,0) is bottom left corner
  for (int y = height-1; y >= 0; y--) {
    for (int x=0; x<width; x++) {
      Color v = GetPixel(x,y);
      fputc ((unsigned char)(v.r),file);
      fputc ((unsigned char)(v.g),file);
      fputc ((unsigned char)(v.b),file);
    }
  }
  fclose(file);
  return true;
}

template <>
bool Image<Color>::Load(const std::string &filename) {
  int len = filename.length();
  if (!(len > 4 && filename.substr(len-4) == std::string(".ppm"))) {
    std::cerr << "ERROR: This is not a PPM filename: " << filename << std::endl;
    return false;
  }
  FILE *file = fopen(filename.c_str(),"rb");
  if (file == NULL) {
    std::cerr << "Unable to open " << filename << " for reading\n";
    return false;
  }

  // misc header information
  char tmp[100];
  fgets(tmp,100,file); 
  assert (strstr(tmp,"P6"));
  fgets(tmp,100,file); 
  while (tmp[0] == '#') { fgets(tmp,100,file); }
  sscanf(tmp,"%d %d",&width,&height);
  fgets(tmp,100,file); 
  assert (strstr(tmp,"255"));

  // the data
  delete [] data;
  data = new Color[height*width];
  // flip y so that (0,0) is bottom left corner
  for (int y = height-1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      Color c;
      c.r = fgetc(file);
      c.g = fgetc(file);
      c.b = fgetc(file);
      SetPixel(x,y,c);
    }
  }
  fclose(file);
  return true;
}

template <>
unsigned char* Image<Color>::getGLPixelData() {
  delete [] gl_data;
  gl_data = new unsigned char[width*height*3];
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) { 
      Color v = GetPixel(x,y);
      gl_data[y*width*3+x*3+0] = v.r;
      gl_data[y*width*3+x*3+1] = v.g;
      gl_data[y*width*3+x*3+2] = v.b;
    }
  }
  return gl_data;
}

// ====================================================================
// EXPLICIT SPECIALIZATIONS for bool images (.pbm)
// ====================================================================
template <>
bool Image<bool>::Save(const std::string &filename) const {
  int len = filename.length();
  if (!(len > 4 && filename.substr(len-4) == std::string(".pbm"))) {
    std::cerr << "ERROR: This is not a PBM filename: " << filename << std::endl;
    return false;
  }
  FILE *file = fopen(filename.c_str(), "wb");
  if (file == NULL) {
    std::cerr << "Unable to open " << filename << " for writing\n";
    return false;
  }

  // write the header information
  char tmp[20];
  tmp[0] = 'P';
  tmp[1] = '4';
  tmp[2] = 10;
  fwrite(tmp, sizeof(char), 3, file);
  sprintf(tmp, "%d %d", width, height);
  fwrite(tmp, strlen(tmp), 1, file);
  tmp[0] = 10;
  fwrite(tmp, sizeof(char), 1, file);
  
  // size of row in bytes
  int rowsize = (width + 7) / 8; // the size of each row
  unsigned char *packedData = new unsigned char[rowsize]; // row of packed bytes to write
  // Write the image row by row
  for (int i = 0; i < height; ++i) {
    // pack a row of pixels
    for(int k = 0; k < rowsize; ++k){ // for each byte in line to write, 
      unsigned char packed_d = 0; // initialize a packed byte to 0
      for(int j = 0; j < 8; ++j){ 
        // special case when not enough bits to fill last byte
        if( (k*8+j) == width){
          packed_d <<= ( 8-j); 
          break;
        }
        packed_d <<= 1; // shift left one
        if(GetPixel(k*8+j,height-1-i))
          packed_d = (packed_d | 1); 		   
        else 
          packed_d = (packed_d | 0); 
      }
      packedData[k] = packed_d;
    }
    fwrite((void *)packedData, sizeof(unsigned char), rowsize, file);     		
  }
  
  fclose(file);
  delete [] packedData;
  return true;
}

template <>
bool Image<bool>::Load(const std::string &filename) {

  int len = filename.length();
  if (!(len > 4 && filename.substr(len-4) == std::string(".pbm"))) {
    std::cerr << "ERROR: This is not a PBM filename: " << filename << std::endl;
    return false;
  }
  FILE *file = fopen(filename.c_str(), "rb");
  if (file == NULL) {
    std::cerr << "Unable to open " << filename << " for reading\n";
    return false;
  }

  char buffer[100];
  // read file identifier (magic number)
  fgets (buffer, sizeof (buffer), file);
  if ((buffer[0] != 'P') || (buffer[1] != '4'))
    {
      std::cerr << "Not a simple pbm file\n";
      return false;
    }

  // read image size
  do {
    fgets (buffer, sizeof (buffer), file);  
    // Skip all comments "#"
    // Also skip any newlines that might (but shouldn't) be there 
  } while (buffer[0] == '#' || buffer[0] == 10);
  
  // Read the width and height of the  image
  sscanf (buffer, "%d %d", &width, &height);

  // Allocate the buffer
  if (data)
    delete [] data; // don't leak!!
  
  data = new bool[ width * height];  // 1 bool per pixel
  
  // Read in the pixel array row-by-row
  // each row is width bits, packed 8 to a byte
  int rowsize = (width + 7) / 8; // the size of each row in bytes
  unsigned char *packedData = new unsigned char[rowsize]; // array of row bits to unpack
  
  // for each line of the image
  for (int i = 0; i < height; ++i) {
    // read a row from the file of packed data
    fread(packedData, sizeof(char), rowsize, file);
    for(int k = 0; k < rowsize; ++k){ // for each byte in the row 
      unsigned char packed_d = packedData[k]; // temporary char of packed bits
      for(int j = 0; j < 8; ++j){ 	

        // special case last byte, might not be enough bits to fill			
        if( (k*8+j) == width){
          break;
        }
        
        // an ugly one linear that extracts j'th bit as a new byte
        unsigned char temp = (packed_d >> (7-j)) & 1;
        
        // in a .pbm file, 1 == true == black
        SetPixel(k*8+j,height-1-i, (temp == 1));
      }
    }		
  }
  
  // close the file
  fclose(file);
  delete [] packedData;
  return true;
}

template <>
unsigned char* Image<bool>::getGLPixelData() {
  delete [] gl_data;
  gl_data = new unsigned char[width*height*3];
  for (int x=0; x<width; x++) {
    for (int y=0; y<height; y++) { 
      bool v = GetPixel(x,y);
      // in a .pbm file, 1 == true == black
      gl_data[y*width*3+x*3+0] = (v) ? 0 : 255;
      gl_data[y*width*3+x*3+1] = (v) ? 0 : 255;
      gl_data[y*width*3+x*3+2] = (v) ? 0 : 255;
    }
  }
  return gl_data;
}

// ====================================================================
// ====================================================================

