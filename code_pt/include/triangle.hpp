#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		for(int i = 0; i <= 2; i++)
			vertices[i] = Vector3f();
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f();
		normal = Vector3f().cross(a - b, b - c);
		normal.normalize();
		d = -Vector3f().dot(a, normal);
	}

	bool is_inside( const Vector3f& point) {
		Vector3f normal1 = Vector3f().cross(vertices[0] - point, vertices[1] - point);
		Vector3f normal2 = Vector3f().cross(vertices[1] - point, vertices[2] - point);
		Vector3f normal3 = Vector3f().cross(vertices[2] - point, vertices[0] - point);
		if(Vector3f().dot(normal1, normal2) < 0)
			return false;
		if(Vector3f().dot(normal3, normal2) < 0)
			return false;
		return true;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
        float t_intersect = -(d + Vector3f().dot(normal, ray.getOrigin())) / Vector3f().dot(normal, ray.getDirection());
		Vector3f intersect_point = ray.pointAtParameter(t_intersect);
		if(is_inside(intersect_point) == false)
			return false;
		if(t_intersect < tmin)
            return false;
        if(t_intersect > hit.getT())
            return false;
        if(Vector3f().dot(normal, ray.getDirection()) < 0)
            hit.set(t_intersect, material, normal);
        else
            hit.set(t_intersect, material, -normal);
        return true;
	}
	Vector3f normal;
	Vector3f vertices[3];
	float d;
protected:

};

#endif //TRIANGLE_H
