/*
    Name: Adrian Mysliwiec
    Course: CSCI340 Operating System
    Description: Executes the simulation of an operating system
    File: SimOS.cpp
*/

#include "SimOS.h"

SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize){
    this->numberOfDisks = numberOfDisks;
    this->amountOfRAM = amountOfRAM;
    this->pageSize = pageSize;

    for (int i = 0; i < numberOfDisks; i++)
        disks.push_back(Disk());

    memory.resize(amountOfRAM / pageSize);
}

void SimOS::NewProcess(){
    Process newProcess(PID);

    //add process to memory
    if(!addMemory(PID))
        throw std::runtime_error("Memory is full");

    if (runningCPU == NO_PROCESS){
        //enter ready queue
        readyQueue.push_back(newProcess.getPID());
        newProcess.setState(READY);

        //enter CPU
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        newProcess.setState(RUNNING);
    }
    else{
        //enter ready queue and wait
        readyQueue.push_back(newProcess.getPID());
        newProcess.setState(READY);
    }

    //store process in OS
    processes[PID] = newProcess;
    PID++;
}

void SimOS::SimFork(){
    //check if process is running
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    //create new process
    Process newProcess(PID, runningCPU);
    processes[PID] = newProcess;

    //add process to memory
    if(!addMemory(PID))
        throw std::runtime_error("Memory is full");
    
    //enter ready queue
    readyQueue.push_back(newProcess.getPID());
    processes[PID].setState(READY);

    //add child to parent
    processes[runningCPU].addChild(PID);
    
    PID++;
}

void SimOS::SimExit(){
    //check if process is running
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    int parent = processes[runningCPU].getParent();
    if (parent == 0){ //running process is parent
        removeMemory(runningCPU); //remove process from memory
        cascadeTerminate(runningCPU); //terminates all children
        removeReadyQueue(runningCPU); //removes process from ready queue
        removeIOQueue(runningCPU); //removes process from I/O queues
        
        //removes from OS
        processes[runningCPU].setState(TERMINATED);
        processes.erase(runningCPU);
    }
    else if (processes[parent].getState() == WAITING){ //running process is child
        processes[parent].setState(READY);
        readyQueue.push_back(parent);
        processes[runningCPU].setState(TERMINATED);
        processes.erase(runningCPU);
    }
    else{ //process becomes zombie
        removeMemory(runningCPU);
        removeReadyQueue(runningCPU);
        removeIOQueue(runningCPU);
        processes[runningCPU].setState(ZOMBIE);
    }


    runningCPU = NO_PROCESS;

    //moves next process in ready queue to CPU
    if (!readyQueue.empty()){
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[runningCPU].setState(RUNNING);
    }
        
}

void SimOS::SimWait(){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");
    
    processes[runningCPU].setState(WAITING);

    //searches for zombie children
    bool zombieExists = false;
    for (int i = 0; i < processes[runningCPU].getChildren().size(); i++){
        int child = processes[runningCPU].getChildren()[i];
        if (processes[child].getState() == ZOMBIE){
            zombieExists = true;
            processes[child].setState(TERMINATED);
            processes.erase(child);
            break;
        }
    }

    if(zombieExists){
        processes[runningCPU].setState(RUNNING);
    }
    else{
        processes[runningCPU].setState(READY);
        readyQueue.push_back(runningCPU);
        runningCPU = NO_PROCESS;
    }

    if (!readyQueue.empty()){
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[runningCPU].setState(RUNNING);
    }
}

void SimOS::TimerInterrupt(){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    processes[runningCPU].setState(WAITING);
    readyQueue.push_back(runningCPU);
    runningCPU = readyQueue.front();
    processes[runningCPU].setState(RUNNING);
    readyQueue.pop_front();
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");
    disks[diskNumber].addRequest({runningCPU, fileName});
    processes[runningCPU].setState(WAITING);
    runningCPU = NO_PROCESS;

    if (!readyQueue.empty()){
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[runningCPU].setState(RUNNING);
    }
}

