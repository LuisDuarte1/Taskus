#ifndef TASKUS_DEBUG_UTILS_H
#define TASKUS_DEBUG_UTILS_H

#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <map>
#include "TaskusTask.h"


namespace Taskus{

    //constants for the svg
    #define SVG_RADIUS_X 60
    #define SVG_RADIUS_Y 40
    #define SVG_TEXT_OFFSET 4 
    #define SVG_OFFSET_BETWEEN_TASKS_X 400 //this offset is used when there are multiple dependantTasks 
    #define SVG_OFFSET_BETWEEN_TASKS_Y 200
    
    static std::function<std::string(Task*)>  _defaultTaskNameFunction = [](Task* task){static int i = 0; std::string s = "Task "+ std::to_string(i); i++; return s;};

    bool SaveDebugTaskFlowSVG(std::string filename, Task * initalTask, std::function<std::string(Task*)> & taskNameFuncion = _defaultTaskNameFunction);
};

#endif