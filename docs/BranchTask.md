# Branch Task

The branch Task serves to branch a Task Flow instead of creating a new one alltogether.

They have some interesting properties:

- They can be repeatable not only when they are in the root Task. Which means we can sort of do a for loop in the Task Flow itself. It's up to the Branch Task itself to disable the repeatable flag and stop the inner loop. When the branch task is repeatable and the root task is also repeatable, the branch task will always run until false and then the Task Flow will eventually repeat.

- They can have as many branches as you want (until the integer limit, but that's too many anyway) and allows for less branch tasks (allthough more checking in that branch task but that's a really small thing compared to the normal overhead of Taskus) and therefore more organized code and a lot less overhead.


We can't use the += operator on adding branch tasks into it because we can't actually have anything after a branch task, the corresponding branch will be added at runtime.
