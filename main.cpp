#include "Regex.hpp"

int main(int argc, char** argv){
    Regex regex{std::string(argv[1])};
    std::cout<<regex.check(argv[2])<<std::endl;
    std::cout<<regex.one_match(argv[2])<<std::endl;
    
    for(auto& m : regex.all_matches(argv[2])){
        std::cout<<m<<std::endl;
    }

    return 0;
}