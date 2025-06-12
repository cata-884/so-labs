#include <iostream>
#include "MyVector.h"
#include "Suma.h"
#include "Find.h"

int main()
{
    MyVector m;
    ((((((m += 1) += 2) += 3) += new Suma()) += 4) += 2);
    m += new Find([](int i) { return i == 2; });
    std::cout << "Elemente    : " << (int)m << std::endl;
    std::cout << "Procesatori : " << (unsigned int)m << std::endl;
    m.process();
    m.clear();
    (((m += {10, 10, 10, 10, 50}) += 2) += new Find([](int i) { return i == 10; })) += 50;
    std::cout << "-----------------------" << std::endl;
    std::cout << "Elemente    : " << (int)m << std::endl;
    std::cout << "Procesatori : " << (unsigned int)m << std::endl;
    m.process();
    return 0;
}

