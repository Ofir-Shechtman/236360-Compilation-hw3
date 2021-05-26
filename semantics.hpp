#include "hw3_output.hpp"
#include <iostream>
#include <vector>

using namespace std;

class STYPE {
protected:
    STYPE()=default;
    virtual ~STYPE()=default;
};
#ifndef YYSTYPE
#define YYSTYPE STYPE*
#endif


class Type : public STYPE{
protected:
    Type()=default;
    virtual ~Type()=default;
public:
    virtual string name() const =0;
};

class Exp : public STYPE{
protected:
    Exp()=default;
    virtual ~Exp()=default;
public:
    virtual Type* type() const = 0;
};

class Id : public Exp{
    string id_name;
public:
    explicit Id(const string val) : id_name(val){};
    string name() const{return id_name;}
    Type* type() const override;
};

class Num : public Exp{
public:
    int val;
    explicit Num(const string name) : val(stoi(name)){};
    Type* type() const override;
};

class NumB : public Exp{
public:
    int val;
    explicit NumB(const string name) : val(stoi(name)){
        if(val>255)
            output::errorByteTooLarge(yylineno, name);
    };
    explicit NumB(const int val) : NumB(to_string(val)){};
    Type* type() const override;
};

class Boolean : public Exp{
public:
    explicit Boolean(bool=true){};
    static bool is_num(STYPE* e) {
        Exp* exp=dynamic_cast<Exp*>(e);
        if(!exp)
            return false;
        auto id=dynamic_cast<Id*>(e);
        if(id)
            exp=id;
        auto t=exp->type()->name();
        return t=="INT" || t=="BYTE";
    }
    static bool is_bool(STYPE* e) {
        return dynamic_cast<Boolean*>(e)!= nullptr;
    }
    Boolean(STYPE* e1, STYPE* e2){
        if(is_num(e1) &&  is_num(e2))
            return;
        else if(is_bool(e1) &&  is_bool(e2))
            return;
        output::errorSyn(yylineno);
    }
    explicit Boolean(STYPE* e){
        if(is_bool(e))
            return;
        output::errorSyn(yylineno);
    }
    Type* type() const override;
};

class String : public Exp{
    string val;
public:
    explicit String(string val) : val(val){};
    Type* type() const override;
};






class Int : public Type{
public:
    Int()=default;
    string name() const override {return "INT";};
};

class Byte : public Type{
public:
    Byte()=default;
    string name() const override {return "BYTE";};
};

class Void : public Type{
public:
    Void()=default;
    string name() const override {return "VOID";};
};

class TypeString : public Type{
public:
    TypeString()=default;
    string name() const override {return "STRING";};
};

class Bool : public Type{
public:
    Bool()=default;
    string name() const override {return "BOOL";};
};

class Variable : public  STYPE{
public:
    Type* type;
    Id* id;
    Variable(STYPE* type, STYPE* id): type(dynamic_cast<Type *>(type)), id(dynamic_cast<Id *>(id)){};
};

struct ExpType{
    Type* type;
    Exp* exp;
    ExpType(Type* t, Exp* e):type(t), exp(e){};
};

class ExpList : public STYPE{
    static ExpList* temp_name;
public:
    vector<ExpType*> exp_list;
    ExpList()=default;
    void add(STYPE* e){
        auto exp = dynamic_cast<Exp *>(e);
        if(!exp)
            output::errorSyn(yylineno);
        Type* t =  exp->type();
        auto et = new ExpType(t, exp);
        exp_list.insert(exp_list.begin(), et);
    };
};


class FormalsList : public STYPE{
public:
    vector<Variable*> Formals;
    FormalsList()=default;
    void add(STYPE* var){
        Formals.insert(Formals.begin(), dynamic_cast<Variable *>(var));
    };
    vector<string> get_types() const{
        vector<string> argTypes;
        for(auto t:Formals)
            argTypes.emplace_back(t->type->name());
        return argTypes;};
};


