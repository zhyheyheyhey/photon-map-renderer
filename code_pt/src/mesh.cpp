#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include "plane.hpp"


void print_(const Vector3f& a) {
    printf("%lf %lf %lf \n", a.x(), a.y(), a.z());
}

bool cmp1(TriangleIndex t1, TriangleIndex t2) {
    return (t1.vertices[t1[0]].x() 
        < t2.vertices[t2[0]].x());
}

bool cmp2(TriangleIndex t1, TriangleIndex t2) {
    return (t1.vertices[t1[0]].y() 
        < t2.vertices[t2[0]].y());
}

bool cmp3(TriangleIndex t1, TriangleIndex t2) {
    return (t1.vertices[t1[0]].z() 
        < t2.vertices[t2[0]].z());
}

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {
    // printf("asdasd\n");
    // Optional: Change this brute force method into a faster one.
    bool result = false;
    result |= intersect(root, r, h, tmin);
    // print_(r.getOrigin());
    // print_(r.getDirection());
    // printf("%lf %lf %lf %lf %lf %lf\n", root -> minX, root -> minY, root -> minZ, 
    //     root -> maxX, root -> maxY, root -> maxZ);
    // if (result == true) printf("asdasd\n");
    return result;



    // bool result = false;
    // for (int triId = 0; triId < (int) t.size(); ++triId) {
    //     TriangleIndex& triIndex = t[triId];
    //     Triangle triangle(v[triIndex[0]],
    //                       v[triIndex[1]], v[triIndex[2]], material);
    //     // triangle.normal = n[triId];
    //     result |= triangle.intersect(r, h, tmin);
    // }
    // // if (result == true) printf("asdasd\n");
    // return result;
}

bool Mesh::intersect(MeshTree* node, const Ray &r, Hit &h, float tmin) {
    // print_(node -> cut.vertices -> begin()[0]);
    float tmin_max = 0.0, tmax_min = 1e38;
    bool is_intersect, flag = false;
    Hit tmpH;
    Plane p1;
    p1 = Plane(Vector3f::RIGHT, node -> minX, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() > tmin_max) tmin_max = tmpH.getT();

    p1 = Plane(Vector3f::UP, node -> minY, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() > tmin_max) tmin_max = tmpH.getT();

    p1 = Plane(Vector3f::FORWARD, -node -> minZ, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() > tmin_max) tmin_max = tmpH.getT();

    p1 = Plane(Vector3f::RIGHT, node -> maxX, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() < tmax_min) tmax_min = tmpH.getT();

    p1 = Plane(Vector3f::UP, node -> maxY, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() < tmax_min) tmax_min = tmpH.getT();

    p1 = Plane(Vector3f::FORWARD, -node -> maxZ, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, tmin);
    if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (tmpH.getT() < tmax_min) tmax_min = tmpH.getT();

    

    if (flag == false)
        return flag;

    // printf("1111\n");

    flag = false;
    // printf("%lf\n", node -> minX);
    // if ((node -> cut).vertices == NULL) {
    //     // printf("%lf\n", node -> minX);
    //     printf("WRONG vertices\n");
    // }
    // printf("%lf\n", ((node -> cut).vertices -> begin() + (node -> cut)[0]) -> x());
    Triangle tri = Triangle((node -> cut).vertices[(node -> cut)[0]], 
                            (node -> cut).vertices[(node -> cut)[1]], 
                            (node -> cut).vertices[(node -> cut)[2]], 
                            material);
    // printf("3333\n");
    flag |= tri.intersect(r, h, tmin);

    if (tmin_max < tmax_min) {
        // printf("2222\n");
        return false;
    }
    
    else {
        if (node -> left != nullptr) {
            flag |= intersect(node -> left, r, h, tmin);
        }
        if (node -> right != nullptr) {
            flag |= intersect(node -> right, r, h, tmin);
        }
    }
    // printf("2222\n");
    return flag;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                trig.vertices = v.begin();
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texID;
                    trig[ii]--;
                }
                t.push_back(trig);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                trig.vertices = v.begin();
                t.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
        }
    }
    computeNormal();
    root = new MeshTree();
    root -> dim = 0;
    buildMeshTree(root, t, 0, (int) t.size() - 1);
    f.close();
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}

Mesh::~Mesh() {
    deleteMeshTree(root);
}

void Mesh::buildMeshTree(MeshTree* node, std::vector<TriangleIndex>& head, int left_ind, int right_ind) {
    node -> cut = head[left_ind];
    // if ((node -> cut).vertices == nullptr)
    //     printf("WRONG vertices\n");
    node -> minX = 999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> minX > head[i].MINxyz(0))
            node -> minX = head[i].MINxyz(0);
    }
    node -> maxX = -999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> maxX < head[i].MAXxyz(0))
            node -> maxX = head[i].MAXxyz(0);
    }
    node -> minY = 999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> minY > head[i].MINxyz(1))
            node -> minY = head[i].MINxyz(1);
    }
    node -> maxY = -999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> maxY < head[i].MAXxyz(1))
            node -> maxY = head[i].MAXxyz(1);
    }
    node -> minZ = 999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> minZ > head[i].MINxyz(2))
            node -> minZ = head[i].MINxyz(2);
    }
    node -> maxZ = -999999999.;
    for (int i = left_ind; i <= right_ind; i++) {
        if (node -> maxZ < head[i].MAXxyz(2))
            node -> maxZ = head[i].MAXxyz(2);
    }

    if (left_ind == right_ind) {
        // if ((node -> cut).vertices == nullptr)
        // printf("WRONG vertices\n");
        return ;
    }

    int mid = (left_ind + right_ind) / 2;
    
    if (node -> dim == 0)
        std::nth_element(head.begin() + left_ind, 
            head.begin() + mid, head.begin() + right_ind + 1, cmp1);
    else if (node -> dim == 1)
        std::nth_element(head.begin() + left_ind, 
            head.begin() + mid, head.begin() + right_ind + 1, cmp2);
    else
        std::nth_element(head.begin() + left_ind, 
            head.begin() + mid, head.begin() + right_ind + 1, cmp3);
    
    node -> right = new MeshTree();
    node -> right -> dim = (node -> dim + 4) % 3;
    node -> cut = head[mid];
    // print_(head[mid].vertices[0]);
    
    // if ((node -> cut).vertices == nullptr)
    //     printf("WRONG vertices\n");
    buildMeshTree(node -> right, head, mid + 1, right_ind);
    if (mid > left_ind) {
        node -> left = new MeshTree();
        node -> left -> dim = (node -> dim + 4) % 3;
        buildMeshTree(node -> left, head, left_ind, mid - 1);
    }
    // if ((node -> cut).vertices == nullptr)
    //     printf("WRONG vertices\n");
}

void Mesh::deleteMeshTree(MeshTree* node) {
    if (node -> left != nullptr)
        deleteMeshTree(node -> left);
    if (node -> right != nullptr)
        deleteMeshTree(node -> right);
    delete node;
}
