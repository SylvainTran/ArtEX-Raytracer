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
using json = nlohmann::json;

// My classes
#include "JSONSceneParser.h"
#include "RayTracer.h"
#include "Camera.h"
#include "Rectangle.h"
#include "PointLight.h"
#include "AreaLight.h"
#include "BoundingBox.h"

JSONSceneParser::JSONSceneParser(nlohmann::json& j) : sceneJSONData(j) {

};
JSONSceneParser::~JSONSceneParser() {

};
JSONSceneParser::JSONSceneParser(JSONSceneParser& other) {
  this->sceneJSONData = other.sceneJSONData;
};
JSONSceneParser& JSONSceneParser::operator=(JSONSceneParser& other) {
  if(&other == this) {
    return other;
  } else {
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
};
void JSONSceneParser::parse_geometry(RayTracer* gE) {
    Surface* surface;
    BoundingBox* bbox;
    for (auto itr = sceneJSONData["geometry"].begin(); itr!=sceneJSONData["geometry"].end();itr++) {
    std::string type;
    if(itr->contains("type")) {
     type = (*itr)["type"];
     //cout<<"Type in json: "<<type<<endl;
    }
    if(type=="sphere") {
      Eigen::Vector3f centre(0,0,0);
      //std::cout<<"*itr: "<<*(itr)<<endl;
      vector<float> input = (*itr)["centre"].get<std::vector<float>>();
      copyInputVector3f(3, centre, input);
      surface = new Sphere(centre);
      float radius;
      radius = (*itr)["radius"].get<float>();
      dynamic_cast<Sphere*>(surface)->radius = radius;
    }
    if(type=="rectangle") {
      //std::cout<<"Parsing a rectangle! "<<std::endl;
      Eigen::Vector3f P1(0,0,0);
      Eigen::Vector3f P2(0,0,0);
      Eigen::Vector3f P3(0,0,0);
      Eigen::Vector3f P4(0,0,0);
      vector<float> p1_input;
      if((*itr)["p1"] != NULL) {
        p1_input = (*itr)["p1"].get<std::vector<float>>();
      }
      vector<float> p2_input;
      if((*itr)["p2"] != NULL) {
        p2_input = (*itr)["p2"].get<std::vector<float>>();
      }
      vector<float> p3_input;
      if((*itr)["p3"] != NULL) {
        p3_input = (*itr)["p3"].get<std::vector<float>>();
      }
      vector<float> p4_input; 
      if((*itr)["p4"] != NULL) {
        p4_input = (*itr)["p4"].get<std::vector<float>>();
      }
      copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);
      Triangle* triangle1 = new Triangle(P1,P2,P3);
      Triangle* triangle2 = new Triangle(P1,P3,P4);
      surface = new Rectangle(triangle1, triangle2);

      // BOUNDING BOX
      // if boundingbox = on
      bbox = new BoundingBox(dynamic_cast<Rectangle*>(surface));
    }
    // material settings
    vector<float> ac_input,dc_input,sc_input;
    Eigen::Vector3f ac,dc,sc;
    float ka,kd,ks,pc;

    if(itr->contains("ac")) {
      ac_input = (*itr)["ac"].get<std::vector<float>>();
      copyInputVector3f(3, ac, ac_input);
      //cout<<"ac: "<<ac<<endl;
    }
    if(itr->contains("dc")) {
      dc_input = (*itr)["dc"].get<std::vector<float>>();
      copyInputVector3f(3, dc, dc_input);
//      cout<<"dc: "<<dc<<endl;
    }
    if(itr->contains("sc")) {
      sc_input = (*itr)["sc"].get<std::vector<float>>();
      copyInputVector3f(3, sc, sc_input);
//      cout<<"sc: "<<sc<<endl;
    }
    if(itr->contains("ka")) {
      ka = (*itr)["ka"];
//      cout<<"ka: "<<ka<<endl;
    }
    if(itr->contains("kd")) {
      kd = (*itr)["kd"];
//      cout<<"kd: "<<kd<<endl;
    }
    if(itr->contains("ks")) {
      ks = (*itr)["ks"];
//      cout<<"ks: "<<ks<<endl;
    }
    if(itr->contains("pc")) {
      pc = (*itr)["pc"];
//      cout<<"pc: "<<pc<<endl;
    }
    surface->mat->ac = ac;
    surface->mat->dc = dc;
    surface->mat->sc = sc;
    surface->mat->ka = ka;
    surface->mat->kd = kd;
    surface->mat->ks = ks;
    surface->mat->pc = pc;
//    // Set default dc in mat as surface color for now
//    // ----
//    surface->color = surface->mat->dc;
    // gE->addGeometry(surface);
    // if boundingbox = on
    if(type=="rectangle") {
        gE->addGeometry(bbox);
    } else if(type == "sphere") {
        gE->addGeometry(surface);
    }
  }
};
void JSONSceneParser::parse_output(RayTracer* gE) {
  for (auto itr = sceneJSONData["output"].begin(); itr!= sceneJSONData["output"].end(); itr++){      
    std::string filename;
    Eigen::Vector3f size(0,0,0), lookat(0,0,0), up(0,0,0), centre(0,0,0), ai(0,0,0), bkc(0,0,0);
    vector<float> centreInput, sizeInput, lookAtInput, upInput, aiInput, bkcInput;
    if((*itr)["centre"] != NULL) {
      centreInput = (*itr)["centre"].get<std::vector<float>>();
      copyInputVector3f(3, centre, centreInput);
    }
    if((*itr)["size"] != NULL) {
      sizeInput = (*itr)["size"].get<std::vector<float>>();
      copyInputVector3f(2, size, sizeInput);
    }
    if((*itr)["lookat"] != NULL) {
      lookAtInput = (*itr)["lookat"].get<std::vector<float>>();
      copyInputVector3f(3, lookat, lookAtInput);
    }
    if((*itr)["up"] != NULL) {
      upInput = (*itr)["up"].get<std::vector<float>>();
      copyInputVector3f(3, up, upInput);
    }
    float fov;
    if((*itr)["fov"] != NULL) {
      fov = (*itr)["fov"].get<float>();
    }
    if((*itr)["bkc"] != NULL) {
      bkcInput = (*itr)["bkc"].get<std::vector<float>>();
      copyInputVector3f(3, bkc, bkcInput);
    }
    if((*itr)["filename"] != NULL) {
        filename = (*itr)["filename"].get<std::string>();
    }
    // Set parameters in camera
    auto newCamera = new Camera();
    newCamera->fov = fov;
    newCamera->centre = centre;
    newCamera->lookat = lookat;
    newCamera->up = up;
    newCamera->size = size;
    newCamera->bkc = bkc;
    newCamera->filename = filename;
    gE->cameraList.push_back(newCamera);
    if((*itr)["ai"] != NULL) {
      aiInput = (*itr)["ai"].get<std::vector<float>>();
      copyInputVector3f(3, ai, aiInput);
      gE->ambientIntensity = ai;
    }
  }
  // Sets the first camera as the default active camera in the scene
  gE->activeSceneCamera = gE->cameraList[0];
};
bool JSONSceneParser::test_parse_geometry() {
//  cout<<"Geometry: "<<endl;
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
      // Get the ac,dc,sc,ka,kd,ks and pc coefficients
      if(itr->contains("ac")) {
        Eigen::Vector3f ac;
        vector<float> ac_input = (*itr)["ac"];
        copyInputVector3f(3, ac, ac_input);
//        cout<<"ac: "<<ac<<endl;
      }
      if(itr->contains("dc")) {
        Eigen::Vector3f dc;
        vector<float> dc_input = (*itr)["dc"];
        copyInputVector3f(3, dc, dc_input);
//        cout<<"dc: "<<dc<<endl;
      }
      if(itr->contains("sc")) {
        Eigen::Vector3f sc;
        vector<float> sc_input = (*itr)["sc"];
        copyInputVector3f(3, sc, sc_input);
//        cout<<"sc: "<<sc<<endl;
      }
      if(itr->contains("ka")) {
        float ka = (*itr)["ka"];
//        cout<<"ka: "<<ka<<endl;
      }
      if(itr->contains("kd")) {
        float kd = (*itr)["kd"];
//        cout<<"kd: "<<kd<<endl;
      }
      if(itr->contains("ks")) {
        float ks = (*itr)["ks"];
//        cout<<"ks: "<<ks<<endl;
      }
      if(itr->contains("pc")) {
        float pc = (*itr)["pc"];
//        cout<<"pc: "<<pc<<endl;
      }
      if(type=="sphere") {
//        cout<<"Sphere: "<<endl;
        Eigen::Vector3f centre(0,0,0);  
        // This makes sure this persists in the game engine, but with weak_ptr
        // TODO: request the engine to allocate for a new geometry (shared_ptr), return it here and then use a copy constructor here or too complicated?      
        auto newSphere = std::make_shared<Sphere>();

        vector<float> input = (*itr)["centre"];
        copyInputVector3f(3, centre, input);
//        cout<<"Centre:\n"<<centre<<endl;
        newSphere->centre = centre;
        float radius;
        try {
          radius = (*itr)["radius"];
//          cout<<"Radius: "<<radius<<endl;
          newSphere->radius = radius;
        } catch(nlohmann::detail::type_error typeError) {
          std::cerr<<"Type error: Geometry radius must be a number! Aborting..."<<endl;
          return false;
        } catch(std::exception e) {
          std::cerr<<"Caught an unhandled error!"<<endl;
          return false;
        }
//        cout<<"Sphere info:"<<endl;
        // cout<<newSphere->centre<<endl;
        // cout<<newSphere->radius<<endl;
      }
      if(type=="rectangle"){
//        std::cout<<"testing rectangle: "<<std::endl;
        std::vector<float> p1_input, p2_input, p3_input, p4_input;
        if((*itr)["p1"] != NULL) {
//          std::cout<<"P1 is null"<<std::endl;
          p1_input = (*itr)["p1"].get<std::vector<float>>();
        }
        if((*itr)["p2"] != NULL) {
//          std::cout<<"P2 is null"<<std::endl;
          p2_input = (*itr)["p2"].get<std::vector<float>>();
        }
        if((*itr)["p3"] != NULL) {
//          std::cout<<"P3 is null"<<std::endl;
          p3_input = (*itr)["p3"].get<std::vector<float>>();
        }
        if((*itr)["p4"] != NULL) {
//          std::cout<<"P4 is null"<<std::endl;
          p4_input = (*itr)["p4"].get<std::vector<float>>();
        }
        Eigen::Vector3f P1(0,0,0);
        Eigen::Vector3f P2(0,0,0);
        Eigen::Vector3f P3(0,0,0);
        Eigen::Vector3f P4(0,0,0);
        copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);
//        cout<<"P1 : "<<P1<<endl;
//        cout<<"P2 : "<<P2<<endl;
//        cout<<"P3 : "<<P3<<endl;
//        cout<<"P4 : "<<P4<<endl;
      }
      ++gc;
  }
