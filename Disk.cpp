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
    if(currentRequest.PID == 0) //currentRequest is empty
        currentRequest = request;
    else //add the request to the queue
        ioQueue.push_back(request);
}

FileReadRequest Disk::getRequest(){
    return currentRequest;
}

std::deque<FileReadRequest> Disk::getDiskQueue(){
    return ioQueue;
}

int Disk::completeRequest(){
    //get the PID of the process that is being completed
    int PID = currentRequest.PID;

    if (ioQueue.empty()) { //if empty, set current request to 0
        currentRequest = {0, ""};
    }
    else{ //set current request to the next request in the queue
        currentRequest = ioQueue.front();
        ioQueue.pop_front();
    }
        
    return PID;
}