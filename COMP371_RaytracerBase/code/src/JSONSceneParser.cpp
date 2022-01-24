// Standard
#include <iostream>
#include <string>
#include <memory>
using std::cout;
using std::endl;
#include <vector>
using std::vector;

// External Utilities
#include <Eigen/Core>
#include <Eigen/Dense>
#include "../external/json.hpp"

// My classes
#include "JSONSceneParser.h"
#include "GraphicsEngine.h"
#include "Camera.h"

JSONSceneParser::JSONSceneParser(GraphicsEngine* _g, nlohmann::json& j) : _g(_g), sceneJSONData(j) {

};
JSONSceneParser::~JSONSceneParser() {

};
JSONSceneParser::JSONSceneParser(JSONSceneParser& other) {
  this->_g = other._g;
  this->sceneJSONData = other.sceneJSONData;
};
JSONSceneParser& JSONSceneParser::operator=(JSONSceneParser& other) {
  if(&other == this) {
    return other;
  } else {
    this->_g = other._g;
    this->sceneJSONData = other.sceneJSONData;
    return *this;
  }
};
void JSONSceneParser::copyInputVector3f(int parameterLen, Eigen::Vector3f& output, std::vector<float> input) {
   for (int i = 0; i < parameterLen; i++){
     output[i] = input[i];
   }
};
void JSONSceneParser::copyRectangleCorners(Eigen::Vector3f& P1, Eigen::Vector3f& P2, Eigen::Vector3f& P3, Eigen::Vector3f& P4, vector<float> p1_input, vector<float> p2_input, vector<float> p3_input, vector<float> p4_input) {                       
  copyInputVector3f(3, P1, p1_input);
  copyInputVector3f(3, P2, p2_input);
  copyInputVector3f(3, P3, p3_input);
  copyInputVector3f(3, P4, p4_input);                        
  cout<<"Corners:\n"<<endl;
  cout<<"P1:\n"<<P1<<endl;
  cout<<"P2:\n"<<P2<<endl;
  cout<<"P3:\n"<<P3<<endl;
  cout<<"P4:\n"<<P4<<endl;      
};
void JSONSceneParser::parse_geometry(GraphicsEngine* gE) {
  for (auto itr = sceneJSONData["geometry"].begin(); itr!=sceneJSONData["geometry"].end();itr++) {
     std::string type;
     if(itr->contains("type")) {
      type = (*itr)["type"];
     } 

    if(type=="sphere") {
        Eigen::Vector3f centre(0,0,0);
        auto newSphere = std::make_shared<Sphere>();
        vector<float> input = (*(sceneJSONData["geometry"].begin()))["centre"];
        copyInputVector3f(3, centre, input);
        newSphere->centre = centre;
        float radius;
        radius = (*(sceneJSONData["geometry"].begin()))["radius"];
        newSphere->radius = radius;
        gE->addGeometry(newSphere.get());
      }
   }
};
void JSONSceneParser::parse_output(GraphicsEngine* gE) {
  for (auto itr = sceneJSONData["output"].begin(); itr!= sceneJSONData["output"].end(); itr++){      
    std::string filename;
    Eigen::Vector3f size(0,0,0), lookat(0,0,0), up(0,0,0), centre(0,0,0);     
    vector<float> centreInput = (*itr)["centre"];
    vector<float> sizeInput = (*itr)["size"];
    vector<float> lookAtInput = (*itr)["lookat"];
    vector<float> upInput = (*itr)["up"];
    copyInputVector3f(3, centre, centreInput);
    copyInputVector3f(3, lookat, lookAtInput);     
    copyInputVector3f(3, up, upInput);            
    copyInputVector3f(2, size, sizeInput);      
    float fov;
    fov = (*itr)["fov"].get<float>();
    // Set parameters in camera
    auto newCamera = std::make_shared<Camera>();
    newCamera->fov = fov;
    newCamera->centre = centre;
    newCamera->lookat = lookat;
    newCamera->up = up;
    newCamera->size = size;
    gE->cameraList.push_back(newCamera.get());
  }
  // Sets the first camera as the default active camera in the scene
  gE->activeSceneCamera = gE->cameraList[0];
};
bool JSONSceneParser::test_parse_geometry() {
  cout<<"Geometry: "<<endl;
  int gc = 0;

  // use iterators to read-in array types
  for (auto itr = sceneJSONData["geometry"].begin(); itr!= sceneJSONData["geometry"].end(); itr++){      
      std::string type;
      if(itr->contains("type")) {
        type = (*itr)["type"];
      } else {
        cout<<"Fatal error: geometry shoudl always contain a type!!!"<<endl;
        return false;
      }
      
      if(type=="sphere") {
        cout<<"Sphere: "<<endl;
        Eigen::Vector3f centre(0,0,0);  
        // This makes sure this persists in the game engine, but with weak_ptr
        // TODO: request the engine to allocate for a new geometry (shared_ptr), return it here and then use a copy constructor here or too complicated?      
        auto newSphere = std::make_shared<Sphere>();

        vector<float> input = (*(sceneJSONData["geometry"].begin()))["centre"];
        copyInputVector3f(3, centre, input);
        cout<<"Centre:\n"<<centre<<endl;
        newSphere->centre = centre;
        float radius;
        try {
          radius = (*(sceneJSONData["geometry"].begin()))["radius"];
          cout<<"Radius: "<<radius<<endl;
          newSphere->radius = radius;
        } catch(nlohmann::detail::type_error typeError) {
          std::cerr<<"Type error: Geometry radius must be a number! Aborting..."<<endl;
          return false;
        } catch(std::exception e) {
          std::cerr<<"Caught an unhandled error!"<<endl;
          return false;
        }
        cout<<"Sphere info:"<<endl;
        cout<<newSphere->centre<<endl;        
        cout<<newSphere->radius<<endl;
      }
      if(type=="rectangle"){
        cout<<"rectangle: "<<endl;
        Eigen::Vector3f P1(0,0,0);
        Eigen::Vector3f P2(0,0,0);
        Eigen::Vector3f P3(0,0,0);
        Eigen::Vector3f P4(0,0,0);                        
        vector<float> p1_input = (*itr)["P1"];
        vector<float> p2_input = (*itr)["P2"];
        vector<float> p3_input = (*itr)["P3"];
        vector<float> p4_input = (*itr)["P4"];                        
        copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);                       
      }
      ++gc;
  }
  cout<<"We have: "<<gc<<" objects!"<<endl;
  return true;
};
bool JSONSceneParser::test_parse_lights() {
  cout<<"Light: "<<endl;
  int lc = 0;
  
  // use iterators to read-in array types
  for (auto itr = sceneJSONData["light"].begin(); itr!= sceneJSONData["light"].end(); itr++) {      
      std::string type;
      if(itr->contains("type")){
          type = (*itr)["type"];
      } else {
          cout<<"Fatal error: light shoudl always contain a type!!!"<<endl;
          return false;
      }
      
      if(type=="point") {
          cout<<"Point based light: "<<endl;
          Eigen::Vector3f centre(0,0,0);
          vector<float> input = (*(sceneJSONData["light"].begin()))["centre"];
          copyInputVector3f(3, centre, input);
          cout<<"Centre:\n"<<centre<<endl;          
      }
      if(type=="area") {
        cout<<"Area-based light: "<<endl;          
        Eigen::Vector3f P1(0,0,0);
        Eigen::Vector3f P2(0,0,0);
        Eigen::Vector3f P3(0,0,0);
        Eigen::Vector3f P4(0,0,0);                        
        vector<float> p1_input = (*itr)["P1"];
        vector<float> p2_input = (*itr)["P2"];
        vector<float> p3_input = (*itr)["P3"];
        vector<float> p4_input = (*itr)["P4"];                        
        copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);
      }
      ++lc;
   }
  cout<<"We have: "<<lc<<" objects!"<<endl;
  return true;
};
bool JSONSceneParser::test_parse_output() {
  cout<<"Outputs: "<<endl;
  int lc = 0;
  
  // use iterators to read-in array types
  for (auto itr = sceneJSONData["output"].begin(); itr!= sceneJSONData["output"].end(); itr++){      
      std::string filename;
      if(itr->contains("filename")){
        filename = (*itr)["filename"];
      } else {
        cout<<"Fatal error: output shoudl always contain a filename!!!"<<endl;
        return false;
      }
      Eigen::Vector3f size(0,0,0), lookat(0,0,0), up(0,0,0), centre(0,0,0);     
      vector<float> centreInput = (*itr)["centre"];
      vector<float> sizeInput = (*itr)["size"];
      vector<float> lookAtInput = (*itr)["lookat"];
      vector<float> upInput = (*itr)["up"];
      copyInputVector3f(3, centre, centreInput);
      copyInputVector3f(3, lookat, lookAtInput);     
      copyInputVector3f(3, up, upInput);            
      copyInputVector3f(2, size, sizeInput);      
      // I am retrieving the field of view
      // this is mandatory field here, but if I dont check if it exists,
      // the code will throw an exception which if not caught will end the execution of yoru program
      cout<<"A"<<endl;
      float fov;
      try {
        fov = (*itr)["fov"].get<float>();
      } catch(nlohmann::detail::type_error typeError) {
        std::cerr<<"Type error: Camera FOV must be a number! Aborting..."<<endl;
        return false;
      } catch(std::exception e) {
        std::cerr<<"Caught an unhandled error!"<<endl;
        return false;
      }
      cout<<"B"<<endl;
      cout<<"Filename: "<<filename<<endl;
      cout<<"Size:\n"<<size<<endl;
      cout<<"Camera centre:\n"<<centre<<endl;
      cout<<"FOV:\n"<<fov<<endl;
      cout<<"lookat:\n"<<lookat<<endl;
      cout<<"up:\n"<<up<<endl;            
      ++lc;
  }
  cout<<"We have: "<<lc<<" objects!"<<endl;
  return true;
};
