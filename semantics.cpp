#include "hw3_output.hpp"
#include <iostream>
#include <vector>

using namespace std;

class Exp {
protected:
    Exp()=default;
    virtual ~Exp()=default;
};

#define YYSTYPE Exp*



//36. 𝐸𝑥𝑝 → 𝐼𝐷
//37. 𝐸𝑥𝑝 → 𝐶𝑎𝑙𝑙
//38. 𝐸𝑥𝑝 → 𝑁𝑈𝑀
//39. 𝐸𝑥𝑝 → 𝑁𝑈𝑀 𝐵
//40. 𝐸𝑥𝑝 → 𝑆𝑇𝑅𝐼𝑁𝐺
//41. 𝐸𝑥𝑝 → 𝑇𝑅𝑈𝐸
//42. 𝐸𝑥𝑝 → 𝐹𝐴𝐿𝑆𝐸

class Id : public Exp{
    string id_name;
public:
    explicit Id(const string val) : id_name(val){};
    string name() const{return id_name;}
};

class Num : public Exp{
    int val;
public:
    explicit Num(const string name) : val(stoi(name)){};
};

class NumB : public Exp{
    int val;
public:
    explicit NumB(const string name) : val(stoi(name)){};
};

class String : public Exp{
    string name;
public:
    explicit String(const string val) : name(val){};
};

class Boolean : public Exp{
    bool val;
public:
    explicit Boolean(bool val) : val(val){};
};


class Type : public Exp{
protected:
    Type()=default;
    virtual ~Type()=default;
public:
    virtual string name() const =0;
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

class Variable : public  Exp{
public:
    Type* type;
    Id* id;
    Variable(Exp* type, Exp* id):type(dynamic_cast<Type *>(type)), id(dynamic_cast<Id *>(id)){};
};


class FormalsList : public Exp{
public:
    vector<Variable*> Formals;
    FormalsList()=default;
    void add(Exp* var){
        Formals.insert(Formals.begin(), dynamic_cast<Variable *>(var));
    };
    vector<string> get_types() const{
        vector<string> argTypes;
        for(auto t:Formals)
            argTypes.emplace_back(t->type->name());
        return argTypes;};
};


class Func : public Type{
    Type* RetType;
    FormalsList* formals_list;
public:
    Func(Exp* p, Exp* f): RetType(dynamic_cast<Type *>(p)), formals_list(dynamic_cast<FormalsList *>(f)){}
    string name() const override{
        vector<string> argTypes = formals_list->get_types();
        return output::makeFunctionType(RetType->name(), argTypes);
    }
    const vector<Variable*>& args() const{
        return formals_list->Formals;
    }
};







