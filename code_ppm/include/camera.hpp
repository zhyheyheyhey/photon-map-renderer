#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include "plane.hpp"
#include "material.hpp"
#include "hit.hpp"

class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point, unsigned short* Xi = nullptr) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle, double focal_distance, double radius) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        distance = (imgH / 2) / tan(angle / 2); 
        this -> focal_distance = focal_distance;
        this -> radius = radius;
        Xi[0] = 0, Xi[1] = 1, Xi[2] = 2;
        return ;
    }

    Ray generateRay(const Vector2f &point, unsigned short* Xi = nullptr) override {
        //
        if (focal_distance < 0.) { 
            Vector3f Rc = Vector3f(point - Vector2f((double) width / 2, (double) height / 2), distance);
            Matrix3f R = Matrix3f(horizontal, up, direction);
            Ray r = Ray(center, R * Rc);
            return r;
        }

        else {
            Vector3f Rc = Vector3f(point - Vector2f((double) width / 2, (double) height / 2), distance);
            Matrix3f R = Matrix3f(horizontal, up, direction);
            Ray r = Ray(center, R * Rc);
            Material m = Material(Vector3f::ZERO);
            Plane p_focal = Plane(direction.normalized(), 
                Vector3f::dot(direction.normalized(), center) + focal_distance, &m);
            Hit hit = Hit();
            p_focal.intersect(r, hit, -1e8);
            Vector3f f_point = r.pointAtParameter(hit.getT());
            Vector3f orig = center;
            double p1 = radius, p2 = radius;
            while(p1 * p1 + p2 * p2 > radius * radius) {
                if (Xi == nullptr) p1 = erand48(this -> Xi) * 2. * radius - radius;
                else p1 = erand48(Xi) * 2. * radius - radius;
                if (Xi == nullptr) p2 = erand48(this -> Xi) * 2. * radius - radius;
                else p2 = erand48(Xi) * 2. * radius - radius;
            }
            orig = center + p1 * horizontal + p2 * up;
            Ray blurred_r = Ray(orig, f_point - orig);
            return blurred_r;
        }
    }

    double distance;
    double focal_distance;
    double radius;
    unsigned short Xi[3];
};

#endif //CAMERA_H
