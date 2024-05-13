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
    if(currentRequest.PID == 0)
        currentRequest = request;
    else
        ioQueue.push_back(request);
}

FileReadRequest Disk::getRequest(){
    return currentRequest;
}

std::deque<FileReadRequest> Disk::getDiskQueue(){
    return ioQueue;
}

int Disk::completeRequest(){
    int PID = currentRequest.PID;

    if (ioQueue.empty()) {
        currentRequest = {0, ""};
    }
    else{
        currentRequest = ioQueue.front();
        ioQueue.pop_front();
    }
        
    return PID;
}