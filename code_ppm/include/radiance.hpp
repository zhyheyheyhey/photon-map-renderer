#ifndef RADIANCE_H
#define RADIENCE_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vecmath.h>
#include "object3d.hpp"
#include "photon.hpp"

Vector3f backgroundColor;
PhotonMap* map;
double estimate_r, estimate_k;
SceneParser* parser_pnt;

int numPhotons = 1e4;
int emitPhotons = 0;

inline double clamp(double x) {
    if (x > 0 && x < 1) return x;
    else if (x >= 1) return 1;
    else return 0;
}

inline double toInt(double x) {
    return double(pow(clamp(x), 1/2.2));
    return x;
}


void print(const Vector3f& a) {
    printf("%lf %lf %lf \n", a.x(), a.y(), a.z());
}

void photonTracing(Photon photon, int depth, unsigned short *Xi, Object3D* baseGroup, PhotonMap* Map, bool indir = false, bool specOnly = true) {
    double t;
    Hit hit = Hit();
    Photon tp = photon;
    bool isIntersect = baseGroup -> intersect(photon, hit, 1e-7);
    if (!isIntersect) {
        photon = tp;
        photon.setPower(Vector3f::ZERO);
        return ;
    }
    Vector3f x = photon.getOrigin() + hit.getT() * photon.getDirection();
    Vector3f n = hit.getNormal().normalized();
    Vector3f nl = Vector3f::dot(n, photon.getDirection()) < 0 ? n : (-1) * n;
    Vector3f materialColor = hit.color;

    double p = materialColor.x() > materialColor.y() ? materialColor.x() : materialColor.y();
    if (materialColor.z() > p)
        p = materialColor.z();
    if (++depth > 15)
        return ;
    if (erand48(Xi) < p) {
        materialColor = (1. / p) * materialColor;
    }
    else
        return ;
    
    if (hit.getMaterial() -> refl == 0) {
        double r1 = 2 * (double) M_PI * erand48(Xi);
        double r2 = erand48(Xi);
        double r2s = sqrt(r2);
        Vector3f w = nl;
        Vector3f u = Vector3f::cross((fabs(w.x()) > .1 ? Vector3f::UP : Vector3f::RIGHT), w).normalized();
        Vector3f v = Vector3f::cross(w, u);
        Vector3f d = (cos(r1) * r2s * u + sin(r1) * r2s * v + w * sqrt(1 - r2)).normalized();
        
        photon = Photon(x, d, materialColor * 
            tp.getPower()/* * (1. / ((materialColor.x() + materialColor.y() + materialColor.z()) / 3.0))*/);
        double xi = erand48(Xi);
        if (indir == true) Map -> insert(photon);
        indir = true;
        photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        return ;
    }
    Vector3f nextDir = photon.getDirection() - 2 * Vector3f::dot(n, photon.getDirection()) * n;
    if (hit.getMaterial() -> refl == 1) {
        indir = true;
        photon = Photon(x, nextDir, materialColor * tp.getPower());
        
        photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        return ;
    }

    indir = true;
    Ray reflRay = Ray(x, nextDir);
    bool into = Vector3f::dot(n, nl) > 0;
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc; 
    double ddn = Vector3f::dot(photon.getDirection().normalized(), nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
    if (cos2t < 0) {
        photon = Photon(x, nextDir, materialColor * tp.getPower());
        photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        return ;
    }
    Vector3f tdir = (photon.getDirection().normalized() * nnt - Vector3f(n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))))).normalized();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1-(into ? -ddn : Vector3f::dot(tdir, n));
    double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
    if (depth > 2) {
        if (erand48(Xi) < P) {
            photon = Photon(x, tdir, materialColor * tp.getPower() * TP);
            photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        }
        else {
            photon = Photon(x, nextDir, materialColor * tp.getPower() * RP);
            photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        }
        return ;
    }
    else {
        if (erand48(Xi) < Tr / (Re + Tr)) {
            photon = Photon(x, tdir, materialColor * tp.getPower() * (Re + Tr));
            photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        }
        else {
            photon = Photon(x, nextDir, materialColor * tp.getPower() * (Re + Tr));
            photonTracing(photon, depth, Xi, baseGroup, Map, indir);
        }
        return ;
    }
}

