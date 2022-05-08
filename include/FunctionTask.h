#ifndef TASKUS_FUNCTION_TASK
#define TAKSUS_FUNCTION_TASK


#include <iostream>
#include <tuple>
#include <vector>
#include <any>
#include <cassert>
#include <functional>
#include <mutex>
#include "TaskusTask.h"
#include "internal_tasks/placeholderTask.h"
#include "functiontaskutils/functionTaskUtil.h"

#define DEFAULT_SIZE_PER_TASK 10000

/*
    FIXME: all functions will be defined here because I can't get the functions to be defined in a seperate
    .cpp file to better organize this, I think it's because the function is defined by a template.
*/

namespace Taskus{    

    struct noReturn{};

    class BaseFunctionClass{
        public:
            virtual ~BaseFunctionClass(){

            };

        private:
            std::string lmao = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";
    };

    template<typename ResultType>
    class EndFunctionTask : public Task{
        public:
            EndFunctionTask(std::vector<std::vector<ResultType>*> nfunctionResults, std::vector<ResultType> *  nfinalResult, std::vector<std::mutex *> nfunctionMutexes) : Task(){
                functionResults = nfunctionResults;
                finalResult = nfinalResult;
                functionMutexes = nfunctionMutexes;

                assert(functionResults.size() == functionMutexes.size());
            }

            void tryMutate(){

            }

            void runTaskFunction(){
                for(int i = 0; i < functionResults.size(); i++){
                    functionMutexes[i]->lock();
                    for(int e = 0; e < functionResults[i]->size(); e++){
                        (*finalResult).push_back((*functionResults[i])[e]);
                    }
                    functionMutexes[i]->unlock();

                }
            }
        
        private:
            std::vector<std::vector<ResultType>*>  functionResults;
            std::vector<std::mutex *> functionMutexes;
            std::vector<ResultType> * finalResult;
    };

    template<typename ResultType, class Func, typename ... Args>
    class DividedFunctionTask : public Task{
        public:
            DividedFunctionTask(Func & func, std::vector<ResultType> & finalVector, std::tuple<Args...> nargs) : function(func), Task(){
                taskArguments = nargs;
                
                bool valid = true;
                doSomethingToAllElements([&valid]<typename T>(T & t){
                    if(!FunctionTaskUtils::is_argumentToDividedTask<T>::value) valid = false;
                });

                assert(valid);
                functionResult = new std::vector<ResultType>();

            }
            std::vector<ResultType> * functionResult;
            std::mutex functionMutex;

            void tryMutate(){

            }

            size_t getSizeOfVectorArgument(){
                size_t r = 0;
                doSomethingToAllElements([&r]<typename T>(T & t){
                    if (r != FunctionTaskUtils::getSizeOfArgument(t)) r = FunctionTaskUtils::getSizeOfArgument(t);
                });
                return r;

            }

            void runTaskFunction(){
                size_t t = getSizeOfVectorArgument();
                functionMutex.lock();
                for(int i = 0; i < t; i++){
                    (*functionResult).push_back(runFunctionToAllElements(function));
                }
                functionMutex.unlock();
            }

        private:
            Func function;
            std::tuple<Args...>  taskArguments;



            //this must return a pointer just in case we want to mutate the tuple itself
            //(even though std::get returns a reference but it seems that auto removes it?)
            //so we force it through by returning a pointer            
            template<size_t Num>
            auto* getElementOfTuple(){
                return &(std::get<Num>(taskArguments));
            }

            template<size_t Num>
            decltype(auto) getPointerOfArgumentList(){
                auto t_element = getElementOfTuple<Num>();
                auto r = (*t_element).t;
                if((*t_element).is_vec){
                    (*t_element).t++; 
                    (*t_element).current_offset += 1;
                }
                return r;
            }

