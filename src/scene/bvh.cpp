#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}


BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

    // TODO (Part 2.1):
    // Construct a BVH from the given vector of primitives and maximum leaf
    // size configuration. The starter code build a BVH aggregate with a
    // single leaf node (which is also the root) that encloses all the
    // primitives.


    BBox bbox;

    int count = 0;
    for (auto p = start; p != end; p++) {
        BBox bb = (*p)->get_bbox();
        bbox.expand(bb);
        count++;
    }

    BVHNode *node = new BVHNode(bbox);
    if (count <= max_leaf_size) {
        node->l = NULL;
        node->r = NULL;
        node->start = start;
        node->end = end;
    } else {

        int axis = 0;
        if (bbox.extent[1] > bbox.extent[0]){
            axis = 1;
        }
        else if (bbox.extent[2] > bbox.extent[axis]){
            axis = 2;
        }

        sort(start, end, [&](Primitive *a, Primitive *b) {
            return a->get_bbox().centroid()[axis] < b->get_bbox().centroid()[axis];
        });

        Vector3D center = bbox.centroid();
        double dividing_point = center[axis];
//        std::vector<Primitive *>::iterator leftStart;
//        std::vector<Primitive *>::iterator rightStart;
//        std::vector<Primitive *>::iterator leftEnd;
//        std::vector<Primitive *>::iterator rightEnd;


        //std::vector<Primitive *>::iterator leftStart = start;
        std::vector<Primitive *>::iterator mid;
        //std::vector<Primitive *>::iterator rightEnd = end;

//        auto mid = std::partition_point(start, end, [axis, dividing_point](Primitive *a) {
//            return a->get_bbox().centroid()[axis] > dividing_point;
//        });

//        auto *left = new vector<Primitive *>();
//        auto *right = new vector<Primitive *>();
//        BBox leftBB;
//        BBox rightBB;

        for (auto p = start; p != end; p++) {
            BBox bb = (*p)->get_bbox();
            if ((*p)->get_bbox().centroid()[axis] > dividing_point){
                mid = p;
                break;
            }
        }

//        for (auto p = start; p != end; p++) {
//            BBox bb = (*p)->get_bbox();
//            if (bb.centroid()[axis] < dividing_point){
//                left->push_back(*p);
//            } else {
//              right->push_back(*p);
//            }
//        }



        if (start == mid || end == mid){
//            int i = 0;
//
//            for (auto p = start; p != end; p++) {
//                if (i > count / 2) {
//                    mid = p;
//                }
//                i++;
//            }
            mid = start + (count / 2);
        }

        node->l = construct_bvh(start,mid, max_leaf_size);
        node->r = construct_bvh(mid,end, max_leaf_size);
    }
    return node;

}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.



//    if (!node->bb.intersect(ray, ray.min_t, ray.max_t)){
//        return false;
//    }
//    else {
//        if (node->isLeaf()){
//            for (auto p = node->start; p != node->end; p++) {
//                total_isects++;
//                if ((*p)->has_intersection(ray))
//                    return true;
//            }
//        }
//        else {
//            return has_intersection(ray, node->l) || has_intersection(ray, node->r);
//        }
//    }
//    return false;

//    if (!node->bb.intersect(ray, ray.min_t, ray.max_t)){
//        return false;
//    }
//    else {
//        if (node->isLeaf()){
//            for (auto p = node->start; p != node->end; p++) {
//                total_isects++;
//                if ((*p)->has_intersection(ray))
//                    return true;
//            }
//            return false;
//        }
//        else {
//            return has_intersection(ray, node->l) || has_intersection(ray, node->r);
//        }
//    }


  for (auto p : primitives) {
    total_isects++;
    if (p->has_intersection(ray))
      return true;
  }
  return false;
}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.

//
    if (!node->bb.intersect(ray, ray.min_t, ray.max_t)){
        return false;
    } else {
        if (!node->isLeaf()) {
            bool left = intersect(ray, i, node->l);
            bool right = intersect(ray, i, node->r);
            return left || right;
        } else {
            //cout << "here";
            bool hit = false;
            for (auto p = node->start; p != node->end; p++) {
                total_isects++;
                hit = (*p)->intersect(ray, i) || hit;
            }
            return hit;
        }
    }


//  bool hit = false;
//  for (auto p : primitives) {
//    total_isects++;
//    hit = p->intersect(ray, i) || hit;
//  }
//  return hit;


}

} // namespace SceneObjects
} // namespace CGL
