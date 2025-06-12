//
// Created by cata-884 on 6/12/25.
//

#ifndef FIND_H
#define FIND_H
#include"Procesator.h"
#include<functional>
#include <utility>

class Find : Procesator{
    function<bool(int)> predicate;
    int count;
public:
    explicit Find(function<bool(int)> f, const int c) : predicate(std::move(f)), count(c) {}
    void Run(int *data, int count) override {
        for(int i=0; i<count; i++) {
            if(predicate(data[i])) count++;
        }
    }
    [[nodiscard]] int getCount() const {
        return count;
    }
    string getName() override {
        return "Find";
    }
};



#endif //FIND_H
