#include <signal.h>
#include <cstring>
#include <iostream>
#include <cerrno>
int main(int argc, char ** argv){
    std::cout<<"Killer activated\n";
    for (int i=1; i<argc; i++){
        if (kill(std::stoi(argv[i]),15) == 0)
            std::cout<<"killed process "<<argv[i]<<"\n";
        else
            std::cout << "kill failed: " << std::strerror(errno) << '\n';
    }
}
