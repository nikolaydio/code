#include "utility.h"

#include <cmath>

Vector2d Vector2d::GetNormalized() {
	Vector2d out; float len = Len();
	out.x = x / len;
	out.y = y / len;
	return out;
}
float Vector2d::Len() {
	return sqrt(x*x + y*y);
}
float Vector2d::Dot(const Vector2d& v) {
	return x * v.x + y * v.y;
}
Vector2d Vector2d::operator+(const Vector2d& v) {
	Vector2d out;
	out.x = x + v.x;
	out.y = y + v.y;
	return out;
}
Vector2d Vector2d::operator-(const Vector2d& v) {
	Vector2d out;
	out.x = x - v.x;
	out.y = y - v.y;
	return out;
}
Vector2d& Vector2d::operator=(const Vector2d& v) {
	x = v.x; y = v.y; return *this;
}
Vector2d Vector2d::operator*(const float scale) {
	Vector2d out;
	out.x = x * scale;
	out.y = y * scale;
	return out;
}
Vector2d Vector2d::operator*(const int scale) {
	Vector2d out;
	out.x = x * scale;
	out.y = y * scale;
	return out;
}
Vector2d Vector2d::operator/(const float scale) {
	Vector2d out;
	out.x = x / scale;
	out.y = y / scale;
	return out;
}
Vector2d Vector2d::operator/(const int scale) {
	Vector2d out;
	out.x = x / scale;
	out.y = y / scale;
	return out;
}
Vector2d::Vector2d(float X, float Y) { 
	x = X; y = Y;
}
Vector2d::Vector2d() {
	x = 0; y = 0;
}
bool Vector2d::operator==(const Vector2d& v) {
	if(std::abs(x - v.x) < 0.0001 && std::abs(y - v.y) < 0.0001){
		return true;
	}
	return false;
}

#include <cstdio>
void Vector2d::Print(const char* a) {
	printf("%s: X->%f, Y->%f\n", a, x, y);
}

IntersectionResult Intersect_line_line(Vector2d p1, Vector2d p2, Vector2d p3, Vector2d p4, int max_distance) {
	IntersectionResult out;
	Vector2d v1 = p2 - p1;

	float det = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	if(std::abs(det) < 0.0001)
	{
		out.success = false;
		return out;
	}
	float pre = (p1.x*p2.y - p1.y*p2.x);
	float post = (p3.x*p4.y - p3.y*p4.x);
	out.point.x = ( pre * (p3.x - p4.x) - (p1.x - p2.x) * post) / det;
	out.point.y = ( pre * (p3.y - p4.y) - (p1.y - p2.y) * post) / det;

	if((p2-p1).Dot(p2-out.point) >= 0) {
		out.success = false;
		return out;
	}
	if(max_distance != 0)
	{
		if((out.point - p1).Len() <= max_distance)
			out.success = true;
		else
			out.success = false;
	}else
	{
		out.success = true;
	}
	return out;
}
#include <stdio.h>
IntersectionResult Intersect_line_circle(Vector2d p1, Vector2d p2, Vector2d p3, uint32_t radius, int dist) {
	IntersectionResult res;
	Vector2d d;
	if(dist != 0)
		d = (p2 - p1).GetNormalized() * dist;
	else
		d = p2 - p1;

	Vector2d f = p1 - p3;
	float a = d.Dot( d );
	float b = 2 * f.Dot( d );
	float c = f.Dot( f ) - radius * radius;

	float discriminant = b * b - 4 * a * c;
	if ( discriminant < 0 ) {
		res.success = false;
		return res;
	} else {
		discriminant = sqrt( discriminant );

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);
		//printf("%f, %f\n", t1,t2);

		if (t1 >= 0 && t1 <= 1) {
			res.success = true;
			res.point = p1 + d * t1;
			return res;
		}
		if (t2 >= 0 && t2 <= 1) {
			res.success = true;
			res.point = p1 + d * t2;
			return res;
		}
		res.success = false;
		return res;
	}
}
#include <algorithm>
bool ClampLines(Vector2d p, Vector2d sp, Vector2d ep) {
	if(sp.x == ep.x) {
		if(p.y < std::min(sp.y, ep.y) || p.y > std::max(sp.y, ep.y)) {
			return false;
		}
	}else {
		if(p.x < std::min(sp.x, ep.x) || p.x > std::max(sp.x, ep.x)) {
			return false;
		}
	}
	return true;
}