#include "RegexParser.hpp"

#include <stack>

enum PROD{
    RE,
    UNION,
    SIMPLE,
    CONCAT,
    BASIC,
    BASIC_OP,
    ELEMENTARY,
    GROUP,
    ANY,
    EOS,
    CHAR,
    SET,
    SET_TAIL,
    SET_ITEMS,
    SET_ITEMS_TAIL,
    SET_ITEM,
    SET_ITEM_TAIL,
    RANGE,
    LPAR,
    RPAR,
    LSQRBR,
    RSQRBR
};

struct Context{
    PROD prod;
    Node* n;
};

Node* RegexParser::parse(const std::string& regex){
    std::stringstream ss(std::move(regex));
    std::stack<Context> prods;
    
    auto root = new Node("main", nullptr);
    prods.push({PROD::RE, root});

    while(!ss.eof() && !prods.empty()){
        auto ctx = prods.top();
        prods.pop();
        auto c = next(ss);

        switch(ctx.prod)
        {
            case RE:{
                auto nn = new Node("re", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({UNION, nn});
                prods.push({SIMPLE, nn});
                break;
            }
            
            case UNION:{
                if(c == '|'){
                    match(ss);
                    
                    auto nn = new Node("union", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);

                    prods.push({RE, nn});
                }
                break;
            }

            case SIMPLE:{
                auto nn = new Node("simple", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);

                prods.push({CONCAT, nn});
                prods.push({BASIC, nn});
                break;
            }

            case CONCAT: {
                auto set = {')', ']', '*', '+', '|'};
                bool is_in = false;
                for(auto& s : set){
                    if(c == s)
                        is_in = true;
                }

                if(is_in)
                    break;

                auto nn = new Node("concat", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({SIMPLE, nn});
                break;
            }

            case BASIC:{
                auto nn = new Node("basic", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({BASIC_OP, nn});
                prods.push({ELEMENTARY, nn});
                break;
            }
            
            case BASIC_OP:{
                if(c == '*'){
                    auto nn = new Node("*", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    match(ss);
                }else if(c == '+'){
                    auto nn = new Node("+", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    match(ss);
                }
                break;
            }

            case ELEMENTARY:{
                auto nn = new Node("elementary", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                switch(c)
                {
                case '(':
                    prods.push({GROUP, nn});
                    break;
                case '.':
                    prods.push({ANY, nn});
                    break;
                case '$':
                    prods.push({EOS, nn});
                    break;
                case '[':
                    prods.push({SET, nn});
                    break;
                default:
                    prods.push({CHAR, nn});
                }
                break;
            }

            case GROUP:{
                auto nn = new Node("group", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({RPAR, nn});
                prods.push({RE, nn});
                prods.push({LPAR, nn});
                break;
            }

            case ANY:{
                auto nn = new Node("any", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                match(ss);
                break;
            }

            case EOS:{
                auto nn = new Node("eos", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                match(ss);
                break;
            }

            case LPAR:{
                match(ss);
                break;
            }

            case RPAR:{
                match(ss);
                break;
            }

            case CHAR:{
                if(c == '\\'){
                    std::string s = "";
                    s += c;
                    match(ss);
                    s += next(ss);
                    match(ss);

                    auto nn = new Node(s, ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                }else{
                    std::string s = "";
                    s += c;
                    auto nn = new Node(s, ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    match(ss);
                }
                break;
            }

            case SET:{
                auto nn = new Node("set", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);

                prods.push({RSQRBR, nn});
                prods.push({SET_TAIL, nn});
                prods.push({LSQRBR, nn});
                break;
            }

            case RSQRBR:{
                match(ss);
                break;
            }

            case LSQRBR:{
                match(ss);
                break;
            }

            case SET_TAIL:{
                if(c == '^'){
                    match(ss);
                    auto nn = new Node("neg-set-tail", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    
                    prods.push({SET_ITEMS, nn});
                }else{
                    auto nn = new Node("pos-set-tail", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    
                    prods.push({SET_ITEMS, nn});
                }
                break;
            }
                
            case SET_ITEMS:{
                auto nn = new Node("set-items", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({SET_ITEMS_TAIL, nn});
                prods.push({SET_ITEM, nn});
                break;
            }

            case SET_ITEMS_TAIL:{
                auto set = {'(', ')', '[', ']', '*', '+', '|', '-', '^', '$', '.'};
                bool is_in = false;
                for(auto& c : set){
                    if(next(ss) == c)
                        is_in = true;
                }

                if(is_in)
                    break;

                auto nn = new Node("set-items-tail", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);

                prods.push({SET_ITEMS, nn});
                break;
            }

            case SET_ITEM:{
                auto nn = new Node("set-item", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                
                prods.push({SET_ITEM_TAIL, nn});
                prods.push({CHAR, nn});
                break;
            }

            case SET_ITEM_TAIL:{
                if(next(ss) == '-'){
                    auto nn = new Node("set-item-tail", ctx.n);
                    nn->parent = ctx.n;
                    ctx.n->child.push_back(nn);
                    
                    prods.push({RANGE, nn});
                }
                break;
            }

            case RANGE:{
                auto nn = new Node("range", ctx.n);
                nn->parent = ctx.n;
                ctx.n->child.push_back(nn);
                match(ss);
                
                prods.push({CHAR, nn});
            }
        }
    }

    return root;
}

char RegexParser::next(std::stringstream& ss){
    return ss.peek();
}

void RegexParser::match(std::stringstream& ss){
    char x;
    x = ss.get();
}