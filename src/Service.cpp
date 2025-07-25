//
// Created by chanbin on 25. 7. 24.
//

#include "../include/Service.h"



void Service::Start()
{
    if (StartContinueMessage())
    {

    }
}


bool Service::StartContinueMessage()
{
    string title = "Hello, Want to use  conditionSortLogger?";
    string description = "If you want to use the program, press ( 1 )\nelse if you want to exit, press ( 2 )";

    SleepCout(title);
    SleepCout(description);

    int input = 0;

    cin >> input;

    if (input == 1)
    {
        return true;
    }
    return false;

}


void Service::SleepCout(string_view str, int ms)
{
    for (auto i: str)
    {
        cout << i << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "\n\n" << flush;
}


bool Service::InputLoggerData()
{
    vector<std::string> regexs;
    string getFilePath, outputFileName;
    int n;

    SleepCout("Enter the path to the desired log.txt file.");
    cin >> getFilePath;
    SleepCout("Enter a positive number N and N regular expressions to use.");
    cin >> n;

    string regex;
    for (int i = 0; i < n; i++)
    {
        SleepCout( to_string(i+1)+"                ");
        cin >> regex;
        regexs.push_back(regex);
    }

    SleepCout("Enter the desired data storage file name");
    cin >> outputFileName;

}