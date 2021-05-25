

#ifndef HW3_SYMBOLTABLE_H
#define HW3_SYMBOLTABLE_H

#endif //HW3_SYMBOLTABLE_H
#include "semantics.cpp"




class Arg{
    Variable* var;
    int offset;
public:
    //Arg(Exp* v, int offset): var((Variable*)v), offset(offset){};
    Arg(Variable* v, int offset):var(v), offset(offset){};
    void print() const {
        output::printID(var->id->name(), offset,var->type->name());
    }
};

class SymbolTable{
    vector<vector<Arg>> tables_stack;
    vector<int> offsets_stack;
    vector<Arg> funcs;
    static SymbolTable* singleton_;
    SymbolTable();

public:
    ~SymbolTable(){
        print_funcs();
    }
    void print_funcs() const {
        output::endScope();
        for(auto f:funcs){
            f.print();
        }
    }
    void operator=(const SymbolTable &) = delete;
    static SymbolTable *GetInstance();
    void push(){
        tables_stack.emplace_back(vector<Arg>());
        offsets_stack.emplace_back(offsets_stack.back());
    }
    void pop(){
        output::endScope();
        for(auto a:tables_stack.back()){
            a.print();
        }
        tables_stack.pop_back();
        offsets_stack.pop_back();
    }
    void add_Func(Exp* f){
        add_Func(dynamic_cast<Variable *>(f));
    }
    void add_Func(Variable* f){
        auto args=  dynamic_cast<Func *>(f->type)->args();
        funcs.emplace_back(Arg(f, 0));
        int offset=args.size()*-1;
        for(auto a:args){
            add_var(a, offset++);
        }
    }

    void add_var(Variable* v, int offset){
        Arg new_arg = Arg(dynamic_cast<Variable *>(v), offset);
        tables_stack.back().emplace_back(new_arg);
    }
    void add_var(Exp* v){
        add_var(dynamic_cast<Variable *>(v),  ++offsets_stack.back());
    }

};
