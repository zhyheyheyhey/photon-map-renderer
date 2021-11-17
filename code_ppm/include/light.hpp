#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"
#include "ray.hpp"

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;
    virtual Ray generatePhoton(unsigned short* Xi) {
        printf("WRONG\n");
        return Ray(Vector3f::ZERO, Vector3f::ZERO);
        
    }

    virtual Vector3f getRadiance(const Vector3f &p, const Vector3f &normal, Vector3f color, unsigned short* Xi, Object3D* obj) {
        printf("WRONG\n");
        return Vector3f::ZERO;
    }

    Vector3f color;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

    Ray generatePhoton(unsigned short* Xi) {
        printf("WRONG\n");
        return Ray(Vector3f::ZERO, Vector3f::ZERO);
        
    }

    Vector3f getRadiance(const Vector3f &p, const Vector3f &normal, Vector3f color, unsigned short* Xi, Object3D* obj) {
        printf("WRONG\n");
        return Vector3f::ZERO;
    }

    Vector3f direction;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3f &p, const Vector3f &c) {
        position = p;
        color = c;
    }

    ~PointLight() override = default;

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
    }

    Ray generatePhoton(unsigned short* Xi) {
        Vector3f dir = Vector3f(2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1);
        while (dir.length() > 1.) {
            dir = Vector3f(2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1);
        }
        Ray ray(position, dir);
        return ray;
    }

    Vector3f getRadiance(const Vector3f &p, const Vector3f &normal, Vector3f color, unsigned short* Xi, Object3D* obj) {
        Vector3f materialColor = color;
        Ray ray(position, p - position);
        Hit hit = Hit();
        hit.t = 0.99;
        if (obj -> intersect(ray, hit, 1e-8))
            return Vector3f::ZERO;
        return Vector3f::dot(-(p - position).normalized(), normal) * color * materialColor / (4 * M_PI * Vector3f::dot(p - position, p - position));
    }

    Vector3f position;
};

class TriangleLight: public Light {
public:
    TriangleLight() = delete;

    TriangleLight(const Vector3f &a, const Vector3f &b, const Vector3f &c, const Vector3f &col) {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        normal = Vector3f().cross(a - b, b - c);
        color = normal.length() * col * 4 * M_PI;
		normal.normalize();
		d = -Vector3f().dot(a, normal);
    }

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        dir = 0.33 * (vertices[0] + vertices[1] + vertices[2]) - p;
        dir = dir / dir.length();
        col = color;
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

    Ray generatePhoton(unsigned short* Xi) {
        double p1 = erand48(Xi);
        double p2 = erand48(Xi);
        Vector3f orig = vertices[0] + p1 * (vertices[1] - vertices[0]) + p2 * (vertices[2] - vertices[0]);
        while (!is_inside(orig)) {
            p1 = erand48(Xi);
            p2 = erand48(Xi);
            orig = vertices[0] + p1 * (vertices[1] - vertices[0]) + p2 * (vertices[2] - vertices[0]);
        }
        Vector3f dir = Vector3f(2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1);
        while (dir.length() > 1.) {
            dir = Vector3f(2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1, 2 * erand48(Xi) - 1);
        }
        Ray ray(orig, dir);
        return ray;
    }

    Vector3f getRadiance(const Vector3f &p, const Vector3f &normal, Vector3f color, unsigned short* Xi, Object3D* obj) {
        Vector3f sum = Vector3f::ZERO;
        Vector3f materialColor = color;

        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                double p1 = (1.0 * i + 3 * erand48(Xi) - 3.) / 15;
                double p2 = (1.0 * j + 3 * erand48(Xi) - 3.) / 15;
                Vector3f orig = vertices[0] + p1 * (vertices[1] - vertices[0]) + p2 * (vertices[2] - vertices[0]);
                if (!is_inside(orig)) {
                    continue;
                }
                Ray ray(orig, p - orig);
                Hit hit = Hit();
                hit.t = 1.0 - 1e-15;
                if (obj -> intersect(ray, hit, 1e-8))
                    return Vector3f::ZERO;

                sum = sum + fabs(Vector3f::dot((p - orig).normalized(), this -> normal.normalized())) * Vector3f::dot(-(p - orig).normalized(), normal) * (color / (this -> normal.length() * 4 * M_PI)) * materialColor / 
                    (4 * M_PI * Vector3f::dot(p - orig, p - orig));
            }
        }
        return (1. / 112.5) * sum;
    }

    Vector3f normal;
	Vector3f vertices[3];
	double d;
};

#endif // LIGHT_H
