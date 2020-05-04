#include <iostream>
#include <string>
#include <unistd.h>
int main(int argc, char ** argv){
    std::cout<<"System call activated\n";
    if (argc >= 2){
        char** newArgs= new char* [argc];
        for (int i=1; i<argc; i++){
            newArgs[i-1] = argv[i];
        }
        newArgs[argc - 1] = NULL;
        std::string path = "/bin/";
        std::string command = newArgs[0];
        path = path + command;
        execv(path.c_str(), newArgs);
        path = "/usr/bin/" + command;
        execv(path.c_str(), newArgs);
        path = "/usr/local/bin/" + command;
        execv(path.c_str(), newArgs);
        std::cout<<"System call failed\n";
    }
    else
        std::cout<<"Could not call: Accepts more than one argument\n";
}
