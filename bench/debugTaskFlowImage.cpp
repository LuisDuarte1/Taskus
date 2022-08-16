#include <iostream>
#include <Taskus.h>



int main(){
    //build a task flow
    Taskus::placeholderTask t;
    Taskus::placeholderTask t1;
    Taskus::placeholderTask t2;

    t += &t1;
    t += &t2;
    
    Taskus::placeholderTask tend;

    t1 += &tend;
    t2 += &tend;

    Taskus::SaveDebugTaskFlowSVG("output.svg", &t);
    

}