Vector3f radiance(const Ray &r, int depth, unsigned short *Xi, Object3D* baseGroup, PhotonTree* root, bool flag = false) {
    double t;
    Hit hit = Hit();
    bool isIntersect = baseGroup -> intersect(r, hit, 1e-7);
    if (!isIntersect) {
        return backgroundColor;
    }
    Vector3f x = r.getOrigin() + hit.getT() * r.getDirection();
    // if (depth == 0) printf("%lf %lf %lf\n", x.x(), x.y(), x.z());
    Vector3f n = hit.getNormal().normalized();
    Vector3f nl = Vector3f::dot(n, r.getDirection()) < 0 ? n : (-1) * n;
    Vector3f materialColor = hit.color;
    double p = materialColor.x() > materialColor.y() ? materialColor.x() : materialColor.y();
    if (materialColor.z() > p)
        p = materialColor.z();
    if (++depth > 10) {
        return hit.getMaterial() -> emissionColor;
        if (erand48(Xi) < p) {
            materialColor = (1 / p) * materialColor;
        }
        else
            return hit.getMaterial() -> emissionColor;
    }

    // cout << depth << endl;
    
    if (hit.getMaterial() -> refl == 0) {
        double r1 = 2 * (double) M_PI * erand48(Xi);
        double r2 = erand48(Xi);
        double r2s = sqrt(r2);
        Vector3f w = nl;
        Vector3f u = Vector3f::cross((fabs(w.x()) > .1 ? Vector3f::UP : Vector3f::RIGHT), w).normalized();
        Vector3f v = Vector3f::cross(w, u);
        Vector3f d = (cos(r1) * r2s * u + sin(r1) * r2s * v + w * sqrt(1 - r2)).normalized();
        if (erand48(Xi) < 0. && flag == false) {
            Vector3f finalColor = Vector3f::ZERO;
            for (int i = 0; i < parser_pnt -> getNumLights(); i++) {
                finalColor = finalColor + parser_pnt -> getLight(i) -> getRadiance(x, nl, materialColor, Xi, baseGroup);
                // print(parser_pnt -> getLight(i) -> getRadiance(x, nl, hit.getMaterial(), Xi, baseGroup));
            }
            return hit.getMaterial() -> emissionColor + materialColor * radiance(Ray(x, d), depth, Xi, baseGroup, root, true);
        }
        else {
            Vector3f finalColor = Vector3f::ZERO;
            // std::priority_queue<std::pair<double, Photon>, std::vector<std::pair<double, Photon>>, std::greater<std::pair<double, Photon>>> heap;
            // for (int i = 0; i < 10000; i++) {
            //     double cur_distance = Vector3f::dot(map -> head[i].getOrigin() - x, map -> head[i].getOrigin() - x);
            //     heap.push(std::make_pair(cur_distance, map -> head[i]));
            // }
            // for (int i = 0; i < 50; i++) {
            //     finalColor = finalColor + materialColor * heap.top().second.getPower();
            //     heap.pop();
            // }
            // double Radius = heap.top().first;
            // finalColor = finalColor / (Radius * M_PI / 20.);


            std::priority_queue<std::pair<double, Photon>> heap;  
            collectPhoton(root, x, estimate_r, estimate_k, heap);
            double Radius;
            if (heap.empty()) {
                Radius = estimate_r;
            }
            else {
                Radius = heap.top().first;
            }
            // printf("%d %lf\n", heap.size(), Radius);
            while (!heap.empty()) {
                finalColor = finalColor + materialColor * heap.top().second.getPower() / (Radius * M_PI * emitPhotons);
                heap.pop();
            }

            if (hit.getMaterial() -> emissionColor != Vector3f::ZERO)
                return hit.getMaterial() -> emissionColor;

            for (int i = 0; i < parser_pnt -> getNumLights(); i++) {
                finalColor = finalColor + parser_pnt -> getLight(i) -> getRadiance(x, nl, materialColor, Xi, baseGroup);
                // print(parser_pnt -> getLight(i) -> getRadiance(x, nl, hit.getMaterial(), Xi, baseGroup));
            }
            // printf("%d\n", heap.size());



            // int cnt = 0;
            // for (int i = 0; i < 100000; i++) {
            //     if ((map -> head[i].getOrigin() - x).length() < 10.0) {
            //         finalColor = finalColor + map -> head[i].getPower() / (100000 * M_PI * 100.);
            //         cnt += 1;
            //     }
            // }
            // printf("%d\n", cnt);
            // print(finalColor);
            if (flag == true) return finalColor;
            return hit.getMaterial() -> emissionColor + finalColor;
        }
        
    }
    Vector3f nextDir = r.getDirection() - 2 * Vector3f::dot(n, r.getDirection()) * n;
    if (hit.getMaterial() -> refl == 1) {
        return hit.getMaterial() -> emissionColor + materialColor * radiance(Ray(x, nextDir), depth, Xi, baseGroup, root, flag);
    }
    Ray reflRay = Ray(x, nextDir);
    bool into = Vector3f::dot(n, nl) > 0;
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc; 
    double ddn = Vector3f::dot(r.getDirection().normalized(), nl);
    double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
    if (cos2t < 0) {
        return hit.getMaterial() -> emissionColor + materialColor * radiance(reflRay, depth, Xi, baseGroup, root, flag);
    }
    Vector3f tdir = (r.getDirection().normalized() * nnt - Vector3f(n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))))).normalized();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1-(into ? -ddn : Vector3f::dot(tdir, n));
    double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
    return hit.getMaterial() -> emissionColor + materialColor * (depth>2 ? (erand48(Xi) < P ? 
    radiance(reflRay, depth, Xi, baseGroup, root, flag) * RP : radiance(Ray(x, tdir), depth, Xi, baseGroup, root, flag) * TP) :
    radiance(reflRay, depth, Xi, baseGroup, root, flag) * Re + radiance(Ray(x, tdir), depth, Xi, baseGroup, root, flag) * Tr);
    return hit.getMaterial() -> emissionColor + materialColor * 
        (radiance(reflRay, depth, Xi, baseGroup, root, flag) * Re + radiance(Ray(x, tdir), depth, Xi, baseGroup, root, flag) * Tr);
}

#endif