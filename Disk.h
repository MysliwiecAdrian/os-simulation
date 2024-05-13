/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Defines the Disk class in the operating system
    File: Disk.h
*/

#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <queue>

struct FileReadRequest
{
    int PID{0};
    std::string fileName{""};
};

class Disk{
    public:
        Disk();
        void addRequest(FileReadRequest request);
        FileReadRequest getRequest();
        std::deque<FileReadRequest> getDiskQueue();
        int completeRequest();

    private:
        FileReadRequest currentRequest = {0, ""};
        std::deque<FileReadRequest> ioQueue;
    
};

#endif