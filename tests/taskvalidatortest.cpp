#include <gtest/gtest.h>
#include <Taskus.h>
#include <iostream>


class test_repeatable : public Taskus::Task{
        public:
            test_repeatable() : Task(){
                isRepeatable = true;
            }
            void foo(){
                std::cout << "Something \n";
            }
            void runTaskFunction(){
                foo();            
                return;
            }
            void tryMutate(){
                foo();
                return;
            }

};

class test_dependencies : public Taskus::Task{
        public:
            void runTaskFunction(){           
                return;
            }
            void tryMutate(){
                return;
            }

};

TEST(TaskValidatorTests, IsRepeatableRootTest){

    test_repeatable * t = new test_repeatable();
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;

    ASSERT_EQ(result,Taskus::ValidationResultEnum::VALIDATION_PASSED);
    

}

TEST(TaskValidatorTests, IsRepeatableLeafTest){
    test_repeatable * t = new test_repeatable();
    test_repeatable * tt = new test_repeatable();
    t->dependentTasks.push_back(tt);
    tt->addDependencyTask(t);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::REPEATABLE_NOT_ROOT);

}

TEST(TaskValidatorTests, LeafDoesntHaveDependencies){
    
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    t->dependentTasks.push_back(t);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::NO_DEPENDENCIES_NOT_ROOT);


}


TEST(TaskValidatorTests, RootHasDependencies){
    
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    t->addDependencyTask(nullptr);
    t->dependentTasks.push_back(tt);
    tt->addDependencyTask(tt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::DEPENDENCIES_AT_ROOT);
}

TEST(TaskValidatorTests, FinalTaskHasMutation){
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    t->mutationAddTask.push_back(tt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::MUTATION_IN_FINAL_TASKS);

    
}

TEST(TaskValidatorTests, MutationDoesntLeadToDependant){
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    test_dependencies * ttt = new test_dependencies();
    t->dependentTasks.push_back(tt);
    ttt->isMutation = true;
    t->mutationAddTask.push_back(ttt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt,
    delete ttt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::MUTATION_DOEST_LEAD_TO_DEPENDANT);

}

TEST(TaskValidatorTests, ValidMutation){
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    test_dependencies * ttt = new test_dependencies();
    test_dependencies * tttt = new test_dependencies();
    t->dependentTasks.push_back(tt);
    tt->addDependencyTask(t);

    ttt->dependentTasks.push_back(tttt);
    ttt->isMutation = true;
    tttt->dependentTasks.push_back(tt);
    tttt->isMutation = true;
    t->mutationAddTask.push_back(ttt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    delete ttt;
    delete tttt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::VALIDATION_PASSED);
    
}

TEST(TaskValidatorTests, AddMutationDoesntHaveFlag){
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    test_dependencies * ttt = new test_dependencies();
    t->dependentTasks.push_back(tt);
    tt->addDependencyTask(t);
    t->mutationAddTask.push_back(ttt);
    ttt->dependentTasks.push_back(tt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    delete ttt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::MUTATION_NOT_MARKED_AS_ONE);
}

TEST(TaskValidatorTests, MutationsCantHaveMutations){
    test_dependencies * t = new test_dependencies();
    test_dependencies * tt = new test_dependencies();
    test_dependencies * ttt = new test_dependencies();
    test_dependencies * tttt = new test_dependencies();
    t->dependentTasks.push_back(tt);
    tt->addDependencyTask(t);
    t->mutationAddTask.push_back(ttt);
    ttt->isMutation = true;
    ttt->dependentTasks.push_back(tt);
    ttt->mutationAddTask.push_back(tttt);
    Taskus::ValidationResultEnum result = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    delete ttt;
    delete tttt;
    ASSERT_EQ(result, Taskus::ValidationResultEnum::MUTATION_HAS_MUTATIONS_TO_ADD_OR_REMOVE); 
}