#include "SymbolTable.hpp"

SymbolTable* SymbolTable::singleton_= nullptr;
SymbolTable::SymbolTable() {
    auto fl1= new FormalsList();
    fl1->add(new Variable(new TypeString(), new Id("arg")));
    Func* f1= new Func(new Void(), fl1);
    auto* v1 = new Variable(f1, new Id("print"));

    auto fl2= new FormalsList();
    fl2->add(new Variable(new Int(), new Id("arg")));
    Func* f2= new Func(new Void(), fl2);
    auto* v2 = new Variable(f2, new Id("printi"));

    funcs={Arg(v1, 0), Arg(v2, 0)};
    tables_stack.emplace_back(funcs);
    offsets_stack.emplace_back(-1);

}

SymbolTable *SymbolTable::GetInstance() {
    if(singleton_==nullptr){
        singleton_ = new SymbolTable();
    }
    return singleton_;
}