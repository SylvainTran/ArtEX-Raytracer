#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>

class HitRecord;
class Ray;

struct Light {
  /**
   *  The centre.
   **/ 
  Eigen::Vector3f centre;
  /**
   *  The diffuse intensity.
   **/
  Eigen::Vector3f id;
  /**
   *  The specular intensity.
   **/
  Eigen::Vector3f is;
  /**
   *  Constructors and destructors.
   **/
  Light() {

  };
  Light(Eigen::Vector3f id, Eigen::Vector3f is) {
    this->id = id;
    this->is = is;
  };
  virtual Eigen::Vector3f illuminate(Ray& r, HitRecord& hrec);
  ~Light() {};
  friend inline std::ostream& operator<<(std::ostream& ofs, Light* l) {
    ofs<<"id: "<<l->id<<"is: "<<l->is<<std::endl;
    return ofs;
  };
};
