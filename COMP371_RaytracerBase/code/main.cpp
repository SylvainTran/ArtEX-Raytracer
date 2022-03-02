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

// Standard
#include <iostream>
#include <string>
#include <memory>
using std::cout;
using std::endl;

// External
#include "external/json.hpp"
#include "external/simpleppm.h"

// My code
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
        std::ifstream t(argv[1]);
        if(!t) {
          cout<<"File "<<argv[1]<<" does not exist!"<<endl; 
          return -1;
        }

        std::stringstream buffer;
        buffer<<t.rdbuf();
        nlohmann::json j = nlohmann::json::parse(buffer.str());
        cout<<"Parsed successfully"<<endl;

#ifdef COURSE_SOLUTION
        cout<<"Running course solution"<<endl;
        RT371::RayTracer<RT371::Kernelf> rt(j);
        cout<<"Running!"<<endl;
        rt.run();
#else
        
#ifdef STUDENT_SOLUTION
        cout<<"Running student solution"<<endl;
        RayTracer rt = RayTracer(j);
        rt.run();
#else
        // Given code - a bunch of test functions to showcase the funcitonality
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

