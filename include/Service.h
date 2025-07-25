//
// Created by chanbin on 25. 7. 24.
//

#ifndef SERVICE_H
#define SERVICE_H
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

class Service {
public:
    Service()
    {

    }
    ~Service()
    {

    }

    void Start();
    void SleepCout(string_view str, int ms = 100);

    bool StartContinueMessage();
    bool InputLoggerData();

};



#endif //SERVICE_H
