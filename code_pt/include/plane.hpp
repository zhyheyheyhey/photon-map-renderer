#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {
        normal = Vector3f(1, 0, 0);
        d = 0;
    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this -> normal = normal.normalized();
        this -> d = -d;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        if (Vector3f::dot(normal, r.getDirection()) < 1e-3 && Vector3f::dot(normal, r.getDirection()) > -1e-3) return false;
        float t_intersect = -(d + Vector3f().dot(normal, r.getOrigin())) / Vector3f().dot(normal, r.getDirection());
        if(t_intersect < tmin) {
            // printf("asdasd\n");
            return false;
        }
        if(t_intersect > h.getT()) {
            // printf("%f\n", h.getT());
            return false;
        }
        // printf("%f\n", material -> getDiffuseColor()[0]);
        if(Vector3f().dot(normal, r.getDirection()) < 0)
            h.set(t_intersect, material, normal);
        else
            h.set(t_intersect, material, -normal);
        return true;
        
    }

    Vector3f normal;
    float d;

protected:


};

#endif //PLANE_H
		

