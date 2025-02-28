#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
//    float min_x = min.x;
//    float max_x = max.x;
//    float min_y = min.y;
//    float max_y = max.y;
//    float min_z = min.z;
//    float max_z = max.z;
//    float t_min_x = (min_x - r.o.x)/r.d.x;
//    float t_max_x = (max_x - r.o.x)/r.d.x;
//    if (t_max_x < t_min_x) {
//        std::swap(t_max_x, t_min_x);
//    }
//    float t_min_y = (min_y - r.o.y)/r.d.y;
//    float t_max_y = (max_y - r.o.y)/r.d.y;
//    if (t_max_y < t_min_y) {
//        std::swap(t_max_y, t_min_y);
//    }
//    float t_min_z = (min_z - r.o.z)/r.d.z;
//    float t_max_z = (max_z - r.o.z)/r.d.z;
//    if (t_max_z < t_min_z) {
//        std::swap(t_max_z, t_min_z);
//    }
//
//    if (t_min_x > t_max_y || t_min_y > t_max_x) {
//        return false;
//    }
//    if (t_min_z > t_max_y || t_min_y > t_max_z) {
//        return false;
//    }
//    if (t_min_x > t_max_z || t_min_z > t_max_x) {
//        return false;
//    }
//
//    float t_min;
//    t_min = t_min_x > t_min_y ? t_min_x:t_min_y;
//    t_min = t_min > t_min_z ? t_min:t_min_z;
//    float t_max;
//    t_max = t_max_x < t_max_y ? t_max_x:t_max_y;
//    t_max = t_max < t_max_z ? t_max:t_max_z;
//
////    if (t_max < t_min || t_min < t0 || t_max > t1){
////        return false;
////    }
//
////    if (t_min > t_max) {
////        return false;
////    }
////
//    if (t_min < t0 || t_max > t1) {
//        return false;
//    }
//    t0 = t_min > t0 ? t_min : t0;
//    t1 = t_max < t1 ? t_max : t1;
//    return true;
    
    double t_min_x = (min.x - r.o.x)/r.d.x;
    double t_max_x = (max.x - r.o.x)/r.d.x;
    if (t_max_x < t_min_x) {
        std::swap(t_max_x, t_min_x);
    }
    double t_min_y = (min.y - r.o.y)/r.d.y;
    double t_max_y = (max.y - r.o.y)/r.d.y;
    if (t_max_y < t_min_y) {
        std::swap(t_max_y, t_min_y);
    }
    double t_min_z = (min.z - r.o.z)/r.d.z;
    double t_max_z = (max.z - r.o.z)/r.d.z;
    if (t_max_z < t_min_z) {
        std::swap(t_max_z, t_min_z);
    }

    double t_min = std::max(t_min_x,t_min_y);
    t_min = std::max(t0, t_min);
    double t_max = std::min(t_max_x,t_max_y);
    t_max = std::min(t1, t_max);

    if (t_max < t_min) {
        return false;
    }

    t_min = std::max(t_min,t_min_z);
    t_max = std::min(t_max,t_max_z);
    if (t_max < t_min) {
        return false;
    }

    if (!((t1 - t_min) >= 0 && (t_max - t0) >= 0)) return false;

    return true;






}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
