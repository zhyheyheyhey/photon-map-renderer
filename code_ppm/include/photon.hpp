#ifndef PHOTON_H
#define PHOTON_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vecmath.h>
#include <queue>
#include <algorithm>
#include "object3d.hpp"

const int maxDepth = 10;

class Photon : public Ray {
public: 
    Photon() : Ray(Vector3f::ZERO, Vector3f::ZERO) {
        this -> power = Vector3f::ZERO;
    }
    Photon(const Vector3f& orig, const Vector3f& dir, const Vector3f& power) : Ray(orig, dir) {
        this -> power = power;
    }
    Photon(const Photon& r) : Ray(r) {
        power = r.power;
    }

    const Vector3f &getPower() const {
        return power;
    }

    void setPower(const Vector3f& power) {
        this -> power = power;
    }

    void setOrigin(const Vector3f& origin) {
        this -> origin = origin;
    }

    void setDir(const Vector3f& dir) {
        this -> direction = dir;
    }

    bool operator < (const Photon& a) const {
        return power.x() > a.getPower().x();
    }
private:
    Vector3f power;
};

class PhotonMap {
public: 
    PhotonMap(int size) {
        this -> size = size;
        tail = 0;
        head = new Photon[size]();
    }

    const Photon &get(int ind) const {
        return head[ind];
    }

    void insert(const Photon& p) {
        head[tail++] = p;
    }

    ~PhotonMap() {
        delete[] head;
    }

    Photon* head;
    int size;
    int tail;
};

class PhotonTree {
public:
    Photon cut;
    int dim;
    PhotonTree* left;
    PhotonTree* right;
    double minX, maxX;
    double minY, maxY;
    double minZ, maxZ;
};

bool cmp_x(Photon p1, Photon p2) {
    return p1.getOrigin().x() < p2.getOrigin().x();
}

bool cmp_y(Photon p1, Photon p2) {
    return p1.getOrigin().y() < p2.getOrigin().y();
}

bool cmp_z(Photon p1, Photon p2) {
    return p1.getOrigin().z() < p2.getOrigin().z();
}

void buildPhotonTree(PhotonTree* node, Photon* head, int left_ind, int right_ind) {
    node -> cut = head[left_ind];
    node -> minX = std::min_element(head + left_ind, head + right_ind + 1, cmp_x) -> getOrigin().x();
    node -> maxX = std::max_element(head + left_ind, head + right_ind + 1, cmp_x) -> getOrigin().x();
    node -> minY = std::min_element(head + left_ind, head + right_ind + 1, cmp_y) -> getOrigin().y();
    node -> maxY = std::max_element(head + left_ind, head + right_ind + 1, cmp_y) -> getOrigin().y();
    node -> minZ = std::min_element(head + left_ind, head + right_ind + 1, cmp_z) -> getOrigin().z();
    node -> maxZ = std::max_element(head + left_ind, head + right_ind + 1, cmp_z) -> getOrigin().z();
    // printf("%lf %lf %lf %lf %lf %lf\n", node -> minX, node -> maxX, node -> minY, node -> maxY, node -> minZ, node -> maxZ);
    if (left_ind == right_ind) {
        return ;
    }
    int mid = (left_ind + right_ind) / 2;
    // printf("%d %d %d\n", left_ind, mid, right_ind);
    if (node -> dim == 0)
        std::nth_element(head + left_ind, head + mid, head + right_ind + 1, cmp_x);
    else if (node -> dim == 1)
        std::nth_element(head + left_ind, head + mid, head + right_ind + 1, cmp_y);
    else
        std::nth_element(head + left_ind, head + mid, head + right_ind + 1, cmp_z);

    node -> right = new PhotonTree();
    node -> right -> dim = (node -> dim + 4) % 3;
    node -> cut = head[mid];
    buildPhotonTree(node -> right, head, mid + 1, right_ind);
    if (mid > left_ind) {
        node -> left = new PhotonTree();
        node -> left -> dim = (node -> dim + 4) % 3;
        buildPhotonTree(node -> left, head, left_ind, mid - 1);
    }
}

void deleteTree(PhotonTree *node) {
    if (node -> left != nullptr)
        deleteTree(node -> left);
    if (node -> right != nullptr)
        deleteTree(node -> right);
    delete node;
}

void collectPhoton(PhotonTree* node, Vector3f center, double maxDistance, int k, std::priority_queue<std::pair<double, Photon>>& heap) {
    double dx = 0, dy = 0, dz = 0;
    if (center.x() < node -> minX)
        dx = node -> minX - center.x();
    else if (center.x() > node -> maxX)
        dx = center.x() - node -> maxX;
    if (center.y() < node -> minY)
        dy = node -> minY - center.y();
    else if (center.y() > node -> maxY)
        dy = center.y() - node -> maxY;
    if (center.z() < node -> minZ)
        dz = node -> minZ - center.z();
    else if (center.z() > node -> maxZ)
        dz = center.z() - node -> maxZ;
    
    double cur_distance = dx * dx + dy * dy + dz * dz;
    if (cur_distance > maxDistance)
        return ;
    cur_distance = Vector3f::dot(node -> cut.getOrigin() - center, node -> cut.getOrigin() - center);
    if (cur_distance < maxDistance) {
        if (heap.size() < k) {
            heap.push(std::make_pair(cur_distance, node -> cut));
        }
        else if (heap.size() == k && cur_distance < heap.top().first) {
            heap.pop();
            heap.push(std::make_pair(cur_distance, node -> cut));
            maxDistance = heap.top().first;
        }
    }
    if (node -> left != nullptr)
        collectPhoton(node -> left, center, maxDistance, k, heap);
    if (node -> right != nullptr)
        collectPhoton(node -> right, center, maxDistance, k, heap);
}

#endif