            template<class F, size_t ... Indexes>
            ResultType runFunctionToCertainElements(F && functor, std::index_sequence<Indexes...>){
                return std::invoke(functor, *getPointerOfArgumentList<Indexes>()...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(taskArguments)>>
            ResultType runFunctionToAllElements(F && functor){
                return runFunctionToCertainElements(functor, std::make_index_sequence<Index>{});
            }


            template<class F, size_t... Indexes>
            void doSomethingToCertainElements(F && functor, std::index_sequence<Indexes...>){
                (std::invoke(functor, (*getElementOfTuple<Indexes>())), ...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(taskArguments)>>
            void doSomethingToAllElements(F && functor){
                doSomethingToCertainElements(functor, std::make_index_sequence<Index>{});
            }
    };






    template<typename ResultType, class Func ,typename... Args>
    class FunctionTask : public BaseFunctionClass{
        /*
            In relation to arguments, if a argument is a vector, then it will assumed that it is 
            actually multiple arguments in different calls. If there are multiple vectors that don't have
            size 1 and are not a vector of a vector, they have to be the same size (same function calls).
            If the argument is not a vector (or by exception,
            a vector (of size 1) of a vector ), it is constant throughout the function call.

            The arguments in the lambda func MUST be in the same order that the arguments are given,
            otherwise the compiler will not like it.
        */
        public:

            FunctionTask(const Func& func, Args ... a) : functionToCall(func){

                arguments = std::make_tuple(std::forward<Args>(a)...);
                std::vector<size_t> vector_counts;
                doSomethingToAllElements([&vector_counts]<typename T>(T& t){
                        if(FunctionTaskUtils::getSizeOfVector(t) != size_t(-1)){
                            vector_counts.push_back(FunctionTaskUtils::getSizeOfVector(t));
                        }
                    });
                size_t last_size = 0;
                for(size_t i : vector_counts){
                    //there is no point to having a argument vector with no elements, so it's illegal to do it
                    if(i == 0){
                        throw new std::runtime_error("The vector argument in FunctionTask has no elements.");
                    }
                    if(last_size == 0){
                        last_size = i;
                        continue;
                    }
                    if(last_size != i){
                        throw new std::runtime_error("It seems that the vector arguments don't have the same size, which makes dividing ambiguous.");
                    }
                };
                totalArguments = last_size;
            };

            ~FunctionTask(){
                for(Task * t: tasksToRun) delete t;
                delete reduceTask;
                delete startTask;
            }

        inline void changeArgumentsPerTask(size_t numargs){maxArgumentsPerTask = numargs;};

        void divideTasks(){
            int rest = totalArguments % maxArgumentsPerTask;
            int dividable = totalArguments - rest;
            int timestodivide = dividable / maxArgumentsPerTask;
            int offset = 0;
            size_t max = maxArgumentsPerTask;

            std::vector<std::mutex *> mutexes;
            for(int i = 0; i < timestodivide; i++){
                //std::any shouldn't be used but it's the only way that 
                //I could "unpack the tuple" to give it to a task
                auto t = doAndGetAllElementsInTuple([ offset, max]<typename T>(T & t)
                {
                    return std::make_tuple(FunctionTaskUtils::divideTask(t, max, offset));
                });
                //create task and append
                auto task = new DividedFunctionTask(functionToCall, result, t);
                tasksToRun.push_back(task);
                resultFromTasks.push_back(task->functionResult);
                mutexes.push_back(&task->functionMutex);
                offset += maxArgumentsPerTask;
            }
            if(rest != 0){
                auto t = doAndGetAllElementsInTuple([ offset, rest]<typename T>(T & t)
                {
                    return std::make_tuple(FunctionTaskUtils::divideTask(t, rest, offset));
                });
                //create task and append
                auto task = new DividedFunctionTask(functionToCall, result, t);
                tasksToRun.push_back(task);
                resultFromTasks.push_back(task->functionResult);
                mutexes.push_back(&task->functionMutex);
                offset += rest;
            };
            //check on debug builds if every element was included
            assert(offset == totalArguments);

            reduceTask = new EndFunctionTask(resultFromTasks, &result, mutexes);

            startTask = new placeholderTask();

            for(Task * task: tasksToRun){
                (*startTask) += task;
                (* task) += reduceTask;
            }
        }

        inline size_t getExpectedResultSize(){return totalArguments;};

        placeholderTask * startTask;
        EndFunctionTask<ResultType> * reduceTask; 

        std::vector<ResultType> result;

        private:



            std::vector<std::vector<ResultType>*> resultFromTasks;

            


            std::vector<Task*> tasksToRun;

            

            Func functionToCall;

            size_t totalArguments;

            //this applies to dividing the task when there are vectors
            size_t maxArgumentsPerTask = DEFAULT_SIZE_PER_TASK;

            //we hide away the variadic recursive meta programming inside the tuple and is initilized 
            //with make_tuple and forward to expand the arguments  
            std::tuple<Args...> arguments;



            template<size_t Num>
            decltype(auto) getElementOfTuple(){
                return std::get<Num>(arguments);
            }

            //in this case F MUST always return a tuple
            template<class F, size_t ... Indexes>
            decltype(auto) doAndGetCertainElementsInTuple(F && functor, std::index_sequence<Indexes...>){
                return std::tuple_cat(std::invoke(functor, getElementOfTuple<Indexes>()) ...);
            } 

            template<class F, size_t Index = std::tuple_size_v<decltype(arguments)>>
            decltype(auto) doAndGetAllElementsInTuple(F && functor){
                return doAndGetCertainElementsInTuple(functor, std::make_index_sequence<Index>{});

            }


            template<class F, size_t... Indexes>
            void doSomethingToCertainElements(F && functor, std::index_sequence<Indexes...>){
                (std::invoke(functor, getElementOfTuple<Indexes>()), ...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(arguments)>>
            void doSomethingToAllElements(F && functor){
                doSomethingToCertainElements(functor, std::make_index_sequence<Index>{});
            }

    };


    template<class Func ,typename... Args>
    class FunctionTask<noReturn, Func, Args...> : public BaseFunctionClass{
        /*
            In relation to arguments, if a argument is a vector, then it will assumed that it is 
            actually multiple arguments in different calls. If there are multiple vectors that don't have
            size 1 and are not a vector of a vector, they have to be the same size (same function calls).
            If the argument is not a vector (or by exception,
            a vector (of size 1) of a vector ), it is constant throughout the function call.

            The arguments in the lambda func MUST be in the same order that the arguments are given,
            otherwise the compiler will not like it.
        */
        public:

            FunctionTask(const Func& func, Args ... a) : functionToCall(func){
                arguments = std::make_tuple(std::forward<Args>(a)...);
                std::vector<size_t> vector_counts;
                doSomethingToAllElements([&vector_counts]<typename T>(T& t){
                        if(FunctionTaskUtils::getSizeOfVector(t) != size_t(-1)){
                            vector_counts.push_back(FunctionTaskUtils::getSizeOfVector(t));
                        }
                    });
                size_t last_size = 0;
                for(size_t i : vector_counts){
                    //there is no point to having a argument vector with no elements, so it's illegal to do it
                    if(i == 0){
                        throw new std::runtime_error("The vector argument in FunctionTask has no elements.");
                    }
                    if(last_size == 0){
                        last_size = i;
                        continue;
                    }
                    if(last_size != i){
                        throw new std::runtime_error("It seems that the vector arguments don't have the same size, which makes dividing ambiguous.");
                    }
                };
                totalArguments = last_size;
            };

            ~FunctionTask(){
                for(Task * t: tasksToRun) delete t;
                delete reduceTask;
                delete startTask;
            }

        inline void changeArgumentsPerTask(size_t numargs){maxArgumentsPerTask = numargs;};

        void divideTasks(){
            int rest = totalArguments % maxArgumentsPerTask;
            int dividable = totalArguments - rest;
            int timestodivide = dividable / maxArgumentsPerTask;
            int offset = 0;
            size_t max = maxArgumentsPerTask;

            std::vector<std::mutex *> mutexes;
            for(int i = 0; i < timestodivide; i++){
                //std::any shouldn't be used but it's the only way that 
                //I could "unpack the tuple" to give it to a task
                auto t = doAndGetAllElementsInTuple([ offset, max]<typename T>(T & t)
                {
                    return std::make_tuple(FunctionTaskUtils::divideTask(t, max, offset));
                });
                //create task and append
                auto task = new DividedFunctionTask(functionToCall, result, t);
                tasksToRun.push_back(task);
                offset += maxArgumentsPerTask;
            }
            if(rest != 0){
                auto t = doAndGetAllElementsInTuple([ offset, rest]<typename T>(T & t)
                {
                    return std::make_tuple(FunctionTaskUtils::divideTask(t, rest, offset));
                });
                //create task and append
                auto task = new DividedFunctionTask(functionToCall, result, t);
                tasksToRun.push_back(task);
                offset += rest;
            };
            //check on debug builds if every element was included
            assert(offset == totalArguments);

            reduceTask = new placeholderTask();

            startTask = new placeholderTask();

            for(Task * task: tasksToRun){
                (*startTask) += task;
                (* task) += reduceTask;
            }
        }

        inline size_t getExpectedResultSize(){return totalArguments;};

        placeholderTask * startTask;
        placeholderTask * reduceTask; 

        std::vector<noReturn> result;

        private:            


            std::vector<Task*> tasksToRun;

            

            Func functionToCall;

            size_t totalArguments;

            //this applies to dividing the task when there are vectors
            size_t maxArgumentsPerTask = DEFAULT_SIZE_PER_TASK;

            //we hide away the variadic recursive meta programming inside the tuple and is initilized 
            //with make_tuple and forward to expand the arguments  
            std::tuple<Args...> arguments;



            template<size_t Num>
            decltype(auto) getElementOfTuple(){
                return std::get<Num>(arguments);
            }

            //in this case F MUST always return a tuple
            template<class F, size_t ... Indexes>
            decltype(auto) doAndGetCertainElementsInTuple(F && functor, std::index_sequence<Indexes...>){
                return std::tuple_cat(std::invoke(functor, getElementOfTuple<Indexes>()) ...);
            } 

            template<class F, size_t Index = std::tuple_size_v<decltype(arguments)>>
            decltype(auto) doAndGetAllElementsInTuple(F && functor){
                return doAndGetCertainElementsInTuple(functor, std::make_index_sequence<Index>{});

            }


            template<class F, size_t... Indexes>
            void doSomethingToCertainElements(F && functor, std::index_sequence<Indexes...>){
                (std::invoke(functor, getElementOfTuple<Indexes>()), ...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(arguments)>>
            void doSomethingToAllElements(F && functor){
                doSomethingToCertainElements(functor, std::make_index_sequence<Index>{});
            }

    };


    template<class Func, typename ... Args>
    class DividedFunctionTask<noReturn, Func, Args ...> : public Task{
        public:
            DividedFunctionTask(Func & func, std::vector<noReturn> & finalVector, std::tuple<Args...> nargs) : function(func), Task(){
                taskArguments = nargs;
                
                bool valid = true;
                doSomethingToAllElements([&valid]<typename T>(T & t){
                    if(!FunctionTaskUtils::is_argumentToDividedTask<T>::value) valid = false;
                });

                assert(valid);
            }

            void tryMutate(){

            }

            size_t getSizeOfVectorArgument(){
                size_t r = 0;
                doSomethingToAllElements([&r]<typename T>(T & t){
                    if (r != FunctionTaskUtils::getSizeOfArgument(t)) r = FunctionTaskUtils::getSizeOfArgument(t);
                });
                return r;

            }

            void runTaskFunction(){
                size_t t = getSizeOfVectorArgument();
                for(int i = 0; i < t; i++){
                    runFunctionToAllElements(function);
                }
            }

        private:
            Func function;
            std::tuple<Args...>  taskArguments;



            //this must return a pointer just in case we want to mutate the tuple itself
            //(even though std::get returns a reference but it seems that auto removes it?)
            //so we force it through by returning a pointer            
            template<size_t Num>
            auto* getElementOfTuple(){
                return &(std::get<Num>(taskArguments));
            }

            template<size_t Num>
            decltype(auto) getPointerOfArgumentList(){
                auto t_element = getElementOfTuple<Num>();
                auto r = (*t_element).t;
                if((*t_element).is_vec){
                    (*t_element).t++; 
                    (*t_element).current_offset += 1;
                }
                return r;
            }

            template<class F, size_t ... Indexes>
            void runFunctionToCertainElements(F && functor, std::index_sequence<Indexes...>){
                std::invoke(functor, *getPointerOfArgumentList<Indexes>()...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(taskArguments)>>
            void runFunctionToAllElements(F && functor){
                runFunctionToCertainElements(functor, std::make_index_sequence<Index>{});
            }


            template<class F, size_t... Indexes>
            void doSomethingToCertainElements(F && functor, std::index_sequence<Indexes...>){
                (std::invoke(functor, (*getElementOfTuple<Indexes>())), ...);
            }

            template<class F, size_t Index = std::tuple_size_v<decltype(taskArguments)>>
            void doSomethingToAllElements(F && functor){
                doSomethingToCertainElements(functor, std::make_index_sequence<Index>{});
            }
    };

    template<typename ResultType, class Func, typename ... Args>
    auto * makeFunctionTask(Func && functor, Args ... args){
        return new FunctionTask<ResultType, Func, Args ...>(functor, args...);
    }

}

#endif