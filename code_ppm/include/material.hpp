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

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, int s = 0, double prob = 0.2) :
            diffuseColor(d_color), emissionColor(s_color), refl(s), absorbProb(prob) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f diffuseColor;
    Vector3f emissionColor;
    int refl;
    double absorbProb;
};


#endif // MATERIAL_H
