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
        std::vector<int> getChildren();
        void setState(STATE state);
        STATE getState();
        int getParent();
        void setLastUsed(int count);
        int getLastUsed();
    
    private:
        int PID;
        int parent;
        int leastRecentlyUsed = 0;
        
        STATE state;
        std::vector<int> children;
};

#endif