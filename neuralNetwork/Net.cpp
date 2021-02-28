#include <Net.h>
#include <netStructure/FFLayer/FFLayer.h>
#include <netStructure/ConvolutionLayer/CLayer.h>
#include <activations/SigmoidFunction.h>
#include <activations/ReLUFunction.h>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

Net::Net(std::vector<std::vector<int>> structure, int seed){
    srand(seed);
    for(int i = 0; i < structure.size(); i ++){
        if(structure[i].size() < 3){
            throw std::invalid_argument( "wrong description in layer " + i );
            return;
        }
        if(structure[i][0] == 0){
            FFLayer * layer = new FFLayer(i, this, structure[i][2], structure[i][1], new SigmoidFunction());
            this->layers.push_back(layer);
            layer->initConnections();
        }
        if(structure[i][0] == 1){
            //todo
            if(structure[i].size() < 5){
                throw std::invalid_argument( "wrong description in layer " + i );
                return;
            }
            CLayer * clayer = new CLayer(i, this, structure[i][1], structure[i][2], structure[i][3], structure[i][4]);
            clayer->initWeights();
            layers.push_back(clayer);
        }
    }
}
Net::~Net(){
    for(auto l : layers){
        delete l;
    }
}

//void run(const CLayer::Tensor &tensorInput);

void Net::run(Tensor tensorInput){ 
    if(dynamic_cast<CLayer*>(this->layers[0]) == nullptr)
        throw std::invalid_argument( "In order to use Tensor feed first layer must be of convolution type! ");
    CLayer * layer = (CLayer *) layers[0];
    layer->run(tensorInput);
    bool ffStarted = false;

    for(int i = 1; i < this->layers.size(); i ++){
        Layer * l = layers[i];
        if (dynamic_cast<CLayer*>(l) != nullptr) {
            if(ffStarted)
                throw std::invalid_argument( "You cannot use CLayer after FFLayer!");
            CLayer * cl = (CLayer *) l;
            cl->run(tensorInput);
            tensorInput = cl->outputTensor;
        }else if(dynamic_cast<FFLayer*>(l) != nullptr){
            FFLayer * ff = (FFLayer *) l;
            ff->run(layers[i - 1]->outputVector);
            ffStarted = true;
        }
    }
}