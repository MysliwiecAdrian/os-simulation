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
    for(int i = 0; i < memory.size(); i++){
        memory[i].PID = NO_PROCESS;
        memory[i].pageNumber = 0;
        memory[i].frameNumber = i;
    }
}

void SimOS::NewProcess(){
    Process newProcess(PID);

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
        cascadeTerminate(runningCPU); //terminates all children
        removeReadyQueue(runningCPU); //removes process from ready queue
        removeIOQueue(runningCPU); //removes process from I/O queues
        
        //removes from OS
        removeMemory(runningCPU); //remove process from memory
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
        removeReadyQueue(runningCPU);
        removeIOQueue(runningCPU);
        removeMemory(runningCPU);
        processes[runningCPU].setState(ZOMBIE);
    }


    runningCPU = NO_PROCESS;

    //moves next process in ready queue to CPU
    if (!readyQueue.empty()){
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[runningCPU].setState(RUNNING);
    }

    //checks if no more processes are running to clean up memory
    cleanUpMemory(); 
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

    if(zombieExists){ //process continues running if zombie is found
        processes[runningCPU].setState(RUNNING);
    }
    else{ //process finishes waiting and enters ready queue
        processes[runningCPU].setState(READY);
        readyQueue.push_back(runningCPU);
        runningCPU = NO_PROCESS;
    }

    //moves next process in ready queue to CPU
    if (!readyQueue.empty()){
        runningCPU = readyQueue.front();
        readyQueue.pop_front();
        processes[runningCPU].setState(RUNNING);
    }
}

void SimOS::TimerInterrupt(){
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");

    //removes process from CPU and back into ready queue
    processes[runningCPU].setState(READY);
    readyQueue.push_back(runningCPU);

    //moves next process in ready queue to CPU
    runningCPU = readyQueue.front();
    readyQueue.pop_front();
    processes[runningCPU].setState(RUNNING);
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName){
    if (diskNumber < 0 || diskNumber > numberOfDisks)
        throw std::out_of_range("Invalid disk number");
    if (runningCPU == NO_PROCESS)
        throw std::logic_error("No process is running");
    
    //adds request and process into io queue
    disks[diskNumber].addRequest({runningCPU, fileName});
    processes[runningCPU].setState(WAITING);
    runningCPU = NO_PROCESS;

    //moves next process in ready queue to CPU
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
    if (completedJob == 0)
        throw std::logic_error("No process exists");
    if (runningCPU == NO_PROCESS){
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
            int count = processes[runningCPU].getLastUsed() + 1;
            processes[runningCPU].setLastUsed(count);
            pageExists = true;
            break;
        }

        //empty frame found
        if (memory[i].PID == NO_PROCESS && emptyFrame == -1){
            emptyFrame = i;
        }
    }

    //locate the least recently used frame
    int LRUFRAME = -1;
    int LRUCOUNTER = INT_MAX;
    for(int i = 0; i < memory.size(); i++){
        if (processes[memory[i].PID].getLastUsed() < LRUCOUNTER){
            LRUCOUNTER = processes[memory[i].PID].getLastUsed();
            LRUFRAME = i;
        }
    }

    if(!pageExists){
        if (emptyFrame != -1){
            //puts process into memory
            memory[emptyFrame].PID = runningCPU;
            memory[emptyFrame].pageNumber = pageNumber;
            processes[runningCPU].setLastUsed(0);
        }
        else{
            //replaces process with least recently used frame
            memory[LRUFRAME].PID = runningCPU;
            memory[LRUFRAME].pageNumber = pageNumber;
            processes[runningCPU].setLastUsed(0);
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
    MemoryUsage temp;
    for (int i = 0; i < memory.size(); i++){
        if(memory[i].PID != NO_PROCESS)
            temp.push_back(memory[i]);
    }
    
    return temp;
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

void SimOS::removeMemory(int PID){
    for (int i = 0; i < memory.size(); i++){
        if (memory[i].PID == PID){
            memory[i].PID = NO_PROCESS;
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

void SimOS::cleanUpMemory(){
    if(runningCPU == NO_PROCESS && readyQueue.size() == 0)
        memory.resize(0);
}