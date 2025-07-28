//
// Created by chanbin on 25. 7. 24.
//

#ifndef SERVICE_H
#define SERVICE_H
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include "ConditionLSortLogger.h"
#include "ThreadPool.h"

using namespace std;

class Service {

private:
    std::unique_ptr<ConditionLSortLogger> logger = nullptr;

public:
    Service() = default;
    ~Service() = default;

    void Start();
    void SleepCout(string_view str, int ms = 100);
    void makeLogger(std::unique_ptr<ConditionLSortLogger> logger);

    bool StartContinueMessage();
    bool InputLoggerData();

};



#endif //SERVICE_H
