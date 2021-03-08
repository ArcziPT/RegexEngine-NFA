#include "Regex.hpp"

int main(int argc, char** argv){
    Regex regex{std::string(argv[1])};
    std::cout<<regex.check(std::string(argv[2]));

    return 0;
}