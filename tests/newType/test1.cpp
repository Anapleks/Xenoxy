#include <Xenoxy/impl/TypeManager.hpp>

int main() {
    Xenoxy::TypeManager typeMgr;
    Xenoxy::TypeDef type1("int", Xenoxy::TypeType::Primitive, 8);
    Xenoxy::TypeDef* type1Ptr1 = typeMgr.AddTypeDef(type1);
    Xenoxy::TypeDef* type1Ptr2 = typeMgr.GetTypeDef("int");
    type1Ptr1->DebugPrint();
    type1Ptr2->DebugPrint();

    Xenoxy::TypeDef type2("class1", Xenoxy::TypeType::Composite);
    type2.AddField("test", type1Ptr2);
    type2.AddField("test2", type1Ptr2);
    typeMgr.AddTypeDef(type2);
    Xenoxy::TypeDef* type2Ptr2 = typeMgr.GetTypeDef("class1");
    type2Ptr2->DebugPrint();

    Xenoxy::TypeDef type3("class2", Xenoxy::TypeType::Composite);
    type3.AddField("test3", type2Ptr2);
    type3.AddField("test4", type2Ptr2);
    type3.AddField("test5", type1Ptr2);
    typeMgr.AddTypeDef(type3);
    Xenoxy::TypeDef* type3Ptr2 = typeMgr.GetTypeDef("class2");
    type3Ptr2->DebugPrint();
 
    Xenoxy::ObjectInst object = typeMgr.CreateObjectOfType("class2", Xenoxy::ObjectFlags{false, false, false, false});
    
    std::cout<<"Test recursive mapping"<<std::endl;
    *(std::int64_t*)object["test4"]["test"].Get() = 15;
    std::cout<<"Value 1: "<<*(std::int64_t*)object["test4"]["test"].Get()<<std::flush;
    std::cout<<"Value 2: "<<*(std::int64_t*)object["test4.test"].Get()<<std::endl;

    std::cout<<"Test no-overlap"<<std::endl;
    *(std::int64_t*)object["test4"]["test2"].Get() = 30;
    *(std::int64_t*)object["test3"]["test2"].Get() = 45;
    *(std::int64_t*)object["test5"].Get() = 60;
    std::cout<<"Value 1: "<<*(std::int64_t*)object["test4"]["test"].Get()<<std::endl;
    std::cout<<"Value 2: "<<*(std::int64_t*)object["test4"]["test2"].Get()<<std::endl;
    std::cout<<"Value 3: "<<*(std::int64_t*)object["test3"]["test2"].Get()<<std::endl;
    std::cout<<"Value 4: "<<*(std::int64_t*)object["test5"].Get()<<std::endl;

    std::cout<<((*type3Ptr2)["test5"] != *type2Ptr2)<<std::endl;
}