//  cout<<"We have: "<<gc<<" objects!"<<endl;
  return true;
};
void JSONSceneParser::parse_lights(RayTracer* gE) {
//  cout<<"Light: "<<endl;
  int lc = 0;
    Light* light;

  // use iterators to read-in array types
    for (auto itr = sceneJSONData["light"].begin(); itr!= sceneJSONData["light"].end(); itr++) {
        std::string type;
        type = (*itr)["type"];

    if(type=="point") {
        Eigen::Vector3f centre,id,is;
        std::vector<float> id_input;
        std::vector<float> is_input;
        vector<float> centre_input;

        if((*itr)["centre"] != NULL) {
        centre_input = (*itr)["centre"].get<std::vector<float>>();
        }
        if((*itr)["id"] != NULL) {
        id_input = (*itr)["id"].get<std::vector<float>>();
        }
        if((*itr)["is"] != NULL) {
        is_input = (*itr)["is"].get<std::vector<float>>();
        }
        copyInputVector3f(3, centre, centre_input);
        copyInputVector3f(3, id, id_input);
        copyInputVector3f(3, is, is_input);
        light = new PointLight(centre,id,is, gE);
    }
    if(type=="area") {
        Eigen::Vector3f P1(0,0,0);
        Eigen::Vector3f P2(0,0,0);
        Eigen::Vector3f P3(0,0,0);
        Eigen::Vector3f P4(0,0,0);
        std::vector<float> p1_input, p2_input, p3_input, p4_input;
        if((*itr)["p1"] != NULL) {
          p1_input = (*itr)["p1"].get<std::vector<float>>();
        }
        if((*itr)["p2"] != NULL) {
          p2_input = (*itr)["p2"].get<std::vector<float>>();
        }
        if((*itr)["p3"] != NULL) {
          p3_input = (*itr)["p3"].get<std::vector<float>>();
        }
        if((*itr)["p4"] != NULL) {
          p4_input = (*itr)["p4"].get<std::vector<float>>();
        }
        copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);
        light = new AreaLight(P1,P2,P3,P4, gE);
    }
    gE->lights.push_back(light);
    ++lc;
  }
