////////////////////////
/**
 * 
 * Author: Sylvain Tran
 * Program Description: The driver for the raytracer.
 * Date Created: Tue, Jan. 11th, 2022
 * Date Last Updated: Tue, Jan. 11th, 2022
 * 
 */
////////////////////////
#ifdef COURSE_SOLUTION
#include "src/RayTracer.h"
#endif

#ifdef STUDENT_SOLUTION
#include "src/RayTracer.h"
#endif

#include <iostream>
#include <string>
#include <memory>

#include "external/json.hpp"
#include "external/simpleppm.h"

// My code
#include "src/GraphicsEngine.h"
using namespace std;

int test_eigen();
int test_save_ppm();
int test_json(nlohmann::json& j);

void runDummyRaytracer() {
    cout<<"Invalid number of arguments"<<endl;
    cout<<"Usage: ./raytracer [scene] "<<endl;
    cout<<"Run sanity checks"<<endl;    
    test_eigen();
    test_save_ppm();
};

int main(int argc, char* argv[])
{
    if(argc!=2){
        runDummyRaytracer();
        
    } else {        
        GraphicsEngine gE;
        nlohmann::json j = gE.validateSceneJSONData(argv[1]);
        if(j == NULL) {
            cout<<"validateSceneJSONData has returned false: one of the tests failed."<<endl;
            return -1;
        }
        
#ifdef COURSE_SOLUTION
        cout<<"Running course solution"<<endl;
        RT371::RayTracer<RT371::Kernelf> rt(j);
        cout<<"Running!"<<endl;
        rt.run();
#else
        
#ifdef STUDENT_SOLUTION
        cout<<"Running studnt solution"<<endl;
        RayTracer rt(j);
        rt.run();
#else
        // GIven code - a bunch of test functions to showcase the funcitonality
        test_eigen();
        test_save_ppm();
        
        if(test_json(j)==0){
            
        } else {
            cout<<"Could not load file!"<<endl;
        }
#endif 
#endif
    }    
    return 0;
}

