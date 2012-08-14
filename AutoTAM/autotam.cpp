#include <iostream>
#include <vector>
#include <ctime>
#include "image.h"
#include "MersenneTwister.h"
#include <cstdlib>
#include <string>
using namespace std;

MTRand gRand;

int resmax = 1024;
//int resmax = 256;
int resmin = 16;
int images = 0;

class Line {
public:
	Line(): contribution(0.) {}
	void genHorizontal() {
		static int quad = 0;
		length = gRand();
		length = fmodf(length,.6);
		length += .3;
		y = gRand();
		x = gRand();
		turby = 0.;
		turbx = gRand();
		turbx = fmodf(turbx,.03);
	}
	void genVertical() {
		length = gRand();
		length = fmodf(length,.6);
		length += .3;
		x = gRand();
		y = gRand();
		turbx = 0.;
		turby = gRand();
		turby = fmodf(turby,.03);
	}
	float length;
	float x, y; float turbx, turby;
	float contribution;
};

struct layer {
	Image<Color> image;
	vector<Line> lines;
};
/*float SqDistPointSegment(float x, float y, float sx, float sy, float tx, float ty) {
	float abx = tx-sx, aby = ty-sy, acx = x-sx, acy = y-sy, bcx = x-tx, bcy = x-ty;
	float e = acx*abx + acy*aby;
	if (e<0.0f) return acx*acx+acy*acy;
	float f = abx*abx+aby*aby;
	if (e<f) return bcx*bcx+bcy*bcy;
	return acx*acx+acy*acy - e*e/f;
*/
class Vec2f {
public:
	Vec2f() {}
	Vec2f(float x, float y) {
		data[0] = x;
		data[1] = y;
	}
	Vec2f(const Vec2f &o) {
		data[0] = o.data[0];
		data[1] = o.data[1];
	}
	Vec2f &operator+=(const Vec2f &o) {
		data[0] += o.data[0];
		data[1] += o.data[1];
		return *this;
	}
	Vec2f &operator-=(const Vec2f &o) {
		data[0] -= o.data[0];
		data[1] -= o.data[1];
		return *this;
	}
	Vec2f &operator*=(const Vec2f &o) {
		data[0] *= o.data[0];
		data[1] *= o.data[1];
		return *this;
	}
	Vec2f &operator/=(const Vec2f &o) {
		data[0] /= o.data[0];
		data[1] /= o.data[1];
		return *this;
	}
	Vec2f &operator+=(float o) {
		data[0] += o;
		data[1] += o;
		return *this;
	}
	Vec2f &operator-=(float o) {
		data[0] -= o;
		data[1] -= o;
		return *this;
	}
	Vec2f &operator*=(float o) {
		data[0] *= o;
		data[1] *= o;
		return *this;
	}
	Vec2f &operator/=(float o) {
		data[0] /= o;
		data[1] /= o;
		return *this;
	}
	Vec2f operator+(const Vec2f &o) const {
		return Vec2f(*this)+=o;
	}
	Vec2f operator-(const Vec2f &o) const {
		return Vec2f(*this)-=o;
	}
	Vec2f operator*(const Vec2f &o) const {
		return Vec2f(*this)*=o;
	}
	Vec2f operator/(const Vec2f &o) const {
		return Vec2f(*this)/=o;
	}
	Vec2f operator+(float o) const {
		return Vec2f(*this)+=o;
	}
	Vec2f operator-(float o) const {
		return Vec2f(*this)-=o;
	}
	Vec2f operator*(float o) const {
		return Vec2f(*this)*=o;
	}
	Vec2f operator/(float o) const {
		return Vec2f(*this)/=o;
	}
	friend Vec2f operator+(float o, const Vec2f &d) {
		return d + o;
	}
	friend Vec2f operator-(float o, const Vec2f &d) {
		return d - o;
	}
	friend Vec2f operator*(float o, const Vec2f &d) {
		return d * o;
	}
	friend Vec2f operator/(float o, const Vec2f &d) {
		return d / o;
	}
	float dot(const Vec2f &o) const {
		return data[0]*o.data[0]+data[1]*o.data[1];
	}
	float length() const {
		return sqrt(dot(*this));
	}
	float operator[](int i) {
		if (i<0 || i>1) exit(0);
		return data[i];
	}
private:
	float data[2];
};

void DistT(const Vec2f &c, const Vec2f &a, const Vec2f &b, float &t, float &d) {
	Vec2f ab = b-a;
	t = ab.dot(c-a) / ab.dot(ab);
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	ab = a + t * ab;
	d = (c-ab).length();
}

