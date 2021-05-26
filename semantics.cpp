#include "semantics.hpp"

SymbolTable* SymbolTable::singleton_= nullptr;

SymbolTable *SymbolTable::GetInstance() {
    if(singleton_==nullptr){
        singleton_ = new SymbolTable();
    }
    return singleton_;
}

Type *Id::type() const {
    SymbolTable& st = *SymbolTable::GetInstance();
    if(st.contain_func(name()) || !st.contain_var(name()))
        output::errorUndef(yylineno, name());
    return st.get_id_type(this);

}

Type *Num::type() const {
    return new Int();
}

Type *NumB::type() const {
    return new Byte();
}

Type *Boolean::type() const {
    return new Bool();
}

Type *String::type() const {
    return new TypeString();
}


