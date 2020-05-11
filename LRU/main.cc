//
//  main.cpp
//  paging
//
//  Created by Mahmoud Salem on 12/19/19.
//  Copyright © 2019 Mahmoud Salem. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

bool isValidPolicy(string policy) {
    if (policy == "OPTIMAL") {
        return true;
    }
    if (policy == "FIFO") {
        return true;
    }
    if (policy == "LRU") {
        return true;
    }
    if (policy == "CLOCK") {
        return true;
    }
    return false;
}

void readInput(int &bufferSize, string &policy, vector<int> &sequence) {
    cin >> bufferSize;
    assert(bufferSize);
    cin >> policy;
    assert(isValidPolicy(policy));
    int temp;
    while (true) {
        cin >> temp;
        if (temp == -1) {
            // if the value that has just been read is -1
            // that means that the sequence is over, hence, break the loop
            break;
        }
        sequence.push_back(temp);
    }
}

void printheader(string policy) {
    cout << "Replacement Policy = " << policy << endl;
    cout << "-------------------------------------" << endl;
    cout << "Page   Content of Frames" << endl;
    cout << "----   -----------------" << endl;
}

void printEntry(int value, bool fault, int buffer[], int size) {
    char f = fault ? 'F' : ' ';
    printf("%02d %c   ", value, f);
    for (int i = 0; i < size; i++) {
        printf("%02d ", buffer[i]);
    }
    cout << endl;
}

void printFooter(int faultCount) {
    cout << "-------------------------------------" << endl;
    cout << "Number of page faults = " << faultCount << endl;
}

inline bool inBuffer(int buffer[], int bufferSize, int value) {
    for (int i = 0; i < bufferSize; i++) {
        if (buffer[i] == value) {
            return true;
        }
    }
    return false;
}

inline int getIndexAfter(int value, vector<int> sequence, int start) {
    // a helping method for the OPTIMAL policy
    // gets the index of a certain value after a given index in a vector
    int size = (int) sequence.size();
    for (int i = start + 1; i < size; i++) {
        if (sequence.at(i) == value) {
            return i;
        }
    }
    return -1;
}

inline int optimalPolicy(int buffer[], int bufferSize, vector<int> sequence, int start) {
    int maximum = 0,
        optimal = 0,
        index;
    for (int i = 0; i < bufferSize; i++) {
        index = getIndexAfter(buffer[i], sequence, start);
        if (index == -1) {
            return i;
        }
        if (maximum < index) {
            maximum = index;
            optimal = i;
        }
    }
    return optimal;
}

inline int fifoPolicy(int buffer[], int bufferSize) {
    static int fifo = 0;
    if (fifo == bufferSize) fifo = 0;
    return fifo++;
}

inline int getIndexBefore(int value, vector<int> sequence, int end) {
    // a helping method for the LRU policy
    // gets the index of a certain value before a given index in a vector
    for (int i = end - 1; i >= 0; i--) {
        if (sequence.at(i) == value) {
            return i;
        }
    }
    return -1;
}

inline int lruPolicy(int buffer[], int bufferSize, vector<int> sequence, int end) {
    int minimum = end,
        lru = 0,
        index;
    for (int i = 0; i < bufferSize; i++) {
        index = getIndexBefore(buffer[i], sequence, end);
        if (minimum > index) {
            minimum = index;
            lru = i;
        }
    }
    return lru;
}

bool *setFlags(int bufferSize) {
    // allocates the flags used for the CLOCK policy
    // and setting them all to true
    bool *flags = new bool[bufferSize];
    for (int i = 0; i < bufferSize; i++) {
        flags[i] = true;
    }
    return flags;
}

inline int getIndex(int buffer[], int bufferSize, int value) {
    // a helping method that returns the index of value in the given buffer array
    for (int i = 0; i < bufferSize; i++) {
        if (buffer[i] == value)
            return i;
    }
    return -1;
}

inline int clockPolicy(int buffer[], int bufferSize, bool flags[]) {
    static int clockPointer = 0;
    int clock = 0;
    while (true) {
        if (clockPointer == bufferSize) {
            clockPointer = 0;
        }
        if (flags[clockPointer]) {
            flags[clockPointer++] = false;
        } else {
            flags[clockPointer] = true;
            clock = clockPointer++;
            break;
        }
    }
    return clock;
}

void run(string policy, int bufferSize, vector<int> sequence) {
    printheader(policy);
    int size = (int) sequence.size(),
        empty = bufferSize,
        buffer[bufferSize],
        lastInBuffer = 0,
        indexSequence = 0,
        faultCount = 0,
        value = 0,
        replace = 0;
    bool fault = false,
         *flags = (policy == "CLOCK") ? setFlags(bufferSize) : nullptr; // bool array for clock policy only
    // all policies are the same as long as no page faults occured, yet
    for (int i = 0; i < size; i++) {
        value = sequence.at(indexSequence++);
        if (!inBuffer(buffer, lastInBuffer, value)) {
            if (empty) {
                buffer[lastInBuffer++] = value;
                empty--;
                fault = false;
            } else {
                // here is where different policies act differently
                // a policy method is called only when a page fault occurs
                // so each method is to choose which element to replace from the buffer
                if (policy == "OPTIMAL")
                    replace = optimalPolicy(buffer, bufferSize, sequence, i);
                else if (policy == "FIFO")
                    replace = fifoPolicy(buffer, bufferSize);
                else if (policy == "LRU")
                    replace = lruPolicy(buffer, bufferSize, sequence, i);
                else { // policy == "CLOCK"
                    replace = clockPolicy(buffer, bufferSize, flags);
                }
                buffer[replace] = value;
                fault = true;
                faultCount += fault;
            }
        } else {
            fault = false;
            if (policy == "CLOCK") // updating the flag of that element in buffer just used
                flags[getIndex(buffer, bufferSize, value)] = true;
        }
        printEntry(value, fault, buffer, lastInBuffer);
    }
    printFooter(faultCount);
    delete flags;
}

void runProgram() {
    int bufferSize;
    string policy;
    vector<int> sequence;
    readInput(bufferSize, policy, sequence);
    run(policy, bufferSize, sequence);
}

int main(int argc, const char * argv[]) {
    runProgram();
    return 0;
}
