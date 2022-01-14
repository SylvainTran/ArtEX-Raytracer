#pragma once
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

// My code
#include "Mesh.h"

struct Sphere : public Mesh {
	Sphere();
	Sphere(Eigen::Vector3f centre);
	~Sphere();
	Eigen::Vector3f centre;
	float radius;
	void info() override;
    friend std::ostream& operator<<(std::ostream& ios, Sphere* s) {
	    ios<<"Sphere : centre="<<s->centre<<"\n, radius="<<s->radius<<std::endl;
	    return ios;
    };
};