//
// Created by chanbin on 25. 7. 24.
//

#ifndef CONDITIONLSORTLOGGER_H
#define CONDITIONLSORTLOGGER_H
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <iostream>
#include <vector>

#include "ThreadPool.h"


class ConditionLSortLogger {

private:
    std::unique_ptr<ThreadPool> pool = nullptr;
    std::vector<std::string> regexs_;

public:
    std::wstring inputTxtPath;
    std::vector<std::pair<unsigned int, std::wstring>> inputTxtFileContent;
    std::vector<std::pair<unsigned int, std::wstring>> outputTxtFileContent;
    std::wstring outputTxtFilePath;

    std::vector<std::string> regexs;

    explicit ConditionLSortLogger(const std::string& _inputTxtPath, const std::string& _outputTxtFileName, std::vector<std::string>&& _regexs);
    ~ConditionLSortLogger() = default;
    bool ReadFile();
    std::vector<std::pair<unsigned int, std::wstring>> Validate(const std::vector<std::string>& _regexs, std::vector<std::pair<unsigned int, std::wstring>> _inputTxtFileContent);
    void Threading();
    void WriteWebFile();
    void styleSheet(std::wofstream &file);
};


#endif //CONDITIONLSORTLOGGER_H
