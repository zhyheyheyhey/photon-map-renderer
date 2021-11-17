#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

Vector3f backgroundColor;

inline float clamp(float x) {
    if (x > 0 && x < 1) return x;
    else if (x >= 1) return 1;
    else return 0;
}

inline float toInt(float x) {
    return float(pow(clamp(x), 1/2.2));
}

Vector3f radiance(const Ray &r, int depth, unsigned short *Xi, Group* baseGroup) {
    float t;
    Hit hit = Hit();
    bool isIntersect = baseGroup -> intersect(r, hit, 1e-3);
    if (!isIntersect) {
        return backgroundColor;
    }
    Vector3f x = r.getOrigin() + hit.getT() * r.getDirection();
    // if (depth == 0) printf("%f %f %f\n", x.x(), x.y(), x.z());
    Vector3f n = hit.getNormal().normalized();
    Vector3f nl = Vector3f::dot(n, r.getDirection()) < 0 ? n : (-1) * n;
    Vector3f materialColor = hit.getMaterial() -> diffuseColor;
    float p = materialColor.x() > materialColor.y() ? materialColor.x() : materialColor.y();
    if (materialColor.z() > p)
        p = materialColor.z();
    if (++depth > 5) {
        if (depth > 30)
            return hit.getMaterial() -> emissionColor;
        if (erand48(Xi) < p) {
            materialColor = (1 / p) * materialColor;
        }
        else
            return hit.getMaterial() -> emissionColor;
    }

    // cout << depth << endl;
    
    if (hit.getMaterial() -> refl == 0) {
        float r1 = 2 * (float) M_PI * erand48(Xi);
        float r2 = erand48(Xi);
        float r2s = sqrt(r2);
        Vector3f w = nl;
        Vector3f u = Vector3f::cross((fabs(w.x()) > .1 ? Vector3f::UP : Vector3f::RIGHT), w).normalized();
        Vector3f v = Vector3f::cross(w, u);
        Vector3f d = (cos(r1) * r2s * u + sin(r1) * r2s * v + w * sqrt(1 - r2)).normalized();
        return hit.getMaterial() -> emissionColor + materialColor * radiance(Ray(x, d), depth, Xi, baseGroup);
    }
    Vector3f nextDir = r.getDirection() - 2 * Vector3f::dot(n, r.getDirection()) * n;
    if (hit.getMaterial() -> refl == 1) {
        return hit.getMaterial() -> emissionColor + materialColor * radiance(Ray(x, nextDir), depth, Xi, baseGroup);
    }
    Ray reflRay = Ray(x, nextDir);
    bool into = Vector3f::dot(n, nl) > 0;
    float nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc; 
    float ddn = Vector3f::dot(r.getDirection().normalized(), nl);
    float cos2t = 1 - nnt * nnt * (1 - ddn * ddn);
    if (cos2t < 0) {
        return hit.getMaterial() -> emissionColor + materialColor * radiance(reflRay, depth, Xi, baseGroup);
    }
    Vector3f tdir = (r.getDirection().normalized() * nnt - Vector3f(n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))))).normalized();
    float a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1-(into ? -ddn : Vector3f::dot(tdir, n));
    float Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
    return hit.getMaterial() -> emissionColor + materialColor * (depth>2 ? (erand48(Xi) < P ? 
    radiance(reflRay, depth, Xi, baseGroup) * RP : radiance(Ray(x, tdir), depth, Xi, baseGroup) * TP) :
    radiance(reflRay, depth, Xi, baseGroup) * Re + radiance(Ray(x, tdir), depth, Xi, baseGroup) * Tr);
}

int main(int argc, char *argv[]) {
    for (int argNum = 0; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 4) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.
    int samps = atoi(argv[3]);

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    
    SceneParser parser = SceneParser(argv[1]);

    backgroundColor = parser.getBackgroundColor();
    
    Camera* camera = parser.getCamera();
    Group* baseGroup = parser.getGroup();
    Image img = Image(camera -> getWidth(), camera -> getHeight());
    img.SetAllPixels(parser.getBackgroundColor());
    Vector3f t_finalColor = Vector3f::ZERO;
    Vector3f *finalColor = new Vector3f[camera -> getWidth() * camera -> getHeight()];
    const int NUM_threads = 10;
    #pragma omp parallel for schedule(dynamic, 1) private(t_finalColor) num_threads(NUM_threads)
    for (int x = 0; x < camera -> getWidth(); x++) {
        fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*x/(camera -> getWidth() - 1));
        for (unsigned short y = 0, Xi[3] = {0, 0, x * x * x}; y < camera -> getHeight(); y++) {
            for (int sx = 0; sx < 2; sx++) {
                for (int sy = 0; sy < 2; sy++, t_finalColor = Vector3f(0., 0., 0.)) {
                    float r1=2*erand48(Xi), dx = r1<1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                    float r2=2*erand48(Xi), dy = r2<1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                    Ray camRay[2020];
                    for (int s = 0; s < samps; s++)
                        camRay[s] = camera -> generateRay(Vector2f((float) x - 0.15 + 0.3 * (float) sx + 0.2 * dx, (float) y - 0.15 + 0.3 * (float) sy + 0.2 * dy), Xi);
                    for (int s = 0; s < samps; s++) {
                        t_finalColor = t_finalColor + radiance(camRay[s], 0, Xi, baseGroup) * (1. / samps);
                        // printf("%f %f %f\n", t_finalColor.x(), t_finalColor.y(), t_finalColor.z());
                    }
                    finalColor[x * camera -> getHeight() + y] = finalColor[x * camera -> getHeight() + y] + Vector3f(clamp(t_finalColor.x()), 
                        clamp(t_finalColor.y()), clamp(t_finalColor.z()))*.25;
                }
            }
        }
        // printf("%d\n", x);
    }
    for (int x = 0; x < camera -> getWidth(); x++) 
        for (int y = 0; y < camera -> getHeight(); y++)
            img.SetPixel(x, y, Vector3f(toInt(finalColor[x * camera -> getHeight() + y].x()), toInt(finalColor[x * camera -> getHeight() + y].y()), toInt(finalColor[x * camera -> getHeight() + y].z())));
    // for(int i = 0; i < baseGroup -> getGroupSize(); i++)
        // printf("%d\n", baseGroup -> record[i]);
    img.SaveBMP(argv[2]);
    delete(finalColor);
    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

