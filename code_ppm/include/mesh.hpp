#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include "image.hpp"


struct TriangleIndex {
    TriangleIndex() {
        x[0] = 0; x[1] = 0; x[2] = 0;
    }
    int &operator[](const int i) { return x[i]; }

    double MINxyz(int dim) {
        double Min = 999999999.;
        for (int i = 0; i < 3; i++) {
            if (vertices[x[i]][dim] < Min) {
                Min = vertices[x[i]][dim];
            }
        }
        return Min;
    }

    double MAXxyz(int dim) {
        double Max = -999999999.;
        for (int i = 0; i < 3; i++) {
            if (vertices[x[i]][dim] > Max) {
                Max = vertices[x[i]][dim];
            }
        }
        return Max;
    }

    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3]{};
    int norm[3];
    int tex[3];
    std::vector<Vector3f>::iterator vertices;
    std::vector<Vector3f>::iterator normals;
    std::vector<Vector2f>::iterator textures;
    Image* texmap;
};

class MeshTree {
public:
    TriangleIndex cut;
    int dim;
    MeshTree* left;
    MeshTree* right;
    double minX, maxX;
    double minY, maxY;
    double minZ, maxZ;
};


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);
    ~Mesh();

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    std::vector<Vector3f> vn;
    std::vector<Vector2f> vt;
    bool intersect(const Ray &r, Hit &h, double tmin) override;
    bool intersect(MeshTree* node, const Ray &r, Hit &h, double tmin);

    void computeNormal();
    void buildMeshTree(MeshTree* node, std::vector<TriangleIndex>& t, int left_ind, int right_ind);
    void deleteMeshTree(MeshTree* node);
    MeshTree* root;
    Image* map;
};

#endif
