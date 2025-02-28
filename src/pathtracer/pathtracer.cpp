#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();
//
//  // w_out points towards the source of the ray (e.g.,
//  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
//
//  // This is the same number of total samples as
//  // estimate_direct_lighting_importance (outside of delta lights). We keep the
//  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out = (0, 0, 0);

////    // TODO (Part 3): Write your sampling loop here
////    // TODO BEFORE YOU BEGIN
////    // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading
////
//    for (int i = 0; i < num_samples; i ++){
//        Vector3D one_sample = hemisphereSampler->get_sample();
//        Vector3D one_sample_world = o2w * one_sample;
//        Ray one_sample_ray = Ray(hit_p + Vector3D(EPS_D) * one_sample_world,  one_sample_world);
//        Intersection *inter = new Intersection();
//
//        one_sample_ray.min_t = EPS_F;
//
//
//        if (bvh->intersect(one_sample_ray, inter))
//            // L_next * f * cos(theta)
////
//            L_out += (inter->bsdf->get_emission() * isect.bsdf->f(w_out, one_sample) * cos_theta(one_sample))/(1/(2 * PI));
//    }
//
//    L_out = L_out/num_samples;
//    return L_out;
    for (int i = 0; i < num_samples; i ++){
        // Vector3D one_sample = hemisphereSampler->get_sample();
        //one_sample.normalize();
        Vector3D one_sample_obj = hemisphereSampler->get_sample();
        one_sample_obj.normalize();

        // the normal is simply a unit vector in z direction, so cos(theta) = dot(normal, one_sample_obj
        // which is the same is simply the z component of the vector so we dont need the unit vector.

        // Vector3D normal = Vector3D(0, 0, 1);

        double cos = cos_theta(one_sample_obj);

        if (cos < 0) {
            continue;
        }
        // wo is w_out for f call, wi is the sampled ray for the f call
        Ray one_sample_ray = Ray(hit_p, o2w * one_sample_obj);
        one_sample_ray.min_t = EPS_F;
        Intersection inter;
        // ray intersection with scene

        if (bvh->intersect(one_sample_ray, &inter)){
            if (inter.bsdf->get_emission() == 0) {
                continue;
            }
            L_out = L_out + inter.bsdf->get_emission() * isect.bsdf->f(w_out, one_sample_obj) * cos * (2 * PI);
        }
    }
    L_out = L_out / (double) num_samples;
    return L_out;




}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
    // Estimate the lighting from this intersection coming directly from a light.
    // To implement importance sampling, sample only from lights, not uniformly in
    // a hemisphere.

    // make a coordinate system for a hit point
    // with N aligned with the Z direction.
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    // w_out points towards the source of the ray (e.g.,
    // toward the camera if this is a primary ray)
    const Vector3D hit_p = r.o + r.d * isect.t;
    const Vector3D w_out = w2o * (-r.d);
    Vector3D L_out;

  for (auto l = scene->lights.begin(); l != scene->lights.end(); l++){
      if (!(*l)->is_delta_light()){
          Vector3D temp;
          double count;
          for (int j = 0; j < ns_area_light; j ++){
              Vector3D wi;
              double distToLight;
              double pdf;
              Vector3D radiance = (*l)->sample_L(hit_p, &wi,&distToLight,&pdf);

              Vector3D one_sample_world = wi;
              Vector3D one_sample = w2o * wi;
              Ray one_sample_ray = Ray(hit_p + Vector3D(EPS_D) * one_sample_world,  one_sample_world);
              Intersection *inter = new Intersection();


              one_sample_ray.min_t = EPS_F;
              one_sample_ray.max_t = distToLight - one_sample_ray.min_t ;

              if (cos_theta(one_sample) >= 0 && !bvh->intersect(one_sample_ray, inter)){
                      temp += (radiance * isect.bsdf->f(w_out, one_sample) * cos_theta(one_sample))/(pdf);;

              }
          }
          temp = temp/ns_area_light;
          L_out += temp;


      }
      else {

          Vector3D wi;
          double distToLight;
          double pdf;
          Vector3D radiance = (*l)->sample_L(hit_p, &wi,&distToLight,&pdf);
          Vector3D one_sample_world = wi;
          Vector3D one_sample = w2o * wi;
          Ray one_sample_ray = Ray(hit_p + Vector3D(EPS_D) * one_sample_world,  one_sample_world);
          Intersection *inter = new Intersection();
          Vector3D temp;

          one_sample_ray.min_t = EPS_F;
          one_sample_ray.max_t = distToLight - one_sample_ray.min_t ;


          if (cos_theta(one_sample) >= 0 && !bvh->intersect(one_sample_ray, inter)){
                 temp += (radiance * isect.bsdf->f(w_out, one_sample) * cos_theta(one_sample))/(pdf);

          }
          L_out += temp;

      }

      }

    return L_out;

}



