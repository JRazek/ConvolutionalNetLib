#include "GeneticLearningUnit.h"
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <ncurses.h>
#include <netStructure/ConvolutionLayer/CLayer.h>
#include <netStructure/FFLayer/FFLayer.h>
#include <netStructure/PoolingLayer/PoolingLayer.h>
#include <map>
GeneticLearningUnit::GeneticLearningUnit(const std::vector< std::vector < int > > structure, int goalGenerations, int individualsPerGeneration):
    structure(structure), goalGenerations(goalGenerations), individualsPerGeneration(individualsPerGeneration){
        //this->win = initscr();
    }

void GeneticLearningUnit::initPopulation(std::vector< std::pair<Net *, csnake::Renderer *> > &currIndividuals, int seed = 0){
    int scoreSum = 0;
    srand(seed);
    std::map<int, int> intervalMap;
    for(int i = 0; i < currIndividuals.size(); i ++){
        auto p = currIndividuals[i];
        scoreSum += p.second->getGame()->getScore();
        intervalMap[scoreSum] = i;
    }

    std::vector<Net *> newGeneration;
    for(int i = 0; i < this->individualsPerGeneration; i ++){
        int random1 = rand() % (scoreSum + 1);
        int random2 = rand() % (scoreSum + 1);
        auto it1 = intervalMap.lower_bound(random1);
        auto it2 = intervalMap.lower_bound(random2);

        if(it1 == intervalMap.end() || it2 == intervalMap.end()){
            throw std::invalid_argument("error!");
        }
        if(it1 == it2){
            ++it2;
            if(it2 == intervalMap.end()){
                it2--;
                if(it2 == intervalMap.begin()){
                    throw std::invalid_argument("There must be more than 2 individuals!!!!");
                }
                it2--;
            }
        }
        Net * p1 = currIndividuals[(*it1).second].first;
        Net * p2 = currIndividuals[(*it2).second].first;

        Net * child = new Net();
        for(int i = 0; i < p1->layers.size(); i ++){
            Layer * l1 = p1->layers[i];
            Layer * l2 = p2->layers[i];
            if (dynamic_cast<CLayer*>(l1) != nullptr && dynamic_cast<CLayer*>(l2) != nullptr){
                child->layers.push_back(new CLayer(i, child, (*(CLayer *) l1), (*(CLayer *) l2), this->mutationRate, rand() % 100 ) );
            }else if(dynamic_cast<FFLayer*>(l1) != nullptr && dynamic_cast<FFLayer*>(l2) != nullptr){
            //FFLayer(int id, Net * net, const FFLayer &p1, const FFLayer &p2, int seed);
                child->layers.push_back(new FFLayer(i, child, (*(FFLayer *) l1), (*(FFLayer *) l2), this->mutationRate, rand() % 100 ));
            }else if(dynamic_cast<PoolingLayer*>(l1) != nullptr && dynamic_cast<PoolingLayer*>(l2) != nullptr){
                child->layers.push_back(new PoolingLayer(*(PoolingLayer *) l1));
            }else{
                throw std::invalid_argument("networks wont match!!!");
            }
        }
        newGeneration.push_back(child);
        //mutation here
    }
    for(auto n : this->currIndividuals){
        delete n.first;
        delete n.second;
    }
    this->currIndividuals.clear();

    for(auto net : newGeneration){
        csnake::SnakeGame * game = new csnake::SnakeGame(this->mapSizeX, this->mapSizeY, this->impenetrableWalls);
        csnake::Renderer * renderer = new csnake::Renderer(game, win);
        this->currIndividuals.push_back({net, renderer});
    }
}
void GeneticLearningUnit::initPopulation(bool cross = false, int seed = 0){
    if(cross){
        initPopulation(this->currIndividuals, seed);
    }else{
        srand(seed);
        for(int i = 0; i < this->individualsPerGeneration; i ++){
            csnake::SnakeGame * game = new csnake::SnakeGame(this->mapSizeX, this->mapSizeY, this->impenetrableWalls);
            csnake::Renderer * renderer = new csnake::Renderer(game, win);
            Net * net = new Net(this->structure, rand() % (int)1e9+7);
            this->currIndividuals.push_back({net, renderer});
        }
    }
    this->generationNum++;
}
void GeneticLearningUnit::runCurrentGeneration(){
    std::vector<bool> deadTab(currIndividuals.size(), false);
    while(1){
        int deadCount = 0;
        for(int i = 0; i < currIndividuals.size(); i ++){
            auto k = currIndividuals[i];
            auto board = k.second->getBoard();
            Tensor input = Tensor(k.second->getGame()->getWidth(), k.second->getGame()->getHeight(), 1);
            if(board.size() != input.getY() || board[0].size() != input.getX()){
                throw std::invalid_argument("board and tensor wont match!");
            }
            for(int y = 0; y < input.getY(); y++){
                for(int x = 0; x < input.getX(); x++){
                    input.edit(x, y, 0, board[y][x]);
                }
            }
            auto gameManager = k.second;
            if(i == 0){
                gameManager->run(false);
            }else{
                gameManager->run(false);
            }
            gameManager->getGame()->update();
            k.first->run(input);

            int strongest = 0;
            float strongestVal = 0;
            const std::vector<float> result = k.first->getResult();
            for(int i = 0; i < result.size(); i ++){
                if(result[i] > strongestVal){
                    strongest = i;
                    strongestVal = result[i];
                }
            }
            char dir;
            switch (strongest){
                case 0:
                    dir = 'w';
                    break;
                case 1:
                    dir = 's';
                    break;
                case 2:
                    dir = 'a';
                    break;
                case 3:
                    dir = 'd';
                    break;
                default:
                    throw std::invalid_argument("error!");
                    break;
            }

            k.second->getGame()->changeDirection(dir);
            
            if(!gameManager->getGame()->getSnakeStatus() && deadTab[i] == false){
                deadTab[i] == true;
                deadCount ++;
            }
        }
        if(deadCount == this->currIndividuals.size()){
            break;
        }
    }
}
int GeneticLearningUnit::getGenerationNum() const{
    return this->generationNum;
}
GeneticLearningUnit::~GeneticLearningUnit(){
    for(auto k : currIndividuals){
        delete k.first;
        delete k.second;
    }
}
