#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <cstring>
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

bool Mesh::intersect(const Ray &r, Hit &h, double tmin) {
    // printf("asdasd\n");
    // Optional: Change this brute force method into a faster one.
    bool result = false;
    result |= intersect(root, r, h, tmin);
    // print_(r.getOrigin());
    // print_(r.getDirection());
    // printf("%lf %lf %lf %lf %lf %lf\n", root -> minX, root -> minY, root -> minZ, 
    //     root -> maxX, root -> maxY, root -> maxZ);
    // if (result == true) printf("asdasd\n");
    // if (!vt.empty() && result == true) printf("%lf %lf %lf\n", h.color.x(), h.color.y(), h.color.z());
    return result;



    // bool result2 = false;
    // for (int triId = 0; triId < (int) t.size(); ++triId) {
    //     TriangleIndex& triIndex = t[triId];
    //     Triangle triangle(v[triIndex[0]],
    //                       v[triIndex[1]], v[triIndex[2]], material);
    //     // triangle.normal = n[triId];
    //     result2 |= triangle.intersect(r, h, tmin);
    // }
    // // if (result2 == true) printf("asdasd\n");
    // if (result2 != result) { printf("asd\n");}
    // return result2;
}

Vector3f computeCoordinate(Vector3f& v1, Vector3f& v2, Vector3f& v3, Vector3f& v) {
    double totalS = Vector3f::cross(v1 - v2, v1 - v3).length() / 2.;
    double total1 = Vector3f::cross(v - v2, v - v3).length() / 2.;
    double total2 = Vector3f::cross(v - v1, v - v3).length() / 2.;
    double total3 = Vector3f::cross(v - v1, v - v2).length() / 2.;
    return ((1. / totalS) * Vector3f(total1, total2, total3));
}

