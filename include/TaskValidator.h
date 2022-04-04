#ifndef TASK_VALIDATOR_H
#define TASK_VALIDATOR_H


#include <iostream>
#include "TaskusTask.h"


namespace Taskus{

    enum ValidationResultEnum{
        VALIDATION_PASSED,
        REPEATABLE_NOT_ROOT,
        NO_DEPENDENCIES_NOT_ROOT,
        DEPENDENCIES_AT_ROOT,
        MUTATION_DOEST_LEAD_TO_DEPENDANT,
        MUTATION_NOT_MARKED_AS_ONE,
        MUTATION_IN_FINAL_TASKS,
        MUTATION_HAS_MUTATIONS_TO_ADD_OR_REMOVE
    };

    ValidationResultEnum ValidateTask(Task * task, int depth=0);

}

#endif