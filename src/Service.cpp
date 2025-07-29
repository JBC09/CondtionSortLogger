//
// Created by chanbin on 25. 7. 24.
//

#include "../include/Service.h"

#include "../include/ConditionLSortLogger.h"

// 서비스가 시작됨
void Service::Start()
{
    if (StartContinueMessage())
    {
        if (InputLoggerData())
        {
            SleepCout("Reading file . . .");

            if (logger->ReadFile())
            {
                SleepCout("Writing file . . .");
                logger->WriteWebFile();
            }
        }
        else
        {
            logger = nullptr;
            Start();
        }
    }
}

// 프로그램의 사용 유무를 물어봄
bool Service::StartContinueMessage()
{
    string title = "Hello, Want to use ConditionSortLogger?";
    string description = "If you want to use the program, press ( 1 )\nelse if you want to exit, press ( 2 )";

    SleepCout(title);
    SleepCout(description);

    int input = 0;

    cin >> input;

    if (input == 1)
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }
    return false;
}


// 문자열의 문자를 ms 속도만큼 후에 출력시킴
void Service::SleepCout(string_view str, int ms)
{
    for (auto i: str)
    {
        cout << i << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "\n" << flush;
}


bool Service::InputLoggerData()
{
    vector<std::string> regexs;
    string getFilePath, outputFileName;
    int n;

    SleepCout("Enter the path to the desired log.txt file.");
    std::getline(cin, getFilePath);

    getFilePath.erase(std::remove(getFilePath.begin(), getFilePath.end(), '"'), getFilePath.end());

    SleepCout("Enter a positive number N and N regular expressions to use.");
    cin >> n;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    string regex;
    for (int i = 0; i < n; i++)
    {
        SleepCout( to_string(i+1)+". Enter a regular expression.");
        std::getline(cin, regex);
        regexs.push_back(regex);
    }

    SleepCout("Enter the desired data storage path.\nThe file name will be temporarily stored");
    std::getline(cin, outputFileName);

    outputFileName.erase(std::remove(outputFileName.begin(), outputFileName.end(), '"'), outputFileName.end());


    makeLogger(std::make_unique<ConditionLSortLogger>(getFilePath, outputFileName, std::move(regexs)));

    return true;
}


void Service::makeLogger(std::unique_ptr<ConditionLSortLogger> _logger)
{
    logger = std::move(_logger);
}


