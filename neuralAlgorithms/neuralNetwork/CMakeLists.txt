cmake_minimum_required(VERSION 3.12)

add_library(
    Net
    Net.h
    Net.cpp
    netStructure/ConvolutionLayer/CLayer.h
    netStructure/ConvolutionLayer/CLayer.cpp
    netStructure/PoolingLayer/PoolingLayer.h
    netStructure/PoolingLayer/PoolingLayer.cpp
    netStructure/FFLayer/FFLayer.h
    netStructure/FFLayer/FFLayer.cpp
    netStructure/Layer/Layer.h
    activations/ActivationFunction.h
    activations/SigmoidFunction.h
    utils/dataStructures/Matrix.h
    utils/dataStructures/Matrix.cpp
    utils/dataStructures/Tensor.h
    utils/dataStructures/Tensor.cpp
    utils/Functions.h
    utils/Functions.cpp
)
include_directories(./)