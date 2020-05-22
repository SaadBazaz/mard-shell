#include <iostream>

#include <unistd.h>
using namespace std;

int main () {
    char data[1];
 
    if(read(0, data, 1) < 0)
     write(2, "An error occurred in the read.\n", 31);

    cout<<"The letter was "<< data <<endl;
 
    exit(0);    

}
