/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Implements the Disk class in the operating system
    File: Disk.cpp
*/

#include "Disk.h"

Disk::Disk(){
}

void Disk::addRequest(FileReadRequest request){
    ioQueue.push_back(request);
}

FileReadRequest Disk::getRequest(){
    if (ioQueue.empty())
        throw std::logic_error("No requests in the queue");
    currentRequest = ioQueue.front();
    ioQueue.pop_front();
    return currentRequest;
}

std::deque<FileReadRequest> Disk::getDiskQueue(){
    return ioQueue;
}

int Disk::completeRequest(){
    if (ioQueue.empty()){
        return 0;
    }
    else{
        int PID = ioQueue.front().PID;
        currentRequest = ioQueue.front();
        ioQueue.pop_front();
        return PID;
    }
}