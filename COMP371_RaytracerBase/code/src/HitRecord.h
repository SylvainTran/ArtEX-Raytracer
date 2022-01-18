#include <Eigen/Core>
#include <Eigen/Dense>

struct HitRecord {
  Mesh* m;
  float t;
  Eigen::Vector3f n;
  // Constructors and destructors
  // The constructor used to indicate a miss, by setting t = +inf
  HitRecord(float t) {
    this->t = t;
    this->m = nullptr;
    this->n = Eigen::Vector3f(0,0,0);
  };
  HitRecord(Mesh* m, float t, Eigen::Vector3f n) {
    this->m = m;
    this->t = t;
    this->n = n;
  };
  ~HitRecord() {
  
  };
};
