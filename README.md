# OS Simulation

A simple operating system simulation in C++. This project simulates processes, disk operations, and core scheduling structures to mimic basic OS behavior.

---

## Table of Contents

- [Features](#features)  
- [Architecture / Components](#architecture--components)  
- [Building](#building)  
- [Usage](#usage)  
- [Example / Tests](#example--tests)  
- [Contributing](#contributing)  
- [License](#license)  

---

## Features

- Simulates processes with lifecycle management  
- Disk I/O simulation  
- Core OS simulation glue (creation, scheduling, dispatching)  
- Basic support files for processes, disks, and their headers  

---

## Architecture / Components

Here’s a breakdown of the source files and what they do:

| File | Purpose |
|---|---|
| `Disk.h` / `Disk.cpp` | Defines a `Disk` class / module: handles disk operations, perhaps queueing, read/write simulation. |
| `Process.h` / `Process.cpp` | Defines a `Process` class: holds process state, IDs, maybe memory usage or permissions, etc. |
| `SimOS.h` / `SimOS.cpp` | Main OS simulation class: coordinates `Process` and `Disk`, handles scheduling, time steps, etc. |
| `incomplete_test.cpp` | Probably a driver / test program which demonstrates or tests incomplete features. |

---

## Building

Requires a C++ compiler (C++11 or newer).

```bash
# Clone the repo
git clone https://github.com/MysliwiecAdrian/os-simulation.git
cd os-simulation

# Compile
g++ -std=c++11 SimOS.cpp Disk.cpp Process.cpp incomplete_test.cpp -o os_sim

# Or using separate compilation
g++ -std=c++11 -c SimOS.cpp Disk.cpp Process.cpp
g++ SimOS.o Disk.o Process.o incomplete_test.cpp -o os_sim