// HELPER FOR TAM GENERATION
double localAvg(const Image<Color> &img, Line &line) {
	double r=0.,g=0.,b=0.;
    //go a quarter of the line length in all directions to make a rectangle around the line
    //unsigned int box = line.length / 4.;
    unsigned int box = (line.length * img.Width())/ 4.;
    std::cout << __LINE__ << " box " << box << std::endl;
    double xmin, xmax, ymin, ymax;

    xmin = (line.x - box) % img.Width();
    xmax = (line.x + line.length + box) % img.Width();
    ymin = (line.y - box) % img.Height();
    ymax = (line.y + box) % img.Height();

	for (int i = xmin; i < xmax; i++) {
		for (int j = ymin; j < ymax; j++) {
			Color temp = img.GetPixel(i,j);
			r += 255-temp.r;
			g += 255-temp.g;
			b += 255-temp.b;
		}
	}
	double area = (xmax - xmin) * (ymax - ymin);
    std::cout << __LINE__ << " local average " << r/area+g/area+b/area << std::endl;
	return r/area+g/area+b/area;
}  

// HELPER FOR TAM GENERATION
double averagePixels(const Image<Color> &img) {
	double r=0.,g=0.,b=0.;
	for (int i = 0; i < img.Width(); i++) {
		for (int j = 0; j < img.Height(); j++) {
			Color temp = img.GetPixel(i,j);
			r += 255-temp.r;
			g += 255-temp.g;
			b += 255-temp.b;
		}
	}
	double area = img.Width() * img.Height();
	return r/area+g/area+b/area;
}

void drawLine(Image<Color> &image, const Line &line, const bool vertical = 0) {
	Vec2f start = Vec2f(image.Width() * line.x, image.Height() * line.y);
	Vec2f target;
	if (vertical) {
		target = Vec2f(start[0], start[1]+line.length*(float)image.Height());
	} else {
		target = Vec2f(start[0]+line.length*(float)image.Width(), start[1]);
	}
	Color fg(0,0,0);
	for (int i = start[0]-3; i<target[0]+3; ++i) {
		for (int j = start[1]-3; j<target[1]+3; ++j) {
			float dist = 0.0f, t=0.0f;
			DistT(Vec2f(i,j), start, target, t, dist);
			//cout << dist << endl;
			if (dist < 2. * (1. - t + .1)) {
				//cout << i%image.Width() << "  " << j%image.Height() << endl;
				float tmp = 2. * (1. - t + .1) - 1.;
				if (dist > tmp) {
					//cout << __LINE__ << "\tX: " << abs(i%image.Width()) << "\tY: " << abs(j%image.Height()) << endl;
					Color bg = image.GetPixel(abs(i%image.Width()),abs(j%image.Height()));
					tmp = dist-tmp;
					int r = (int)(tmp*(float)bg.r+(1-tmp)*(float)fg.r);
					int g = (int)(tmp*(float)bg.g+(1-tmp)*(float)fg.g);
					int b = (int)(tmp*(float)bg.b+(1-tmp)*(float)fg.b);
					image.SetPixel(abs(i%image.Width()),abs(j%image.Height()),Color(r,g,b));
				} else {
					image.SetPixel(abs(i%image.Width()),abs(j%image.Height()),fg);
				}
			}
		}
	}
}

/* direction flag:
 * 0 means horizontal
 * 1 means vertical
 * 2 means alternating
 */
