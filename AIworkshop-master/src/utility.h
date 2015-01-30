#ifndef UTILITY_H
#define UTILITY_H

//keep this file short. Will be included often

//some typedefs for easer port
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;


#define PI 3.14159265358979
class Vector2d {
public:
	float x,y;
	Vector2d GetNormalized();
	float Len();
	float Dot(const Vector2d& v);
	Vector2d();
	Vector2d(float X, float Y);
	Vector2d operator+(const Vector2d& v);
	Vector2d operator-(const Vector2d& v);
	Vector2d& operator=(const Vector2d& v);
	Vector2d operator*(const float scale);
	Vector2d operator*(const int scale);
	Vector2d operator/(const float scale);
	Vector2d operator/(const int scale);
	bool operator==(const Vector2d& v);

	void Print(const char* a);
};
class IntersectionResult {
public:
	IntersectionResult() { vpoint = true; }
	Vector2d point;
	bool vpoint;
	//point is valid only if success is true. Success is true if the calculation is possible
	//or is actually useful. (useless If out of max distance for example)
	bool success;
};
//p1 + p2 = one lines, same for p3 and p4. Returns the point of intersection
IntersectionResult Intersect_line_line(Vector2d p1, Vector2d p2, Vector2d p3, Vector2d p4, int max_distance);
//p1 + p2 = line, p3 + radius = circle
IntersectionResult Intersect_line_circle(Vector2d p1, Vector2d p2, Vector2d p3, uint32_t radius, int dist);

bool ClampLines(Vector2d p, Vector2d sp, Vector2d ep);

#endif