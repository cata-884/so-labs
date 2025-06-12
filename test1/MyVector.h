
//
// Created by cata-884 on 6/12/25.
//

#ifndef MYVECTOR_H
#define MYVECTOR_H
#include<vector>
#include"Procesator.h"
#include<memory>
#include<initializer_list>
class MyVector {
    vector<int> Data;
    vector<unique_ptr<Procesator>> procesators;
public:
    MyVector() = default;
    MyVector& operator+=(int n) {
        Data.push_back(n);
        return *this;
    }
    MyVector& operator+=(const unique_ptr<Procesator> &proc) {
        procesators.push_back(proc);
        return *this;
    }
    MyVector& operator+=(initializer_list<int> l) {
        for(int i=0; i<l.size(); i++) {
            Data.push_back(*(l.begin()+i));
        }
        return *this;
    }

    explicit operator int() const {
        return static_cast<int>(Data.size());
    }
    explicit operator unsigned int() const {
        return static_cast<int>(procesators.size());
    }
    void clear() {
        Data.clear();
    }
    void process() {
        //print?
        for(auto processor : procesators) {
            processor->Run(Data.data(), static_cast<int>(Data.size()));
            cout<< processor->getName() + to_string(processor->getCount())<<endl;
        }
    }
};



#endif //MYVECTOR_H