//  cout<<"We have: "<<lc<<" objects!"<<endl;
};
bool JSONSceneParser::test_parse_lights() {
//  cout<<"Test Lights: "<<endl;
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
//          cout<<"Point based light: "<<endl;
          Eigen::Vector3f centre,id,is;
          std::vector<float> id_input;
          std::vector<float> is_input;
          vector<float> centre_input = (*itr)["centre"].get<std::vector<float>>();
          copyInputVector3f(3, centre, centre_input);
//          cout<<"Centre:\n"<<centre<<endl;
          if((*itr)["id"] != NULL) {
            id_input = (*itr)["id"].get<std::vector<float>>();
          } else {
              return false;
          }
          if((*itr)["is"] != NULL) {
            is_input = (*itr)["is"].get<std::vector<float>>();
          } else {
              return false;
          }
          copyInputVector3f(3, id, id_input);
          copyInputVector3f(3, is, is_input);
//          cout<<"id:\n"<<id<<endl;
//          cout<<"is:\n"<<is<<endl;
      }
      if(type=="area") {
//        cout<<"Area-based light: "<<endl;
        std::vector<float> p1_input, p2_input, p3_input, p4_input;
        if((*itr)["p1"] != NULL) {
            p1_input = (*itr)["p1"].get<std::vector<float>>();
        } else {
            cout<<"p1 Is Null"<<endl;
            return false;
        }
        if((*itr)["p2"] != NULL) {
            p2_input = (*itr)["p2"].get<std::vector<float>>();
        } else {
            cout<<"p2 Is Null"<<endl;
            return false;
        }
        if((*itr)["p3"] != NULL) {
            p3_input = (*itr)["p3"].get<std::vector<float>>();
        } else {
            cout<<"p3 Is Null"<<endl;
            return false;
        }
        if((*itr)["p4"] != NULL) {
            p4_input = (*itr)["p4"].get<std::vector<float>>();
        } else {
            cout<<"p4 Is Null"<<endl;
            return false;
        }
        Eigen::Vector3f P1(0,0,0);
        Eigen::Vector3f P2(0,0,0);
        Eigen::Vector3f P3(0,0,0);
        Eigen::Vector3f P4(0,0,0);
        copyRectangleCorners(P1,P2,P3,P4,p1_input,p2_input,p3_input,p4_input);
      }
      ++lc;
   }