class Func : public Type{
    FormalsList* formals_list;
public:
    Type* RetType;
    Func(STYPE* p, STYPE* f):formals_list(dynamic_cast<FormalsList *>(f)),  RetType(dynamic_cast<Type *>(p)){}
    vector<string> type_list() const{
        return formals_list->get_types();
    }
    string name() const override{
        vector<string> argTypes = type_list();
        return output::makeFunctionType(RetType->name(), argTypes);
    }
    const vector<Variable*>& args() const{
        return formals_list->Formals;
    }
};




struct Arg{
    Variable* var;
    int offset;
    //Arg(STYPE* v, int offset): var((Variable*)v), offset(offset){};
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
    SymbolTable(){
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

    };

public:
    void print_funcs() const {
        bool MainMissing = true;
        output::endScope();
        for(auto f:funcs){
            if(f.var->id->name()=="main" && f.var->type->name()=="()->VOID")
                MainMissing= false;
            f.print();
        }
        if(MainMissing) output::errorMainMissing();
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
    void add_Func(STYPE* f){
        add_Func(dynamic_cast<Variable *>(f));
    }
    void add_Func(Variable* f){
        if(contain_var(f->id->name()) || contain_func(f->id->name()))
            output::errorDef(yylineno, f->id->name());
        auto args=  dynamic_cast<Func *>(f->type)->args();
        funcs.emplace_back(Arg(f, 0));
        int offset=args.size()*-1;
        for(auto a:args){
            add_var(a, offset++);
        }
    }

    void add_var(Variable* v, int offset){
        if(contain_var(v->id->name()) || contain_func(v->id->name()))
            output::errorDef(yylineno, v->id->name());
        Arg new_arg = Arg(dynamic_cast<Variable *>(v), offset);
        tables_stack.back().emplace_back(new_arg);
    }
    void add_var(STYPE* v){
        add_var(dynamic_cast<Variable *>(v),  ++offsets_stack.back());
    }

    Type* get_id_type(const Id* id) const{
        for(const auto& i:tables_stack){
            for(auto j : i){
                if(j.var->id->name()==id->name())
                    return j.var->type;
            }
        }
        return nullptr;
    }

    bool contain_var(const string& name) const{
        auto id = Id(name);
        auto t = get_id_type(&id);
        return t != nullptr;
    }

    Func* get_func_type(const Id* id) const{
        for(const auto& i:funcs){
            if(i.var->id->name()==id->name())
                return dynamic_cast<Func *>(i.var->type);
        }
        return nullptr;
    }

    bool contain_func(const string& name) const{
        auto id = Id(name);
        auto t = get_func_type(&id);
        return t != nullptr;
    }

    Type* call(STYPE* id_st, STYPE* el_st=new ExpList()) const{
        Id* id = dynamic_cast<Id *>(id_st);
        auto* el = dynamic_cast<ExpList *>(el_st);
        if(!contain_func(id->name()))
            output::errorUndefFunc(yylineno,id->name());
        auto f_args = get_func_type(id)->type_list();
        auto& exp_list = el->exp_list;
        if(f_args.size()!=exp_list.size())
            output::errorPrototypeMismatch(yylineno, id->name(), f_args);
        auto it1=f_args.begin();
        auto it2=exp_list.begin();
        for(;it1!=f_args.end(); ++it1,++it2){
            if((*it1)!=(*it2)->type->name()  && !((*it1)=="INT" && (*it2)->type->name()=="BYTE"))
                output::errorPrototypeMismatch(yylineno, id->name(), f_args);
        }
        return dynamic_cast<Func *>(get_func_type(id))->RetType;
    }

    void assign(STYPE* id_st, STYPE* exp_st){
        Id* id = dynamic_cast<Id *>(id_st);
        Exp* exp = dynamic_cast<Exp *>(exp_st);
        if(!contain_var(id->name()))
            output::errorUndef(yylineno, id->name());
        auto id_t = get_id_type(id);
        if(id_t->name()==exp->type()->name() || (id_t->name()=="INT" && exp->type()->name()=="BYTE"))
            return;
        else
            output::errorMismatch(yylineno);
    }

};






