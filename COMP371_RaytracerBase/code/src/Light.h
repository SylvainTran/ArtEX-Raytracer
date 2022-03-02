#pragma once
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>

class HitRecord;
class Ray;

struct Light {
    std::string type;
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
        this->type = "light";
    };
    Light(Eigen::Vector3f id, Eigen::Vector3f is) {
        this->type = "light";
        this->id = id;
        this->is = is;
    };
    ~Light() {};
    virtual Eigen::Vector3f illuminate(Ray& r, HitRecord& hrec);
    friend inline std::ostream& operator<<(std::ostream& ofs, Light* l) {
        ofs<<"id: "<<l->id<<"is: "<<l->is<<std::endl;
        return ofs;
    };
};
