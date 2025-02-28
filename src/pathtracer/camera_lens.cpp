#include "camera.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR, double rndTheta) const {

  // Part 2, Task 4:
  // compute position and direction of ray from the input sensor sample coordinate.
  // Note: use rndR and rndTheta to uniformly sample a unit disk.

    Vector3D pLens = Vector3D(lensRadius * sqrt(rndR) * cos(2 * PI * rndTheta),
                             lensRadius * sqrt(rndR) * sin(2 * PI * rndTheta), 0);
    auto left = Vector3D(-tan(radians(this->hFov) * 0.5), -tan(radians(this->vFov) * 0.5), -1);
    auto right = Vector3D(tan(radians(this->hFov) * 0.5), tan(radians(this->vFov) * 0.5), -1);
    Vector3D pFilm = Vector3D((1 - x) * left.x + x * right.x, (1 - y) * left.y + y * right.y, -1);
    Vector3D pFocus = pFilm * focalDistance - pLens;
    pFocus = c2w * pFocus;
    pFocus.normalize();

    Ray ret  = Ray(c2w * pLens + pos, pFocus);
    ret.min_t = nClip;
    ret.max_t = fClip;
    return ret;


     
}


} // namespace CGL
