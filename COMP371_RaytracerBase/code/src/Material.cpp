#include "Material.h"

Material::Material() {

};
Material::Material(Eigen::Vector3f l, Eigen::Vector3f v, Eigen::Vector3f n) {
  this->l = l;
  this->v = v;
  this->n = n;
};
Material::Material(Material& mat) {
  this->l = mat.l;
  this->v = mat.v;
  this->n = mat.n;
  this->ac = mat.ac;
  this->dc = mat.dc;
  this->sc = mat.sc;
  this->ka = mat.ka;
  this->kd = mat.kd;
  this->ks = mat.ks;
  this->pc = mat.pc;
};
Material& Material::operator=(Material& mat) {
  this->l = mat.l;
  this->v = mat.v;
  this->n = mat.n;
  this->ac = mat.ac;
  this->dc = mat.dc;
  this->sc = mat.sc;
  this->ka = mat.ka;
  this->kd = mat.kd;
  this->ks = mat.ks;
  this->pc = mat.pc;
  return *this;
};
Material::~Material() {

};
Eigen::Vector3f Material::evaluate(Eigen::Vector3f l, Eigen::Vector3f v, Eigen::Vector3f n) {

};
