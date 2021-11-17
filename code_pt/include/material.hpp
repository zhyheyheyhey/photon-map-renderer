#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, int s = 0) :
            diffuseColor(d_color), emissionColor(s_color), refl(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        // printf("1\n");
        // printf("%f %f %f\n", emissionColor[0], emissionColor[1], emissionColor[2]);
        Vector3f N = hit.getNormal().normalized(), V = -ray.getDirection(), L = dirToLight;
        
        V.normalize(), L.normalize();
        
        Vector3f R = 2 * Vector3f().dot(N, dirToLight) * N - L;
        
        float C1 = std::max((float) 0, Vector3f().dot(L, N));
        
        float C2 = std::max((float) 0, Vector3f().dot(V, R));
        // printf("0\n");
        // printf("%f %f\n", C1, C2);
        
        Vector3f shaded = lightColor * (diffuseColor * C1 + emissionColor * pow(C2, refl));
        return shaded;
    }

    Vector3f diffuseColor;
    Vector3f emissionColor;
    int refl;
};


#endif // MATERIAL_H
