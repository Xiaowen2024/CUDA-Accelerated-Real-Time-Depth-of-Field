#include "sphere.h"

#include <cmath>

#include "pathtracer/bsdf.h"
#include "util/sphere_drawing.h"

namespace CGL {
namespace SceneObjects {

bool Sphere::test(const Ray &r, double &t1, double &t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
    
    double a = dot(r.d, r.d);
    Vector3D dis = r.o - o;
    //temp = 2.0 * temp;
    double b = 2.0 * dot(r.d, dis);
    //temp = temp / 2.0;
    double c = dot(dis, dis) - r2;
    //cout << c << " ";
    //<< b << " " << c << "; ";
    
    float det = b * b - 4 * a * c;
    
    //cout << a << " " << b << " " << c << " ";

    if (det < 0) {
        return false;
    } else if (det == 0) {
        t1 = -1.0;
        t2  = (-1.0 * b) / (2 * a);
    } else {
        det = sqrt(det);
        t1 = (-b + det) / (2 * a);
        t2 = (-b - det) / (2 * a);
    }

    return true;
}

bool Sphere::has_intersection(const Ray &r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
    double a = r.d.norm2();
    Vector3D dis = r.o - o;
    double b = 2.0 * dot(r.d, dis);
    double c = dis.norm2() - r2;

    float det = b * b - 4 * a * c;

    if (det < 0) {
        return false;
    }

    double t1 = (-b - ::sqrt(det)) / (2 * a);
    double t2 = (-b + ::sqrt(det)) / (2 * a);

    if (t2 < t1) {
        std::swap(t1, t2);
    }

    if (r.min_t <= t1 && t1 <= r.max_t) {
        r.max_t = t1;
        return true;
    } else if (r.min_t <= t2 && t2 <= r.max_t) {
        r.max_t = t2;
        return true;
    }
  return false;
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

    if (!has_intersection(r)) {
        return false;
    }

    i->t = r.max_t;






    i->n = r.o + i->t * r.d - o;
    i->n.normalize();

    i->primitive = this;
    i->bsdf = this->get_bsdf();

  return true;




}

void Sphere::draw(const Color &c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

} // namespace SceneObjects
} // namespace CGL
