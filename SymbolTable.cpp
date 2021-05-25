#include "SymbolTable.hpp"

SymbolTable* SymbolTable::singleton_= nullptr;
SymbolTable::SymbolTable() {
    tables_stack.emplace_back(vector<Arg>());
    offsets_stack.emplace_back(0);
    auto fl1= new FormalsList();
    fl1->add(new TypeString());
    Func* f1= new Func(new Void(), new Id("print"), fl1);

    auto fl2= new FormalsList();
    fl2->add(new Int());
    Func* f2= new Func(new Void(), new Id("print"), fl2);

    funcs.emplace_back(f1);
    funcs.emplace_back(f2);

}

SymbolTable *SymbolTable::GetInstance() {
    if(singleton_==nullptr){
        singleton_ = new SymbolTable();
    }
    return singleton_;
}