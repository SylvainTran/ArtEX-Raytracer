#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

struct Mesh {
	Mesh();
	~Mesh();
	Eigen::Vector3f centre;
	virtual void info();
};