//  cout<<"We have: "<<lc<<" objects!"<<endl;
  return true;
};
bool JSONSceneParser::test_parse_output() {
//  cout<<"Outputs: "<<endl;
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
      vector<float> centreInput, sizeInput, lookAtInput, upInput;
      Eigen::Vector3f size(0,0,0), lookat(0,0,0), up(0,0,0), centre(0,0,0);
      if((*itr)["centre"] != NULL) {
          centreInput = (*itr)["centre"].get<std::vector<float>>();
      } else {
          cout<<"centre Is Null"<<endl;
          return false;
      }
      if((*itr)["size"] != NULL) {
          sizeInput = (*itr)["size"].get<std::vector<float>>();
      } else {
          cout<<"size Is Null"<<endl;
          return false;
      }
      if((*itr)["lookat"] != NULL) {
          lookAtInput = (*itr)["lookat"].get<std::vector<float>>();
      } else {
          cout<<"lookat Is Null"<<endl;
          return false;
      }
      if((*itr)["up"] != NULL) {
          upInput = (*itr)["up"].get<std::vector<float>>();
      } else {
          cout<<"up Is Null"<<endl;
          return false;
      }
      copyInputVector3f(3, centre, centreInput);
      copyInputVector3f(3, lookat, lookAtInput);     
      copyInputVector3f(3, up, upInput);            
      copyInputVector3f(2, size, sizeInput);      

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
//      cout<<"B"<<endl;
//      cout<<"Filename: "<<filename<<endl;
//      cout<<"Size:\n"<<size<<endl;
//      cout<<"Camera centre:\n"<<centre<<endl;
//      cout<<"FOV:\n"<<fov<<endl;
//      cout<<"lookat:\n"<<lookat<<endl;
//      cout<<"up:\n"<<up<<endl;
      ++lc;
  }
//  cout<<"We have: "<<lc<<" objects!"<<endl;
  return true;
};
