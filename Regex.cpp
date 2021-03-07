#include "Regex.hpp"

Regex::Regex(const std::string& regex){
    auto root = RegexParser::parse(regex);
    nfa = RegexCompiler::compile(root);
}

std::vector<State*> Regex::walk_e(State* s){
    std::vector<State*> states;
    for(auto& state : s->e_transition){
        if(!state->transitions.empty() || state->type == State::Type::ACCEPT){
            states.push_back(state);
        }
        auto r = walk_e(state);
        states.insert(states.end(), r.begin(), r.end());
    }

    return states;
}

std::vector<State*> Regex::handle_state(State* s, char c){
    std::vector<State*> states;
    for(auto& tran : s->transitions){
        if(tran.is_accepted[c] == 1){
            states.push_back(tran.next);
            auto r = walk_e(tran.next);
            states.insert(states.end(), r.begin(), r.end());
        }
    }

    return states;
}

bool Regex::match(const std::string& s){
    std::vector<std::vector<State*>> states;
    states.push_back({});

    states[0].push_back(nfa.begin);
    auto r = walk_e(nfa.begin);
    states[0].insert(states[0].end(), r.begin(), r.end());

    int i = 0; 
    for(auto& c : s){
        std::vector<State*> s;
        for(auto& state : states[i]){
            auto r = handle_state(state, c);
            s.insert(s.end(), r.begin(), r.end());
        }

        if(s.empty())
            break;

        states.push_back(s);
        i++;
    }

    for(auto it = states.rbegin(); it != states.rend(); it++){
        for(auto& s : *it){
            if(s->type == State::Type::ACCEPT)
                return 1;
        }
    }

    return 0;
}