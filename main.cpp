#include <iostream>
#include "neuralNetwork/Net.h"

int main(){
    Net n = Net({{0, 4}, {0, 4}, {0, 4}});
    for(auto l : n.layers){
       // l->run();
    }
    std::cout<<"";
}