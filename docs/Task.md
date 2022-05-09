# Task


## Repeatable Tasks

A task flow can be repeatable and is activated with is_repeatable variable **ONLY** on the root task (if it's not a branch task).

What will happen is that when adding the Task Flow, the Task Pool with add a *repeatTask* following the last task of the Task Flow.



The is_repeatable can be set to false again, but only getting effect on the next iteration of the Task Flow.


## Mutations


The need for mutations comes from scaling up and scaling down resources as needed in runtime. It's more of a niche case but it doesnt add much overhead by implementing it.

Mutations need the *isMutation* variable set as true.

Although we don't have any way to check it, we do any code related to mutation in the tryMutate function. The mutation will only apply on the next iteration of the loop, so the Task Flow must be repeatable for mutations to even take effect.

Any mutations we want to add we put it in the `std::vector` mutationAddTask. 
Any mutations we want to remove we put it in the `std::vector` mutationRemoveTask.


## Profiling

Taskus will always try to measure the time of execute on each runTaskFunction and store the last 1000 runs. (for instance it allows for a simpler implementation of scaling back or scaling up of mutations on a Task, and it doesn't add much overhead anyways).

It's not yet implemented yet but it will allow for creating a tool that can profile Tasks and how much time they take (with and without Taskus overhead).