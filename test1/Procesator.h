//
// Created by cata-884 on 6/12/25.
//

#ifndef PROCESATOR_H
#define PROCESATOR_H

#include<string>
using namespace std;

class Procesator {
public:
    virtual ~Procesator() = 0;
    virtual void Run(int* data, int count) = 0;
    virtual string getName() = 0;
    virtual int getCount() = 0;
};



#endif //PROCESATOR_H
