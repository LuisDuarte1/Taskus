#define _USE_MATH_DEFINES
#include <cmath>

#include "TaskusDebugUtils.h"

struct EllipsePosition{
    uint16_t x;
    uint16_t y;
};


namespace Taskus{

    void InsertEllipseText(std::ofstream & stream, EllipsePosition pos, Task * task, std::function<std::string(Task*)> & taskNameFuncion){
        std::string text = taskNameFuncion(task);
        stream << 
        "<g transform=\"translate("<< pos.x << ","<< pos.y << ")\">"
        "<ellipse rx=\""<< SVG_RADIUS_X << "\" ry=\""<< SVG_RADIUS_Y << "\" fill=\"rgb(255, 255, 255)\" stroke=\"rgb(0, 0, 0)\"/>"
        "<text dominant-baseline=\"middle\" text-anchor=\"middle\" font-size=\"20\">"
        << text <<
        "</text>"
        "</g>";

    }
    void AddArrow(std::ofstream & stream, EllipsePosition tPos, EllipsePosition dPos){
        //this will calculate what is the best starting point and end point for two tasks that are ellipses we will brute force this step because it's easier than
        //to come up with a formula,which would be faster but takes more time to develop

        //we will use a "binary search"-like method aka we will choose a starting point for both ellipses and check left or right on the start elipse the one 
        //direction that gets lesser distance is the one we will choose, the loop will stop when the distance increases again 
        double step = 0.1;
        double angle = M_PI_2;
        double startX = tPos.x + SVG_RADIUS_X*cos(angle);
        double startY = tPos.y + SVG_RADIUS_Y*sin(angle);


        //parametric equation for a ellipse
        EllipsePosition endPos = {dPos.x + static_cast<uint16_t>(SVG_RADIUS_X*cos(- M_PI_2)), dPos.y + static_cast<uint16_t>(SVG_RADIUS_Y*sin(- M_PI_2))};
        
        double dist = sqrt((startX - endPos.x)*(startX - endPos.x) + (startY - endPos.y)*(startY - endPos.y));
        bool direction = false; //false goes right, true goes left
        bool failed = false; //this can fail once and then breaks
        while (true)
        {
            if(direction)
                angle += step;
            else
                angle -= step; 
            
            double newX = tPos.x + SVG_RADIUS_X*cos(angle);
            double newY = tPos.y + SVG_RADIUS_Y*sin(angle);
            double newDist = sqrt((newX - endPos.x)*(newX - endPos.x) + (newY - endPos.y)*(newY - endPos.y));
            if(newDist < dist){
                startX = newX;
                startY = newY;
            } else{
                if(!failed){
                    failed = true;
                    direction = true;
                }
                else break;
            }

        }

        stream << "<g>"
            "<defs>"
            "<marker id=\"arrowhead\" markerWidth=\"10\" markerHeight=\"7\" "
            "refX=\"0\" refY=\"3.5\" orient=\"auto\">"
            "<polygon points=\"0 0, 10 3.5, 0 7\" />"
            "</marker>"
            "</defs>"
            "<line x1=\""<<(uint16_t) startX<<"\" y1=\""<<(uint16_t) startY <<"\" x2=\""<< endPos.x <<"\" y2=\""<< endPos.y <<"\" stroke=\"#000\" "
            "stroke-width=\"3\" marker-end=\"url(#arrowhead)\" /></g>";
    }

    bool SaveDebugTaskFlowSVG(std::string filename, Task * initalTask, std::function<std::string(Task*)> & taskNameFuncion){
        std::ofstream output_file(filename);
        //first write the beginning of the file 
        output_file << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" style=\"background-color: rgb(255, 255, 255);\"><defs/><g>";

        //this maps assumes that the Task does not get removed in the meantime, which can cause some bugs
        std::map<Task*, EllipsePosition> foundTasks;

        std::multimap<Task*, Task*> connections;

        static std::function<void(Task*)> traverse_tasks = [&](Task * task){
            EllipsePosition tPos;
            if(task->dependenciesTasks.size() == 0){ //means that this is the root of the TaskFlow
                foundTasks.insert({task, {SVG_RADIUS_X,SVG_RADIUS_Y}});
                tPos = {SVG_RADIUS_X, SVG_RADIUS_Y};
                InsertEllipseText(output_file, tPos, task, taskNameFuncion);
            } else{
                tPos = foundTasks.find(task)->second;
            }
            if(task->dependentTasks.size() == 0) return; //check for one possible end for the TaskFlow
            //add dependentTasks if they don't exist yet
            for(size_t i = 0; i < task->dependentTasks.size(); i++){
                EllipsePosition dPos;
                if(foundTasks.find(task->dependentTasks[i]) == foundTasks.end()){
                dPos = {static_cast<uint16_t>(tPos.x+(SVG_OFFSET_BETWEEN_TASKS_X*i)),
                    static_cast<uint16_t>(tPos.y+SVG_OFFSET_BETWEEN_TASKS_Y)};
                foundTasks.insert({task->dependentTasks[i], dPos});
                
                InsertEllipseText(output_file, dPos, task->dependentTasks[i], taskNameFuncion);
                
                } else {
                    dPos = foundTasks.find(task->dependentTasks[i])->second;
                }
                //add arrow if necessary
                auto range = connections.equal_range(task);
                if(range.first == connections.end()){ //first to be added
                    connections.insert({task, task->dependentTasks[i]});
                    AddArrow(output_file, tPos, dPos);
                } else{
                    bool found = false;
                    for(auto e = range.first; e != range.second; e++){
                        if(e->second == task->dependentTasks[i]){
                            found = true;
                            break;
                        }
                    }
                    if(!found){
                        connections.insert({task, task->dependentTasks[i]});
                        AddArrow(output_file, tPos, dPos);
                    }

                }

                traverse_tasks(task->dependentTasks[i]);
            }

        };

        traverse_tasks(initalTask);

        //end of file
        output_file << "</g><switch><g requiredFeatures=\"http://www.w3.org/TR/SVG11/feature#Extensibility\"/><a transform=\"translate(0,-5)\" xlink:href=\"https://www.diagrams.net/doc/faq/svg-export-text-problems\" target=\"_blank\"><text text-anchor=\"middle\" font-size=\"10px\" x=\"50%\" y=\"100%\">Text is not SVG - cannot display</text></a></switch></svg>";

        if (output_file.good()){
            output_file.close();
            return true;
        }
        return false;
    }
}