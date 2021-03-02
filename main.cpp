#include "RegexParser.hpp"

void print(Node* n, int i){
    for(int x=0; x<i; x++){
        std::cout<<"\t";
    }

    std::cout<<*n<<std::endl;
    for(auto& c : n->child){
        print(c, i+1);
    }
}

int main(int argc, char** argv){
    RegexParser parser;

    auto n = parser.parse(std::string(argv[1]));
    print(n, 0);

    return 0;
}