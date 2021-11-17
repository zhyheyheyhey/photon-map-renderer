#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include "image.hpp"

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions


class Plane : public Object3D {
public:
    Plane() {
        normal = Vector3f(1, 0, 0);
        d = 0;
    }

    Plane(const Vector3f &normal, double d, Material *m) : Object3D(m) {
        this -> normal = normal.normalized();
        this -> d = -d;
        this -> has_texture = false;
    }

    Plane(const Vector3f &normal, double d, Material *m, Image* img, Vector3f axis_x, Vector3f axis_y, double offset_x, double offset_y) : Object3D(m) {
        this -> normal = normal.normalized();
        this -> d = -d;
        this -> has_texture = true;
        this -> map = img;
        this -> axis_x = axis_x;
        this -> axis_y = axis_y;
        this -> offset_x = offset_x;
        this -> offset_y = offset_y;
    }

    ~Plane() {
        if (this -> has_texture) delete map;
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        if (Vector3f::dot(normal, r.getDirection()) < 1e-10 && Vector3f::dot(normal, r.getDirection()) > -1e-10) return false;
        double t_intersect = -(d + Vector3f().dot(normal, r.getOrigin())) / Vector3f().dot(normal, r.getDirection());
        if(t_intersect < tmin) {
            // printf("asdasd\n");
            return false;
        }
        if(t_intersect > h.getT()) {
            // printf("%lf\n", h.getT());
            return false;
        }
        // printf("%lf\n", material -> getDiffuseColor()[0]);
        Vector3f color = material -> diffuseColor;
        int x = (int) (Vector3f::dot(axis_x, r.pointAtParameter(t_intersect)) - offset_x);
        int y = (int) (Vector3f::dot(axis_y, r.pointAtParameter(t_intersect)) - offset_y);
        // printf("%lf, %lf", Vector3f::dot(axis_x, r.pointAtParameter(t_intersect)), Vector3f::dot(axis_y, r.pointAtParameter(t_intersect)));
        // if (has_texture) printf("%d %d %d %d \n", x, y, map -> Width(), map -> Height());
        if (has_texture == true) {
            // printf("%d %d\n", x, y);
            x = (map -> Width() + x % map -> Width()) % map -> Width();
            y = (map -> Height() + y % map -> Height()) % map -> Height();
            color = map -> GetPixel(x, y);
            // printf("%lf %lf %lf\n", color.x(), color.y(), color.z());
        }
        if(Vector3f().dot(normal, r.getDirection()) < 0)
            h.set(t_intersect, material, normal, color);
        else
            h.set(t_intersect, material, -normal, color);
        return true;
    }

    Vector3f normal;
    double d;
    Image* map;
    Vector3f axis_x, axis_y;
    double offset_x, offset_y;

protected:
    bool has_texture;

};

#endif //PLANE_H
		