void SimOS::DiskJobCompleted(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    int completedJob = disks[diskNumber].completeRequest();
    if (completedJob == NO_PROCESS) {
        throw std::logic_error("No process is waiting for disk job to complete");
    } 
    else if (runningCPU == NO_PROCESS){
        //enter ready queue
        readyQueue.push_back(completedJob);
        processes[completedJob].setState(READY);

        //enter CPU
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[completedJob].setState(RUNNING);
    }
    else {
        readyQueue.push_back(completedJob);
        processes[completedJob].setState(READY);
    }

}

void SimOS::AccessMemoryAddress(unsigned long long address){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    unsigned long long pageNumber = address / pageSize;
    bool pageExists = false;
    int emptyFrame = -1;

    for(int i = 0; i < memory.size(); i++){
        //page exists in memory so LRU is increased
        if (memory[i].PID == runningCPU && memory[i].pageNumber == pageNumber){
            leastRecentlyUsed[memory[i].frameNumber]++;
            pageExists = true;
            break;
        }

        //empty frame found
        if (memory[i].PID == NO_PROCESS && emptyFrame == -1)
            emptyFrame = i;
    }

    //locate the least recently used frame
    unsigned long long LRUFRAME = ULLONG_MAX;
    for(int i = 0; i < memory.size(); i++){
        if (leastRecentlyUsed[memory[i].frameNumber] < leastRecentlyUsed[LRUFRAME])
            LRUFRAME = i;
    }

    if(!pageExists){
        if (emptyFrame != -1){
            //puts process into memory
            memory[emptyFrame].PID = runningCPU;
            memory[emptyFrame].pageNumber = pageNumber;
            memory[emptyFrame].frameNumber = emptyFrame;
            leastRecentlyUsed[emptyFrame] = 0;
        }
        else{
            //replaces process with least recently used frame
            memory[LRUFRAME].PID = runningCPU;
            memory[LRUFRAME].pageNumber = pageNumber;
            leastRecentlyUsed[LRUFRAME] = 0;
        }
    }

}

int SimOS::GetCPU(){
    return runningCPU;
}

std::deque<int> SimOS::GetReadyQueue(){
    return readyQueue;
}

MemoryUsage SimOS::GetMemory(){
    return memory;
}

FileReadRequest SimOS::GetDisk(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    return disks[diskNumber].getRequest();
}

std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");

    return disks[diskNumber].getDiskQueue();
}

//helper functions
void SimOS::cascadeTerminate(int PID){
    for (int i = 0; i < processes[PID].getChildren().size(); i++){
        int child = processes[PID].getChildren()[i];
        removeMemory(child);
        removeReadyQueue(child);
        cascadeTerminate(child);

        processes[child].setState(TERMINATED);
        processes.erase(child);
    }
}

bool SimOS::addMemory(int PID){
    for (int i = 0; i < memory.size(); i++){
        if (memory[i].PID == NO_PROCESS){
            memory[i].PID = PID;
            memory[i].pageNumber = 0;
            memory[i].frameNumber = i;
            return true;
        }
    }
    return false;
}

void SimOS::removeMemory(int PID){
    for (int i = 0; i < memory.size(); i++){
        if (memory[i].PID == PID){
            memory[i].PID = NO_PROCESS;
            leastRecentlyUsed.erase(memory[i].frameNumber);
        }
    }
}

void SimOS::removeReadyQueue(int PID){
    std::deque<int> temp;
    while (!readyQueue.empty()){
        int front = readyQueue.front();
        readyQueue.pop_front();
        if (front != PID)
            temp.push_back(front);
    }
    readyQueue = temp;
}

void SimOS::removeIOQueue(int PID){
    for (Disk &currentDisk : disks){
        std::deque<FileReadRequest> temp;

        while (!currentDisk.getDiskQueue().empty()){
            FileReadRequest front = currentDisk.getDiskQueue().front();
            currentDisk.getDiskQueue().pop_front();
            if (front.PID != PID)
                temp.push_back(front);
        }

        currentDisk.getDiskQueue() = temp;
    }
}