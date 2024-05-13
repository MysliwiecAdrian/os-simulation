/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Processes within the operating system
    File: Process.h
*/

#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <vector>
#include <stdexcept>

enum STATE{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    ZOMBIE,
};

class Process{
    public:
        Process();
        Process(int PID, int parent = 0);
        int getPID();
        void addChild(int PID);
        void setState(STATE state);
        STATE getState();
        int getParent();

        //tester functions
        std::vector<int> getChildren();
    
    private:
        int PID = 0;
        STATE state;
        int parent;
        std::vector<int> children;
};

#endif