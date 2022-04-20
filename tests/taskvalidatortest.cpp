#include <gtest/gtest.h>
#include <Taskus.h>
#include <iostream>


class test_repeatable : public Taskus::Task{
        public:
            test_repeatable() : Task(){
                isRepeatable.store(true);
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


class BranchTest : public Taskus::BranchTask{
    public:
        BranchTest(Taskus::TaskPool *t) : BranchTask(t){
            possibleBranches.push_back({});
        }

        void tryMutate(){

        }

        void runTaskFunction(){

        }

        int chooseBranch(){
            return 0;
        }

};


TEST(TaskValidatorTests, BranchHaveDependants){
    BranchTest * t = new BranchTest(nullptr);
    test_dependencies * tt = new test_dependencies();
    *t += tt;
    Taskus::ValidationResultEnum v = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(v, Taskus::ValidationResultEnum::BRANCH_TASK_HAS_DEPENDANT_TASKS);

}

class BranchTestEmpty : public Taskus::BranchTask{
    public:
        BranchTestEmpty(Taskus::TaskPool *t) : BranchTask(t){
        }

        void tryMutate(){

        }

        void runTaskFunction(){

        }

        int chooseBranch(){
            return 0;
        }

};


TEST(TaskValidatorTests, BranchDoesNotHaveBranches){
    BranchTestEmpty * t = new BranchTestEmpty(nullptr);

    Taskus::ValidationResultEnum v = Taskus::ValidateTask(t);
    delete t;
    ASSERT_EQ(v, Taskus::ValidationResultEnum::BRANCH_TASK_HAS_NO_BRANCHES);

}

TEST(TaskValidatorTests, BranchHaveMutations){
    BranchTest * t = new BranchTest(nullptr);
    test_dependencies * tt = new test_dependencies();
    t->mutationAddTask.push_back(tt);
    Taskus::ValidationResultEnum v = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_EQ(v, Taskus::ValidationResultEnum::BRANCH_TASK_HAS_MUTATIONS);

}


TEST(TaskValidatorTests, BranchTaskHasAMistakeInBranch){
    BranchTest * t = new BranchTest(nullptr);
    test_dependencies * tt = new test_dependencies();
    tt->isRepeatable.store(true);
    t->possibleBranches[0].push_back(tt);
    Taskus::ValidationResultEnum v = Taskus::ValidateTask(t);
    delete t;
    delete tt;
    ASSERT_NE(v, Taskus::ValidationResultEnum::VALIDATION_PASSED);
}

TEST(TaskValidatorTests, ValidRepeatableBranchTask){
    test_dependencies * root = new test_dependencies();
    test_dependencies * r = new test_dependencies();
    *root += r;
    test_dependencies * rr = new test_dependencies();
    *r += rr;
    BranchTest * t = new BranchTest(nullptr);
    *rr += t;
    t->isRepeatable.store(true);
    test_dependencies * tt = new test_dependencies();
    t->possibleBranches[0].push_back(tt);
    Taskus::ValidationResultEnum v = Taskus::ValidateTask(root);
    delete t;
    delete tt;
    delete root;
    delete r;
    delete rr;
    ASSERT_EQ(v, Taskus::ValidationResultEnum::VALIDATION_PASSED);
}


