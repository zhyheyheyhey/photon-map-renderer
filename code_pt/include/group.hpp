#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {
        num_objects = 0;
    }

    explicit Group (int num_objects) {
        this -> num_objects = num_objects;
        object_list = new Object3D* [num_objects];
    }

    ~Group() override {
        for(int i = 0; i < num_objects; i++) {
            delete object_list[i];
        }
        delete object_list;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool is_intersect = false;
        for(int i = 0; i < num_objects; i++) {
            is_intersect |= object_list[i] -> intersect(r, h, tmin);
            // record[i] |= object_list[i] -> intersect(r, h, tmin);
            if(object_list[i] -> intersect(r, h, tmin) == true && h.getMaterial() == nullptr) {
                printf("object %d is wrong\n", i);
            }
            // if(object_list[i] -> intersect(r, h, tmin) == true) {
                // printf("object %d", i);
            
        }
        // printf("%f\n", h.getMaterial() -> getDiffuseColor()[0]);
        return is_intersect;
    }

    void addObject(int index, Object3D *obj) {
        object_list[index] = obj;
    }

    int getGroupSize() {
        return num_objects;
    }
    
    // bool record[100];

private:
    int num_objects;
    Object3D** object_list;
};

#endif
	
