#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {

Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }

bool Triangle::has_intersection(const Ray &r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.

    Vector3D a_b = p2 - p1;
    Vector3D a_c = p3 - p1;

    Vector3D n = cross(a_b, a_c);


    //Vector3D center = alpha * p1 + beta * p2 + gamma * p3;

    Vector3D dis = p1 - r.o;
    double top = dot(dis, n);
    double bot = dot(r.d, n);

    if (bot == 0.0) {
        return false;
    }

    double t = top / bot;

    if (t < r.min_t || r.max_t < t) {
        return false;
    }

    Vector3D p = r.at_time(t);

    Vector3D temp1 = p - p1;
    Vector3D temp2 = cross(a_b, temp1);

    if (dot(n, temp2) < 0) {
        return false;
    }

    Vector3D temp3 = p3 - p2;
    temp1 = p - p2;
    temp2 = cross(temp3, temp1);

    if (dot(n, temp2) < 0) {
        return false;
    }

    temp3 = -1 * a_c;
    temp1 = p - p3;
    temp2 = cross(temp3, temp1);

    if (dot(n, temp2) < 0) {
        return false;
    }

    r.max_t = t;
    return true;
}

bool Triangle::intersect(const Ray &r, Intersection *isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly


    if (!has_intersection(r)) {
        return false;
    }

    double t = r.max_t;
    Vector3D p = r.at_time(t);

    isect->t = t;
    // p1 is A, p2 is B, p3 is C
//    Vector3D temp = cross((p3 - p1), (p - p1)) / cross((p1 - p2), (p3 - p2));
//    temp.normalize();
//    double alpha = temp.norm();
//    temp = cross((p1 - p2), (p - p2)) / cross((p1 - p2), (p3 - p2));
//    temp.normalize();
//    double beta = temp.norm();
//    //temp = cross((p3 - p2), (p - p3)) / cross((p1 - p2), (p3 - p2));
//    double gamma = 1 - alpha - beta;
//
//    Vector3D n = alpha * n1 + beta * n2 + gamma * n3;
//    n.normalize();
//    isect->n = n;
//
//    isect->primitive = this;
//    isect->bsdf = this->get_bsdf();


    Vector3D ab = p2 - p1;
    Vector3D ac = p3 - p1;
    Vector3D ap = p - p1;

    float abab = dot(ab, ab);
    float acac = dot(ac, ac);
    float abac = dot(ab, ac);
    float apab = dot(ap, ab);
    float apac = dot(ap, ac);

//    Vector3D P = cross(r.d, e2);
//    double det  = dot(e1,P);
//    Vector3D T = r.o - p1;
//
//    double a = dot(T, P) / det;
//    Vector3D q = cross(T, e1);
//    double b = dot(r.d, q) / det;
//    double c = 1 - a - b;

      float a = (acac * apab - abac * apac)/ (abab * acac - abac * abac);
      float b = (abab * apac - abac * apab)/ (abab * acac - abac * abac);
      float c = 1 - a - b;


        // Update Ray
        r.max_t = t;
        // Update intersection
        isect->t = t;
        isect->n = a * n1 + b * n2 + c * n3;
        isect->primitive = this;
        isect->bsdf = get_bsdf();


    return true;



}

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
