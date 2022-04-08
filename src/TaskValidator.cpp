#include "TaskValidator.h"


namespace Taskus{
    ValidationResultEnum checkMutationValid(Task * task, std::vector<Task *> 
    dependants){
        //this only check for mutation related validations
        if (!task->isMutation) return MUTATION_NOT_MARKED_AS_ONE;

        if(task->dependentTasks.size() == 0) return MUTATION_DOEST_LEAD_TO_DEPENDANT;

        if(task->mutationAddTask.size() != 0 || task->mutationRemoveTask.size() != 0) return MUTATION_HAS_MUTATIONS_TO_ADD_OR_REMOVE;

        bool foundAllOriginalDependants = true;
        for(int i = 0; i < dependants.size(); i++){
            bool foundelement = false;
            for(int e = 0; e < task->dependentTasks.size(); e++){
                if(task->dependentTasks[e] == dependants[i]) {
                    foundelement = true;
                    break;
                }
            }
            foundAllOriginalDependants &= foundelement;
        }
        //there are two cases:
        //  if foundAlloriginaldependants == true means that the task is final in the mutator sense
        //  if not, recurse through any task
        if(!foundAllOriginalDependants){
            for(int i = 0; i < task->dependentTasks.size(); i++){
                ValidationResultEnum vv = checkMutationValid(task->dependentTasks[i], dependants);
                if(vv != VALIDATION_PASSED){
                    return vv;
                }
            }
        }
        return VALIDATION_PASSED;
    }

    ValidationResultEnum ValidateTask(Task * task, int depth){
        if(task->isRepeatable && depth != 0) return REPEATABLE_NOT_ROOT;
        if(task->getDependenciesSize() == 0 && depth != 0) return NO_DEPENDENCIES_NOT_ROOT;
        if(task->getDependenciesSize() != 0 && depth == 0) return DEPENDENCIES_AT_ROOT;
        if(task->mutationAddTask.size() != 0 && task->dependentTasks.size() == 0) return MUTATION_IN_FINAL_TASKS;
        if(task->mutationRemoveTask.size() != 0 && task->dependentTasks.size() == 0) return MUTATION_IN_FINAL_TASKS;
        if(task->mutationAddTask.size() != 0){
            for(int i = 0; i < task->mutationAddTask.size(); i++){
                ValidationResultEnum v = checkMutationValid(task->mutationAddTask[i], task->dependentTasks);
                if(v != VALIDATION_PASSED) return v; 
            }
            for(int i = 0; i < task->mutationRemoveTask.size(); i++){
                ValidationResultEnum v = checkMutationValid(task->mutationRemoveTask[i], {});
                if(v != VALIDATION_PASSED) return v; 
            }      
        }  
        //now we check for dependents
        for(int i = 0; i < task->dependentTasks.size(); i++){
            int new_depth = depth + 1;
            ValidationResultEnum r_leafs = ValidateTask(task->dependentTasks[i], new_depth);
            if(r_leafs != VALIDATION_PASSED){
                //if a mistake is spotted in any of the depedent tasks, return 
                //(which means it will not check for all the tree of tasks and will stop at the first error)
                return r_leafs;
            }
        }
        return VALIDATION_PASSED;


    }

}