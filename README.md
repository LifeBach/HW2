# HW2: Two-way Min-cut Partitioning

## Introduction 
Let **_C_** be a set of cells and **_𝑵_** be a set of nets. Each net connects a subset of cells. The two-way min-cut partitioning problem is to partition the cell set into two groups **_A_** and **_B_**. The cost of a two-way partitioning is measured by the cut size, which is the number of nets having cells in both groups. 

## Problem Description
The two-way min-cut partitioning problem is defined asfollows:
1. **Input:**
    - A netlist for a circuit (.nets)
    - The size of each cell (.cells)
2. **Output:**
    - The final cut size and a partition result (.out)
3. **Objective:**
    - Partition the circuit in two sub-circuits **_𝑨_** and **_B_**, such that the cut size is minimized under the constraint of |_area_(**_𝑨_**) − _area_(**_B_**)| < \frac{**_n_**}{10} , where _area_(**_𝑨_**) is the sum of all cell sizes in **_𝑨_**, _area_(**_B_**) is the sum of all cell sizes in **_B_**, and **_n_** is the sum of all cell sizes in the circuit.

## Algorithm
Fiduccia-Mattheyses algorithm