//  return Vector3D(1.0);



Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light


  //return Vector3D(1.0);

  return isect.bsdf->get_emission();

}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`


      return estimate_direct_lighting_hemisphere(r, isect);
}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
//  Matrix3x3 o2w;
//  make_coord_space(o2w, isect.n);
//  Matrix3x3 w2o = o2w.T();
//
//  Vector3D hit_p = r.o + r.d * isect.t;
//  Vector3D w_out = w2o * (-r.d);
//
//  Vector3D L_out = (0, 0, 0);

  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.

//
//  Vector3D radiance;
//  L_out += one_bounce_radiance(r, isect);
//  if (coin_flip(0.7) ){
//      Vector3D w_in;
//      double pdf;
//      Vector3D f = isect.bsdf->sample_f(w_out, &w_in, &pdf);
//      Vector3D one_sample = w_in;
//      Vector3D one_sample_world = o2w * w_in;
//      Ray one_sample_ray = Ray(hit_p + Vector3D(EPS_D) * one_sample_world,  one_sample_world);
//      Intersection *inter = new Intersection();
//      one_sample_ray.depth = r.depth - 1;
//      one_sample_ray.min_t = EPS_F;
//      if (cos_theta(one_sample) && bvh->intersect(one_sample_ray, inter)){
//          radiance = at_least_one_bounce_radiance(one_sample_ray, *inter);
//          L_out += (radiance * f * cos_theta(one_sample))/(pdf);
//      }
//  }
//  return L_out;
    Matrix3x3 o2w;
    make_coord_space(o2w, isect.n);
    Matrix3x3 w2o = o2w.T();

    Vector3D hit_p = r.o + r.d * isect.t;
    Vector3D w_out = w2o * (-r.d);

    Vector3D L_out(0, 0, 0);

    Vector3D radiance;
    L_out += one_bounce_radiance(r, isect);
    Vector3D w_in;
    double pdf;
    Vector3D f = isect.bsdf->sample_f(w_out, &w_in, &pdf);
    Vector3D one_sample = w_in;
    Vector3D one_sample_world = o2w * w_in;
    Ray one_sample_ray = Ray(hit_p + Vector3D(EPS_D) * one_sample_world,  one_sample_world);
    Intersection *inter = new Intersection();
    one_sample_ray.depth = r.depth - 1;
    one_sample_ray.min_t = EPS_F;
    float cpdf = 0.6;

    if (cos_theta(one_sample) && bvh->intersect(one_sample_ray, inter)){
        if (one_sample_ray.depth > 1 && coin_flip(cpdf)) {
            radiance = at_least_one_bounce_radiance(one_sample_ray, *inter);
            L_out += (radiance * f * cos_theta(one_sample))/pdf/cpdf;
        }
        else if (one_sample_ray.depth <= max_ray_depth && one_sample_ray.depth > 1 ){
            radiance = at_least_one_bounce_radiance(one_sample_ray, *inter);
            L_out += (radiance * f * cos_theta(one_sample))/(pdf);

        }
    }

    return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect))
    return envLight ? envLight->sample_dir(r) : L_out;


  L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

  // TODO (Part 3): Return the direct illumination.

  L_out = zero_bounce_radiance(r, isect);
  L_out += at_least_one_bounce_radiance(r, isect);





  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct

  return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa;          // total samples to evaluate
  Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel

  Vector3D est = Vector3D();
  int count = 0;
  int sample_so_far = 0;
  int sample_added = 0;
  float s1 = 0;
  float s2 = 0;
  float mean;
  float variance;
  float I;



    for (int i = 0; i < num_samples; i++) {
        count++;
        sample_so_far ++;
        Vector3D rad;
        if (count == samplesPerBatch){
            count = 0;
            mean = s1 / (float) sample_so_far;
            variance = (1.0/(sample_so_far-1.0)) * (s2 - ((s1 * s1)/(float) sample_so_far));
            I = 1.96 * ::sqrt(variance) / ::sqrt(sample_so_far);
            if ( I <= maxTolerance * mean){
                break;
            }
        }
        Vector2D dv = gridSampler->get_sample();
        dv = origin + dv;
        dv.x = dv.x / (sampleBuffer.w);
        dv.y = dv.y / (sampleBuffer.h);
        Vector2D samplesForLens = gridSampler->get_sample();
        Ray r = camera->generate_ray_for_thin_lens(dv.x, dv.y, samplesForLens.x, samplesForLens.y * 2.0 * PI);
        r.depth = max_ray_depth;
        rad = est_radiance_global_illumination(r);
        float illm = rad.illum();
        s1 = s1 + illm;
        s2 = s2 + illm * illm;

        est += rad;
        sample_added += 1;
    }

    est = est / sample_added;
    sampleBuffer.update_pixel(est, x, y);
    sampleCountBuffer[x + y * sampleBuffer.w] = sample_added;

}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL
