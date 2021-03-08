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

Match Regex::run_once(std::string_view sv){
    std::vector<std::vector<State*>> states;
    states.push_back({});

    states[0].push_back(nfa.begin);
    auto r = walk_e(nfa.begin);
    states[0].insert(states[0].end(), r.begin(), r.end());

    int i = 0; 
    for(auto& c : sv){
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
                return {0, std::distance(it, states.rend())-1};
        }
    }

    return {0, 0};
}

bool Regex::check(std::string_view sv){
    auto m = run_once(sv);

    return m.len == sv.size();
}

Match Regex::one_match(std::string_view sv){
    int sz = sv.size();
    for(int i=0; i<sz; i++){
        auto m = run_once(sv.substr(i, sz-i));
        if(m.len > 0){
            m.pos += i;
            return m;
        }
    }

    return {0, 0};
}

std::vector<Match> Regex::all_matches(std::string_view sv){
    std::vector<Match> matches;
    
    int sz = sv.size();
    for(int i=0; i<sz; i++){
        auto m = run_once(sv.substr(i, sz-i));
        if(m.len > 0){
            m.pos += i;
            matches.push_back(m);
        }
    }

    return matches;
}