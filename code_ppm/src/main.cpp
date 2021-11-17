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
#include "radiance.hpp"

#include <string>

using namespace std;


int main(int argc, char *argv[]) {
    for (int argNum = 0; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 4) {
        // cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
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
    parser_pnt = &parser;

    backgroundColor = parser.getBackgroundColor();
    
    Camera* camera = parser.getCamera();
    Group* baseGroup = parser.getGroup();
    Image img = Image(camera -> getWidth(), camera -> getHeight());
    img.SetAllPixels(parser.getBackgroundColor());
    Vector3f t_finalColor = Vector3f::ZERO;
    Vector3f *finalColor = new Vector3f[camera -> getWidth() * camera -> getHeight()];
    estimate_r = 50.;
    estimate_k = 500.;
    double progressive_rate = 1.0;
    
    
    
    for (int i = 0; i < 20; i++) {

        for (int x = 0; x < camera -> getWidth(); x++) 
                for (int y = 0; y < camera -> getHeight(); y++)
                    finalColor[x * camera -> getHeight() + y] = (1 - progressive_rate) * finalColor[x * camera -> getHeight() + y];
        
        
        estimate_r *= 0.8;
        for (int j = 1; j <= (int) 1. * sqrt(i); j++) {
            int counter = 0;
            emitPhotons = 0;
            numPhotons = 1e6;
            unsigned short Xi[3] = {0, 0, (j * 2333 + i) % 10007};
            PhotonMap globalMap(numPhotons + 2000);
            // printf("asdasd\n");
            while (globalMap.tail < numPhotons) {
                // printf("%d\n", globalMap.tail);
                // printf("%lf %lf %lf\n", dir.x(), dir.y(), dir.z());
                Light* light = parser.getLight(counter);
                Photon p(light -> generatePhoton(Xi).getOrigin(), light -> generatePhoton(Xi).getDirection(), light -> color);
                emitPhotons += 1;
                // print(light -> generatePhoton(Xi).getOrigin());
                // print(light -> generatePhoton(Xi).getDirection());
                // cout << light -> generatePhoton(Xi).getOrigin() << ' ' << light -> generatePhoton(Xi).getDirection() << endl;
                photonTracing(p, 0, Xi, baseGroup, &globalMap);
                // Xi[2] = globalMap.tail / 100;
                counter = (counter + 1) % parser.getNumLights();
                if (emitPhotons % 10000 == 0)
                    printf("%d\n", globalMap.tail);
                // cout << parser.getNumLights() << endl;
            }

            numPhotons = globalMap.size;

            PhotonTree *root = new PhotonTree();
            root -> dim = 0;

            map = &globalMap;

            buildPhotonTree(root, globalMap.head, 0, numPhotons - 1);

            
        
            t_finalColor = Vector3f::ZERO;
            #pragma omp parallel for schedule(dynamic, 1) private(t_finalColor)
            for (int x = 0; x < camera -> getWidth(); x++) {
                fprintf(stderr,"\rRendering (%d spp) %5.2f%%",4 * samps,100.*x/(camera -> getWidth() - 1));
                for (unsigned short y = 0; y < camera -> getHeight(); y++) {
                    for (int sx = 0; sx < 2; sx++) {
                        for (int sy = 0; sy < 2; sy++, t_finalColor = Vector3f(0., 0., 0.)) {
                            double r1=2*erand48(Xi), dx = r1<1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
                            double r2=2*erand48(Xi), dy = r2<1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
                            Ray camRay[2020];
                            for (int s = 0; s < samps; s++)
                                camRay[s] = camera -> generateRay(Vector2f((float) x - 0.15 + 0.3 * (float) sx + 0.2 * dx, (float) y - 0.15 + 0.3 * (float) sy + 0.2 * dy), Xi);
                            for (int s = 0; s < samps; s++) {
                                
                                t_finalColor = t_finalColor + radiance(camRay[s], 0, Xi, baseGroup, root) * (1. / samps);
                                // printf("%lf %lf %lf\n", t_finalColor.x(), t_finalColor.y(), t_finalColor.z());
                            }
                            finalColor[x * camera -> getHeight() + y] = finalColor[x * camera -> getHeight() + y] + (1. / (int) (1. * sqrt(i))) * progressive_rate* Vector3f(clamp(t_finalColor.x()), 
                                clamp(t_finalColor.y()), clamp(t_finalColor.z())) * .25;
                        }
                    }
                }
            
                // printf("%d\n", x);
            }
            deleteTree(root);
            printf("%d %d\n", i, j);
            for (int x = 0; x < camera -> getWidth(); x++) 
                for (int y = 0; y < camera -> getHeight(); y++)
                img.SetPixel(x, y, Vector3f(toInt(finalColor[x * camera -> getHeight() + y].x()), toInt(finalColor[x * camera -> getHeight() + y].y()), toInt(finalColor[x * camera -> getHeight() + y].z())));
            char buf[100];
            for (int ii = 0; ii < strlen(argv[2]); ii++)
                buf[ii] = argv[2][ii];
            buf[strlen(argv[2])] = '.';
            buf[strlen(argv[2]) + 1] = '0' + j;
            buf[strlen(argv[2]) + 2] = '.';
            buf[strlen(argv[2]) + 3] = 'b';
            buf[strlen(argv[2]) + 4] = 'm';
            buf[strlen(argv[2]) + 5] = 'p';
            buf[strlen(argv[2]) + 6] = 0;
            img.SaveBMP(buf);
        }
        for (int x = 0; x < camera -> getWidth(); x++) 
            for (int y = 0; y < camera -> getHeight(); y++)
                img.SetPixel(x, y, Vector3f(toInt(finalColor[x * camera -> getHeight() + y].x()), toInt(finalColor[x * camera -> getHeight() + y].y()), toInt(finalColor[x * camera -> getHeight() + y].z())));
        // for(int i = 0; i < baseGroup -> getGroupSize(); i++)
            // printf("%d\n", baseGroup -> record[i]);
        img.SaveBMP(argv[2]);
        
        progressive_rate = std::max(1. / (i + 2), 0.3);
        printf("\n");
    }
    delete(finalColor);
    
    // cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

