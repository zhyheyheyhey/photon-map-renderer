#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        center = Vector3f(0);
        radius = 1.0;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        this->center = center;
        this->radius = radius;
    }


    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f r1 = Vector3f();
        r1 = center - r.getOrigin();
        float t_mid = Vector3f().dot(r1, r.getDirection());
        t_mid /= r.getDirection().length();
        float t_delta = radius * radius - (Vector3f().dot(r1, r1) - t_mid * t_mid);
        if(t_delta < 0)
            return false;
        t_delta = sqrt(t_delta);
        float t1, t2;
        t1 = t_mid - t_delta;
        t2 = t_mid + t_delta;
        float t_close;
        if(t1 < tmin && t2 < tmin)
            return false;
        else if(t1 < tmin)
            t_close = t2;
        else if(t2 < tmin)
            t_close = t1;
        else
            t_close = std::min(t1, t2);
        Vector3f intersect_point = Vector3f();
        Vector3f intersect_normal = Vector3f();
        if(t_close / r.getDirection().length() < h.getT()) {
            intersect_point = r.pointAtParameter(t_close / r.getDirection().length());
            intersect_normal = intersect_point - center;
            h.set(t_close / r.getDirection().length(), material, intersect_normal.normalized());
            return true;
        }
        return true;
}
    Vector3f center;
    float radius;

protected:
    
};


#endif
