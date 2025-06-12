//
// Created by cata-884 on 6/12/25.
//

#ifndef SUMA_H
#define SUMA_H
#include"Procesator.h"


class Suma : Procesator{
    int suma = 0;
public:
    void Run(int *data, int count) override {
        for(int i=0; i<count; i++) {
            suma+=data[i];
        }
    }
    [[nodiscard]] int getCount() const {
        return suma;
    }
    string getName() override {
        return "Suma";
    }
};



#endif //SUMA_H
