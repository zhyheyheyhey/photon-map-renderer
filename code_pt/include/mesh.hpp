#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"


struct TriangleIndex {
    TriangleIndex() {
        x[0] = 0; x[1] = 0; x[2] = 0;
    }
    int &operator[](const int i) { return x[i]; }

    float MINxyz(int dim) {
        float Min = 999999999.;
        for (int i = 0; i < 3; i++) {
            if (vertices[x[i]][dim] < Min) {
                Min = vertices[x[i]][dim];
            }
        }
        return Min;
    }

    float MAXxyz(int dim) {
        float Max = -999999999.;
        for (int i = 0; i < 3; i++) {
            if (vertices[x[i]][dim] > Max) {
                Max = vertices[x[i]][dim];
            }
        }
        return Max;
    }

    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3]{};
    std::vector<Vector3f>::iterator vertices;
};

class MeshTree {
public:
    TriangleIndex cut;
    int dim;
    MeshTree* left;
    MeshTree* right;
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
};


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);
    ~Mesh();

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, float tmin) override;
    bool intersect(MeshTree* node, const Ray &r, Hit &h, float tmin);

    void computeNormal();
    void buildMeshTree(MeshTree* node, std::vector<TriangleIndex>& t, int left_ind, int right_ind);
    void deleteMeshTree(MeshTree* node);
    MeshTree* root;
};

#endif
