/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Processes within the operating system
    File: Process.cpp
*/

#include "Process.h"

Process::Process(){
}

Process::Process(int PID, int parent){
    this->PID = PID;
    state = NEW;
    this->parent = parent;
}

int Process::getPID(){
    return PID;
}

void Process::addChild(int PID){
    children.push_back(PID);
}

std::vector<int> Process::getChildren(){
    return children;
}

void Process::setState(STATE state){
    if (state == NEW || state == READY || state == RUNNING || state == WAITING || state == TERMINATED || state == ZOMBIE)
        this->state = state;
    else
        throw std::invalid_argument("Invalid state");
}

STATE Process::getState(){
    return state;
}

int Process::getParent(){
    return parent;
}

void Process::setLastUsed(int count){
    leastRecentlyUsed = count;
}

int Process::getLastUsed(){
    return leastRecentlyUsed;
}