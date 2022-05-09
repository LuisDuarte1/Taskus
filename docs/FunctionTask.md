# Function Task

The Function Task is intended for a quick use for the developer when it needs to create a Task quickly and it doesn't need to be very performant about it. It's really useful for one time functions.

There are some restritions on it tho:

- We can't sequence Function Tasks that depend of a result from the last one, at least in a easy automatic way. It's really easy to solve this for 2 Function Tasks, but not N function tasks without restricting even more the use of Task Functions. So the procedure to doing something like this is:
    - Create Function Task
    - Start Function Task
    - Wait for it to end
    - Collect result
    - Create the next Function Task...

This is possible to run, because the Task Pool, unless specified, will create more threads than logical processors that the processor has to allow for the OS scheduler to switch and allow other Tasks execution while waiting.

- `std::vector` of size 0 in the arguments are not valid. It doesn't make much sense considering they give the amount of times the function has to run.

- Multiple `std::vector` that have different sizes don't make much sense either (because how is Taskus going to know how many times the function has to run?).