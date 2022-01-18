#include <Eigen/Core>
#include <Eigen/Dense>

struct Ray {
  /**
   * The origin point of the ray.
   */
  Eigen::Vector3f o;
  /**
   * This is the direction vector of the ray, usually given as the lookat vec3f.
   */
  Eigen::Vector3f d;
  // Constructors and destructor
  Ray(Eigen::Vector3f o, Eigen::Vector3f d) {

  };
  ~Ray() {
  };
  // Returns a point at o+t(d)
  Eigen::Vector3f evaluate(float t) {
    return o+(t*d);
  };
};