bool Mesh::intersect(MeshTree* node, const Ray &r, Hit &h, double tmin) {
    // print_(node -> cut.vertices -> begin()[0]);
    double tmin_max = -1e38, tmax_min = 1e38, t1 = -1e15, t2 = 1e15;
    bool is_intersect, flag = false;
    Hit tmpH;
    Plane p1;

    t1 = -1e15; t2 = 1e15;
    p1 = Plane(Vector3f::RIGHT, node -> minX, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();

    p1 = Plane(Vector3f::RIGHT, node -> maxX, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();
    // printf("%lf %lf\n", t1, t2);
    if (t1 < t2) swap(t1, t2);

    if (tmin_max < t2) tmin_max = t2;
    if (tmax_min > t1) tmax_min = t1;

    t1 = -1e15; t2 = 1e15;
    p1 = Plane(Vector3f::UP, node -> minY, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();

    p1 = Plane(Vector3f::UP, node -> maxY, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();
    // printf("%lf %lf\n", t1, t2);
    if (t1 < t2) swap(t1, t2);

    if (tmin_max < t2) tmin_max = t2;
    if (tmax_min > t1) tmax_min = t1;
    
    t1 = -1e15; t2 = 1e15;
    p1 = Plane(Vector3f::FORWARD, -node -> minZ, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();

    p1 = Plane(Vector3f::FORWARD, -node -> maxZ, material);
    tmpH = Hit();
    is_intersect = p1.intersect(r, tmpH, -1e20);
    //if (is_intersect == false) return false;
    if (tmpH.getT() < h.getT()) flag = true;
    if (t1 < tmpH.getT() && is_intersect == true) t1= tmpH.getT();
    if (t2 > tmpH.getT() && is_intersect == true) t2= tmpH.getT();
    // printf("%lf %lf\n", t1, t2);
    if (t1 < t2) swap(t1, t2);

    if (tmin_max < t2) tmin_max = t2;
    if (tmax_min > t1) tmax_min = t1;

    bool flag2 = false;
    if (r.getOrigin().x() > node -> minX && r.getOrigin().x() < node -> maxX &&
        r.getOrigin().y() > node -> minY && r.getOrigin().y() < node -> maxY &&
        r.getOrigin().z() > node -> minZ && r.getOrigin().z() < node -> maxZ)
        flag2 = true;

    // if (flag == false && !flag2)
    //     return flag; 
    // printf("3333\n");
    if (tmin_max > tmax_min && !flag2) {
        // printf("2222\n");
        // printf("%lf %lf %lf %lf %lf %lf\n", root -> minX, root -> minY, root -> minZ, 
        // root -> maxX, root -> maxY, root -> maxZ);
        // print_(r.getDirection());
        // print_(r.getOrigin());
        // printf("%lf %lf\n", tmin_max, tmax_min);
        return false;
    }

    // printf("1111\n");

    flag = false;
    // printf("%lf\n", node -> minX);
    // if ((node -> cut).vertices == NULL) {
    //     // printf("%lf\n", node -> minX);
    //     printf("WRONG vertices\n");
    // }
    // printf("%lf\n", ((node -> cut).vertices -> begin() + (node -> cut)[0]) -> x());
    // print_((node -> cut).vertices[(node -> cut)[0]]);
    Triangle tri = Triangle(node -> cut.vertices[node -> cut[0]], 
                            node -> cut.vertices[node -> cut[1]], 
                            node -> cut.vertices[node -> cut[2]], 
                            material);
    // tri.normal = n[node -> cut[0]];
    // printf("3333\n");
    flag2 = tri.intersect(r, h, tmin);
    flag |= flag2;

    if (flag2) {
        if (!vn.empty()) {
            Vector3f dest = r.pointAtParameter(h.getT());
            Vector3f coef = computeCoordinate(node -> cut.vertices[node -> cut[0]], 
                node -> cut.vertices[node -> cut[1]], node -> cut.vertices[node -> cut[2]], dest);
            h.normal = coef[0] * vn[node -> cut.norm[0]] + 
                       coef[1] * vn[node -> cut.norm[1]] + 
                       coef[2] * vn[node -> cut.norm[2]];
        }
        if (!vt.empty()) {
            Vector3f dest = r.pointAtParameter(h.getT());
            Vector3f coef = computeCoordinate(node -> cut.vertices[node -> cut[0]], 
                node -> cut.vertices[node -> cut[1]], node -> cut.vertices[node -> cut[2]], dest);
            Vector2f texturePos = coef[0] * vt[node -> cut.tex[0]] + 
                                  coef[1] * vt[node -> cut.tex[1]] + 
                                  coef[2] * vt[node -> cut.tex[2]];
            // printf("%lf %lf\n", texturePos[0], texturePos[1]);
            int x = (int) (texturePos[0] * (double) map -> Width());
            int y = (int) (texturePos[1] * (double) map -> Height());
            // printf("%d %d\n", x, y);
            h.color = map -> GetPixel(x, y);
        }
    }
    
    if (node -> left != nullptr) {
         flag |= intersect(node -> left, r, h, tmin);
    }
    if (node -> right != nullptr) {
        flag |= intersect(node -> right, r, h, tmin);
    }
    
    // printf("2222\n");
    return flag;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    bool flag_image = false;
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::cout << filename << endl;
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    std::string vNorm("vn");
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
            std::string sv[3];
            std::stringstream read_line(line);
            read_line >> tok;
            read_line >> sv[0] >> sv[1] >> sv[2];
            // printf("asdd\n");
            TriangleIndex trig;
            for (int i = 0; i < 3; i++) {
                if (sv[i].find(bslash) == std::string::npos) {
                    std::stringstream read_id(sv[i]);
                    read_id >> trig[i];
                    trig[i]--;
                }
                else if (sv[i].find_first_of(bslash) - sv[i].find_last_of(bslash) < -1) {
                    std::replace(sv[i].begin(), sv[i].end(), bslash, space);
                    std::stringstream read_id(sv[i]);
                    read_id >> trig[i] >> trig.tex[i] >> trig.norm[i];
                    trig[i]--;
                    trig.tex[i]--;
                    trig.norm[i]--;
                }
                else if (sv[i].find_first_of(bslash) - sv[i].find_last_of(bslash) == -1) {
                    // cout << i << ' ' << sv[i] << endl;
                    std::replace(sv[i].begin(), sv[i].end(), bslash, space);
                    std::stringstream read_id(sv[i]);
                    read_id >> trig[i] >> trig.norm[i];
                    trig[i]--;
                    trig.norm[i]--;
                }
                else {
                    std::replace(sv[i].begin(), sv[i].end(), bslash, space);
                    std::stringstream read_id(sv[i]);
                    read_id >> trig[i] >> trig.tex[i];
                    trig[i]--;
                    trig.tex[i]--;
                }
            }
            trig.vertices = v.begin();
            trig.normals = vn.begin();
            trig.textures = vt.begin();
            trig.texmap = map;
            t.push_back(trig);
            // if (line.find(bslash) != std::string::npos) {
            //     std::replace(line.begin(), line.end(), bslash, space);
            //     std::stringstream facess(line);
            //     TriangleIndex trig;
            //     trig.vertices = v.begin();
            //     trig.normals = vn.begin();
            //     trig.textures = vt.begin();
            //     facess >> tok;
            //     for (int ii = 0; ii < 3; ii++) {
            //         facess >> trig[ii] >> texID;
            //         trig[ii]--;
            //     }
            //     t.push_back(trig);
            // } else {
            //     TriangleIndex trig;
            //     for (int ii = 0; ii < 3; ii++) {
            //         ss >> trig[ii];
            //         trig[ii]--;
            //     }
            //     trig.vertices = v.begin();
            //     trig.normals = vn.begin();
            //     trig.textures = vt.begin();
            //     t.push_back(trig);
            // }
        } else if (tok == texTok) {
            // printf("zhyzhy\n");
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            vt.push_back(texcoord);
            if (flag_image == false) {
                flag_image = true;
                char p[300];
                memcpy(p, filename, strlen(filename) - 3);
                p[strlen(filename) - 3] = 'p';
                p[strlen(filename) - 2] = 'p';
                p[strlen(filename) - 1] = 'm';
                p[strlen(filename)] = 0;
                // printf("asda\n");
                printf("%s\n", p);
                map = Image::LoadPPM(p);
                // cout << map -> Width() << ' ' << map -> Height() << endl;
                // printf("azhy\n");
            }
        } else if (tok == vNorm) {
            Vector3f normal;
            ss >> normal[0] >> normal[1] >> normal[2];
            vn.push_back(normal);
        }
    }
    computeNormal();
    root = new MeshTree();
    root -> dim = 0;
    buildMeshTree(root, t, 0, (int) t.size() - 1);
    f.close();
}

void Mesh::computeNormal() {
    // n.resize(t.size());
    // for (int triId = 0; triId < (int) t.size(); ++triId) {
    //     TriangleIndex& triIndex = t[triId];
    //     Vector3f a = v[triIndex[1]] - v[triIndex[0]];
    //     Vector3f b = v[triIndex[2]] - v[triIndex[0]];
    //     b = Vector3f::cross(a, b);
    //     n[triId] = b / b.length();
    // }
}

Mesh::~Mesh() {
    deleteMeshTree(root);
    delete map;
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

    // printf("%lf %lf %lf %lf %lf %lf\n", node -> minX, node -> minY, node -> minZ, 
    //     node -> maxX, node -> maxY, node -> maxZ);

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
    // print_(head[mid].vertices[node -> cut[0]]);
    
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
