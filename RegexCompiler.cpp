#include "RegexCompiler.hpp"

#include <stack>

Frag RegexCompiler::compile(Node* root){
    std::stack<Node*> stack;
    stack.push(root);

    while(!stack.empty()){
        auto n = stack.top();
        
        if(!n->visited){
            for(auto it = n->child.rbegin(); it != n->child.rend(); it++){
                stack.push(*it);
            }
            n->visited = true;
        }else{
            stack.pop();

            switch (n->type){
                case Node::Type::CHAR:
                    n->compiled = handle_char(n);
                    break;
                case Node::Type::SET_ITEM:
                    n->compiled = handle_set_item(n);
                    break;
                case Node::Type::ANY:
                    n->compiled = handle_any(n);
                    break;
                case Node::Type::POS_SET_TAIL:
                    n->compiled = handle_pos_set_tail(n);
                    break;
                case Node::Type::NEG_SET_TAIL:
                    n->compiled = handle_neg_set_tail(n);
                    break;
                case Node::Type::BASIC:
                    n->compiled = handle_basic(n);
                    break;
                case Node::Type::SIMPLE:
                    n->compiled = handle_simple(n);
                    break;
                case Node::Type::RE:
                    n->compiled = handle_re(n);
                    break;
                case Node::Type::BASIC_OP:
                    break;
                default:
                    n->compiled = n->child[0]->compiled;
                    break;
            }

            if(stack.empty()){
                n->compiled.end->type = State::Type::ACCEPT;
                return n->compiled;
            }
        }
    }
}

Frag RegexCompiler::handle_char(Node* root){
    auto s = new State();
    auto e = new State();

    s->type = State::Type::STEP;
    e->type = State::Type::NOP;

    Transition tran;
    tran.is_accepted[root->val[0]] = 1;
    tran.next = e;
    s->transitions.push_back(tran);

    return {s, e};
}

Frag RegexCompiler::handle_any(Node* root){
    auto s = new State();
    auto e = new State();

    s->type = State::Type::STEP;
    e->type = State::Type::NOP;

    Transition tran;
    tran.is_accepted = ~tran.is_accepted;
    tran.next = e;
    s->transitions.push_back(tran);

    return {s, e};
}

Frag RegexCompiler::handle_set_item(Node* root){
    if(root->child.size() == 1){
        return root->child[0]->compiled;
    }else if(root->child.size() == 2){
        auto a = root->child[0]->val[0];
        auto b = root->child[1]->val[0];

        if(b <= a)
            throw std::runtime_error("Wrong set");

        auto s = new State();
        auto e = new State();

        s->type = State::Type::STEP;
        e->type = State::Type::NOP;

        Transition tran;
        for(char c = a; c <= b; c++){
            tran.is_accepted[c] = 1;
        }
        s->transitions.push_back(tran);

        return {s, e};
    }

    //TODO: ERROR
}

Frag RegexCompiler::handle_pos_set_tail(Node* root){
    auto s = new State();
    auto e = new State();
    s->type = State::Type::STEP;
    e->type = State::Type::NOP;

    Transition tran;
    for(auto& item : root->child){
        tran.is_accepted |= item->compiled.begin->transitions[0].is_accepted;
    }

    s->transitions.push_back(tran);

    return {s, e};
}

Frag RegexCompiler::handle_neg_set_tail(Node* root){
    auto s = new State();
    auto e = new State();
    s->type = State::Type::STEP;
    e->type = State::Type::NOP;

    Transition tran;
    for(auto& item : root->child){
        tran.is_accepted |= item->compiled.begin->transitions[0].is_accepted;
    }

    tran.is_accepted = ~tran.is_accepted;
    s->transitions.push_back(tran);

    return {s, e};
}

Frag RegexCompiler::handle_basic(Node* root){
    if(root->child.size() == 1){
        return root->child[0]->compiled;
    }else if(root->child.size() == 2){
        auto re = root->child[0]->compiled;
        auto op = root->child[1]->val[0];

        if(op == '+'){
            re.end->e_transition.push_back(re.begin);
            return re;
        }else if(op == '*'){
            re.end->e_transition.push_back(re.begin);
            return {re.begin, re.begin};
        }
    }
}

Frag RegexCompiler::handle_simple(Node* root){
    if(root->child.size() == 1){
        return root->child[0]->compiled;
    }else if(root->child.size() == 2){
        auto c1 = root->child[0]->compiled;
        auto c2 = root->child[1]->compiled;

        c1.end->e_transition.push_back(c2.begin);   
        return {c1.begin, c2.end};
    }

    //TODO: error
}

Frag RegexCompiler::handle_re(Node* root){
    if(root->child.size() == 1){
        return root->child[0]->compiled;
    }else if(root->child.size() == 2){
        auto s = new State();
        auto e = new State();

        s->type = State::Type::NOP;
        e->type = State::Type::NOP;

        auto c1 = root->child[0]->compiled;
        auto c2 = root->child[1]->compiled;

        s->e_transition.push_back(c1.begin);
        s->e_transition.push_back(c2.begin);

        c1.end->e_transition.push_back(e);
        c2.end->e_transition.push_back(e);

        return {s, e};
    }
}