#define num_boxes 100
#define box_size 1./num_boxes
void GoGoPowerRangers(layer &l, double desired_density, const unsigned int direction_flag = 0) {
	Image<Color> &tmp = l.image;
	static int strat_quad_x1 = 0, strat_quad_x2 = 0, strat_quad_y1 = 0, strat_quad_y2 = 0;
    //double num_boxes = 100.0;
    //double box_size = 1/num_boxes;
	
	double cur_density = averagePixels(tmp);
	//double cur_density;
    //{ 
        //Line line;
        //if (direction_flag) {
            //line.genVertical();
        //} else {
            //line.genHorizontal();
        //}
        //cur_density  = localAvg(tmp, line);
    //}
	int counter = 0;
	//while (cur_density < desired_density && counter < 1000) {
	while (cur_density < desired_density) {
		cout << __LINE__<< "\tcounter: " << counter++  << "\tdensity: " << cur_density << endl;
		Image<Color> best = Image<Color>(tmp);
		double cur_best = averagePixels(best);
		double cur_local_best; 
        {
            Line line;
            if (direction_flag) {
                line.genVertical();
            } else {
                line.genHorizontal();
            }

            cur_local_best = localAvg(best, line);
        }
		double cur_fitness = 0;
		Line best_line;
		for (int i = 0; i < 100; i++) {
			Image<Color> test_img = Image<Color>(tmp);
			Line line;
			bool direction = 0;

			
			if (direction_flag == 2) {
				direction = l.lines.size() % 2;
			} else {
				direction = direction_flag;
			}
			
			if (direction) {
				line.genHorizontal();
				//line.x *= (box_size * ((strat_quad_2 % num_boxes) + 1));
				line.y += (box_size * ((strat_quad_y1 % num_boxes) + 1));
				line.x += (box_size * ((strat_quad_x1 % num_boxes) + 1));
                //line.y *= box_size;
                //float tempx = line.x;
                //std::cout << "Line:\t" << line.x << " " << line.y << std::endl;
                //line.y += ((strat_quad_y1 % num_boxes) + 1);
				//line.y *= box_size;
                std::cout << "Modified Line:\t" << line.x << " " << line.y << std::endl;
                //line.x += (strat_quad % num_boxes) + 1;
				//line.y = ((line.y * box_size) + ((strat_quad % num_boxes) + 1));
				//strat_quad++; strat_quad %= num_boxes;
				strat_quad_y1++; strat_quad_y1 %= num_boxes;
				if (strat_quad_y1 == 0) strat_quad_x1++; strat_quad_x1 %= num_boxes;
			} else {
				line.genVertical();
			    line.x += (box_size * ((strat_quad_x2 % num_boxes) + 1));
			    line.y += (box_size * ((strat_quad_y2 % num_boxes) + 1));
                //line.x *= box_size;
                //float tempy = line.y;
                //line.x += ((strat_quad_x2 % num_boxes) + 1);
				//line.x *= box_size;
				strat_quad_x2++; strat_quad_x2 %= num_boxes;
				if (strat_quad_x2 == 0) strat_quad_y2++; strat_quad_y2 %= num_boxes;
                //line.y = (strat_quad % num_boxes) + 1;
				//line.x = ((line.x * box_size) + ((strat_quad_2 % num_boxes) + 1));
				//line.y *= (box_size * ((strat_quad % num_boxes) + 1));
				//strat_quad_2++; strat_quad_2 %= num_boxes;
			}
			
			//line.genVertical();
			double test_density;
			double test_local_density;
		
			drawLine(test_img,line, direction);
			test_density = averagePixels(test_img);
			test_local_density = localAvg(test_img, line);
			
			double test_fitness = (test_local_density - cur_local_best) * line.length;
			
			if (test_fitness >= cur_fitness) {
				best_line = line;
				best = test_img;
				cur_fitness = test_fitness;
				cur_best = test_density;
				cur_local_best = test_local_density;
			}
		}
		l.lines.push_back(best_line);
		cur_density = cur_best;
		tmp = best;
		//std::cout << __LINE__ << "\tcounter: " << counter << "\tnum_lines: " << l.lines.size() << std::endl;
	}
}

void DrawBatchOfLines(Image<Color> &image, const vector<Line> &lines, const unsigned int direction_flag = 0) {
	bool direction;
	
	if (direction_flag == 1) {
		direction = 1;
	} else {
		direction = 0;
	}
	
	for (int i=0; i<lines.size(); ++i) {
		if (direction_flag == 2) {
			direction = i%2;
		}
		drawLine(image,lines[i], direction);
	}
}

int main() {
	// init
	{
		gRand = MTRand(time(0));
		int res = resmax;
		while (res > resmin) {
			images++;
			res/=2;
		}
	}
	// generate mipmaps
	layer **layers = new layer*[6];
	for (int i=0; i<6; ++i) {
		layers[i] = new layer[images];
	}
	
	// init the first column
	for (int i=0, res=resmin; i<images; ++i,res*=2) {
		layers[0][i].image.Allocate(res,res);
		layers[0][i].image.SetAllPixels(Color(255,255,255));
	}
	double densities[] = {200,350,450,600,675,730};
	GoGoPowerRangers(layers[0][0],100);
	for (int i=1; i<images; ++i) {
		layers[0][i].lines = layers[0][i-1].lines;
		DrawBatchOfLines(layers[0][i].image, layers[0][i].lines);
		GoGoPowerRangers(layers[0][i],100);
	}
	for (int j=1, flag=0; j<6; ++j, flag+=(j==3 || j==5)) {
		layers[j][0].image = layers[j-1][0].image;
		GoGoPowerRangers(layers[j][0],densities[j]);
		for (int i=1; i<images; ++i) {
			layers[j][i].image = layers[j-1][i].image;
			layers[j][i].lines = layers[j][i-1].lines;
			DrawBatchOfLines(layers[j][i].image, layers[j][i].lines,flag);
			GoGoPowerRangers(layers[j][i],densities[j],flag);
		}
	}
	for (int j=0; j<6; ++j) {
		for (int i=0; i<images; ++i) {
			char s1[] = "0", s2[]="0";
			s1[0]+=j; s2[0]+=i;
			layers[j][i].image.Save(string("output/")+s1+"_"+s2+".ppm");
		}
	}
	//Image<Color> tmp; tmp.Allocate(128,128); tmp.SetAllPixels(Color(255,255,255));
	//layer tmplayer; tmplayer.image.Allocate(256,256); tmp.SetAllPixels(Color(255,255,255));
	//GoGoPowerRangers(tmplayer,150.);
	//tmplayer.image.Save("test.ppm");
	return 0;
}
