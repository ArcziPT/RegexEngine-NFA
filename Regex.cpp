#include "Regex.hpp"

Regex::Regex(const std::string& regex){
    auto root = RegexParser::parse(regex);
    nfa = RegexCompiler::compile(root);
}

/**
 * Walk through states using epsilon transitions.
 * If state has non-epsilon transitions it can be used in next step so return it.
 * When we get to accepting state return it to.
 */
std::vector<State*> Regex::walk_e(State* s){
    std::vector<State*> states;

    for(auto& state : s->e_transition){
        if(!state->transitions.empty() || state->type == State::Type::ACCEPT){
            states.push_back(state);
        }
        auto r = walk_e(state); //recurse
        states.insert(states.end(), r.begin(), r.end()); //add to result
    }

    return states;
}

/**
 * Check all transitions and take it if possible, then call walk_e to
 * return all posible next states.
 */
std::vector<State*> Regex::handle_state(State* s, char c){
    std::vector<State*> states;
    for(auto& tran : s->transitions){
        if(tran.is_accepted[c] == 1){
            states.push_back(tran.next); //add current state
            auto r = walk_e(tran.next); //add states, which can be achieved by taking epsilon transitions
            states.insert(states.end(), r.begin(), r.end());
        }
    }

    return states;
}

/**
 * Tries to find a match for string starting at the begining.
 * If it fails, returns {pos=0, len=0}.
 */
Match Regex::run_once(std::string_view sv){
    std::vector<std::vector<State*>> states; //i-th item holds states that we were in after i-th iteration (i-th character from input)
    states.push_back({});

    states[0].push_back(nfa.begin); //begining state of nfa
    auto r = walk_e(nfa.begin); //all states that empty string can achieve
    states[0].insert(states[0].end(), r.begin(), r.end());

    int i = 0; 
    //for every character
    for(auto& c : sv){
        std::vector<State*> s;
        //find possible states
        for(auto& state : states[i]){
            auto r = handle_state(state, c);
            s.insert(s.end(), r.begin(), r.end());
        }

        //if we didnt find any -> finish
        if(s.empty())
            break;

        states.push_back(s);
        i++;
    }

    //iterate through visited states from the latest, so we return maximum match
    for(auto it = states.rbegin(); it != states.rend(); it++){
        for(auto& s : *it){
            if(s->type == State::Type::ACCEPT)
                return {0, std::distance(it, states.rend())-1};
        }
    }

    return {0, 0};
}

//Check if whole strirng is accepted by regex
bool Regex::check(std::string_view sv){
    auto m = run_once(sv);

    return m.len == sv.size();
}

//Find first matching substring
Match Regex::one_match(std::string_view sv){
    int sz = sv.size();
    //for every substring
    for(int i=0; i<sz; i++){
        auto m = run_once(sv.substr(i, sz-i));
        if(m.len > 0){
            m.pos += i;
            return m;
        }
    }

    return {0, 0};
}

//Find all matching substrings
std::vector<Match> Regex::all_matches(std::string_view sv){
    std::vector<Match> matches;
    
    int sz = sv.size();
    //for every substring
    for(int i=0; i<sz; i++){
        auto m = run_once(sv.substr(i, sz-i));
        if(m.len > 0){
            m.pos += i;
            matches.push_back(m);
        }
    }

    return matches;
}