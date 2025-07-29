//
// Created by chanbin on 25. 7. 24.
//

#include "../include/ConditionLSortLogger.h"

#include <regex>
#include <algorithm>
#include "../include/ThreadPool.h"
#include <fstream>
#include <chrono>

ConditionLSortLogger::ConditionLSortLogger(const std::string &_inputTxtPath,
                                           const std::string &_outputTxtFileName,
                                           std::vector<std::string> &&_regexs)
    : regexs(std::move(_regexs)), pool(std::make_unique<ThreadPool>())
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
    inputTxtPath = converter.from_bytes(_inputTxtPath);
    outputTxtFilePath = converter.from_bytes(_outputTxtFileName);
}


bool ConditionLSortLogger::ReadFile()
{
    std::wifstream file;

    try
    {
        file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;

        file.open(inputTxtPath);

        if (!file.is_open())
        {
            std::wcerr << L"File Open Fail: " << inputTxtPath << L"\n";
            return false;
        }

        std::wstringstream buffer;
        buffer << file.rdbuf();

        unsigned int count = 0;
        std::wstring line;
        while (std::getline(buffer, line))
        {
            count++;
            inputTxtFileContent.emplace_back(count, line);

            if (count % 100 == 0) // 몇 줄 단위로 쓰레드를 돌릴 것인가 본인 맘대로 하는게 좋을 듯
            {
                auto res = pool->enqueue([this, regexs = this->regexs, input = std::move(inputTxtFileContent)]()
                {
                    return this->Validate(regexs, input);
                });

                auto result = res.get();
                for (const auto &line: result) // vector 순회
                {
                    outputTxtFileContent.push_back(line);
                }

                inputTxtFileContent.clear();
            }
        }

        if (inputTxtFileContent.size() > 0)
        {
            auto res = pool->enqueue([this, regexs = this->regexs, input = std::move(inputTxtFileContent)]()
            {
                return this->Validate(regexs, input);
            });

            auto result = res.get();
            for (const auto &line: result)
            {
                outputTxtFileContent.push_back(line);
            }

            inputTxtFileContent.clear();
        }

        pool->stop = true;

        file.close();
    } catch (std::exception const &e)
    {
        if (file.is_open())
        {
            file.close();
        }
        std::cout << "Occur Error -> " << e.what() << "\n";
        return false;
    }

    return true;
}

std::vector<std::pair<unsigned int, std::wstring> > ConditionLSortLogger::Validate(
    const std::vector<std::string> &_regexs, std::vector<std::pair<unsigned int, std::wstring> > _inputTxtFileContent)
{
    std::vector<std::pair<unsigned int, std::wstring> > result;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;

    for (std::pair<unsigned int, std::wstring> &line: _inputTxtFileContent)
    {
        for (const std::string &regex: _regexs)
        {
            std::string lineStr = converter.to_bytes(line.second);
            std::smatch match;

            if (std::regex_search(lineStr, match, std::regex(regex)))
            {
                std::string highlighted = lineStr;
                std::string matchedText = match.str();
                size_t pos = highlighted.find(matchedText);

                highlighted.replace(pos, matchedText.length(),
                                    "<span>" + matchedText + "</span>");

                line.second = L"<p>" + converter.from_bytes(highlighted) + L"</p>";
                result.push_back(line);
                break;
            }
        }
    }

    return result;
}

void ConditionLSortLogger::styleSheet(std::wofstream &file)
{
    file << "<style>"
            "body {"
            "font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;"
            "background: #f5f8fa;" // 연한 화이트-블루
            "color: #2c3e50;" // 다크 네이비 텍스트
            "margin: 0;"
            "padding: 20px;"
            "min-height: 100vh;"
            "line-height: 1.6;"
            "}"

            "h1 {"
            "text-align: center;"
            "color: #1a2a3a;" // 진한 네이비
            "font-size: 2.4em;"
            "margin-bottom: 30px;"
            "font-weight: 600;"
            "}"

            ".container {"
            "max-width: 1200px;"
            "margin: 0 auto;"
            "background: #ffffff;" // 화이트 배경
            "border-radius: 10px;"
            "padding: 40px;"
            "box-shadow: 0 8px 24px rgba(0, 0, 0, 0.1);" // 부드러운 그림자
            "}"

            "span {"
            "background: #eaf3fb;" // 연한 네이비-화이트 배경
            "color: #1f4e79;" // 포인트 네이비
            "padding: 6px 12px;"
            "border-radius: 6px;"
            "font-weight: 600;"
            "border: 1px solid #d0e3f0;"
            "}"

            "p {"
            "font-size: 15px;"
            "padding: 18px 24px;"
            "margin: 8px 0;"
            "border-radius: 8px;"
            "border-left: 4px solid #d8e6f2;"
            "transition: all 0.25s ease;"
            "word-wrap: break-word;"
            "background: #f9fcff;" // 기본 배경
            "}"

            "p:nth-child(odd) {"
            "background: #f1f6fb;"
            "border-left-color: #c6dff2;"
            "}"

            "p:nth-child(even) {"
            "background: #f9fcff;"
            "border-left-color: #b9d7ed;"
            "}"

            "p:hover {"
            "background: #e6f0fa;"
            "border-left-color: #96c0e4;"
            "transform: translateX(4px);"
            "}"

            "p:hover span {"
            "background: #dbeaf7;"
            "color: #1d3e70;"
            "}"

            "::selection {"
            "background: #c8dbef;"
            "color: #000;"
            "}"

            "::-webkit-scrollbar {"
            "width: 10px;"
            "}"

            "::-webkit-scrollbar-track {"
            "background: #e9eff5;"
            "}"

            "::-webkit-scrollbar-thumb {"
            "background: #b0c9df;"
            "border-radius: 5px;"
            "}"

            "::-webkit-scrollbar-thumb:hover {"
            "background: #95b7d8;"
            "}"
            "</style>";
}

void ConditionLSortLogger::WriteWebFile()
{
    sort(outputTxtFileContent.begin(), outputTxtFileContent.end(),
         [](std::pair<unsigned int, std::wstring> a, std::pair<unsigned int, std::wstring> b) -> bool
         {
             return a.first < b.first;
         });

    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();


    try
    {
        std::wofstream file(outputTxtFilePath + std::to_wstring(millis) + L".html");

        styleSheet(file);

        file << "<h1>ConditionSortLogger</h1>" << std::endl;

        if (file.is_open())
        {
            file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

            for (auto i: outputTxtFileContent)
            {
                file << i.second << std::endl;
            }
        } else
        {
            std::wcout << L"File Make Fail" << std::endl;
        }
    } catch (std::exception const &e)
    {
        std::cout << "Occur Error -> " << e.what() << "\n";
    }
}
