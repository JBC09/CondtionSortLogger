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

                // 굵은 주황색 (더 눈에 띔)
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

        // styling
        file << "<style>"
        "body { "
            "font-family: 'JetBrains Mono', 'Consolas', 'Monaco', monospace; "
            "background: linear-gradient(135deg, #1e1e2e 0%, #2d3748 100%); "
            "color: #e4e4e7; "
            "margin: 0; "
            "padding: 20px; "
            "min-height: 100vh; "
            "line-height: 1.6; "
        "} "

        "h1 { "
            "text-align: center; "
            "color: #fbbf24; "
            "font-size: 2.5em; "
            "margin-bottom: 30px; "
            "text-shadow: 0 0 20px rgba(251, 191, 36, 0.3); "
            "font-weight: 300; "
        "} "

        ".container { "
            "max-width: 1200px; "
            "margin: 0 auto; "
            "background: rgba(15, 23, 42, 0.8); "
            "border-radius: 15px; "
            "padding: 30px; "
            "box-shadow: 0 25px 50px rgba(0, 0, 0, 0.3), "
                        "0 0 0 1px rgba(255, 255, 255, 0.05); "
            "backdrop-filter: blur(10px); "
        "} "

        "span { "
            "background: linear-gradient(45deg, #f97316, #ea580c); "
            "color: #fff; "
            "padding: 4px 8px; "
            "border-radius: 6px; "
            "font-weight: 600; "
            "box-shadow: 0 4px 15px rgba(249, 115, 22, 0.4), "
                        "inset 0 1px 0 rgba(255, 255, 255, 0.2); "
            "text-shadow: 0 1px 2px rgba(0, 0, 0, 0.3); "
            "position: relative; "
            "display: inline-block; "
            "animation: glow 2s ease-in-out infinite alternate; "
        "} "

        "@keyframes glow { "
            "from { box-shadow: 0 4px 15px rgba(249, 115, 22, 0.4), "
                               "inset 0 1px 0 rgba(255, 255, 255, 0.2); } "
            "to { box-shadow: 0 4px 25px rgba(249, 115, 22, 0.6), "
                             "inset 0 1px 0 rgba(255, 255, 255, 0.3), "
                             "0 0 30px rgba(249, 115, 22, 0.3); } "
        "} "

        "p { "
            "font-size: 13px; "
            "padding: 15px 20px; "
            "margin: 8px 0; "
            "border-radius: 10px; "
            "border-left: 4px solid transparent; "
            "transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); "
            "position: relative; "
            "word-wrap: break-word; "
        "} "

        "p:nth-child(odd) { "
            "background: linear-gradient(90deg, "
                        "rgba(30, 41, 59, 0.6) 0%, "
                        "rgba(30, 41, 59, 0.3) 100%); "
            "border-left-color: #3b82f6; "
        "} "

        "p:nth-child(even) { "
            "background: linear-gradient(90deg, "
                        "rgba(15, 23, 42, 0.8) 0%, "
                        "rgba(15, 23, 42, 0.4) 100%); "
            "border-left-color: #10b981; "
        "} "

        "p:hover { "
            "transform: translateX(10px) scale(1.01); "
            "box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2), "
                        "0 0 0 1px rgba(255, 255, 255, 0.1); "
            "border-left-width: 6px; "
        "} "

        "p:hover span { "
            "transform: scale(1.05); "
            "box-shadow: 0 6px 20px rgba(249, 115, 22, 0.6), "
                        "inset 0 1px 0 rgba(255, 255, 255, 0.3), "
                        "0 0 40px rgba(249, 115, 22, 0.4); "
        "} "

        ".log-entry::before { "
            "content: '→'; "
            "position: absolute; "
            "left: -15px; "
            "top: 50%; "
            "transform: translateY(-50%); "
            "color: #64748b; "
            "opacity: 0; "
            "transition: opacity 0.3s ease; "
        "} "

        "p:hover::before { "
            "opacity: 1; "
        "} "

        "::selection { "
            "background: rgba(249, 115, 22, 0.3); "
            "color: #fff; "
        "} "

        "::-webkit-scrollbar { width: 12px; } "
        "::-webkit-scrollbar-track { "
            "background: rgba(15, 23, 42, 0.5); "
            "border-radius: 6px; "
        "} "
        "::-webkit-scrollbar-thumb { "
            "background: linear-gradient(180deg, #f97316, #ea580c); "
            "border-radius: 6px; "
            "border: 2px solid rgba(15, 23, 42, 0.5); "
        "} "
        "::-webkit-scrollbar-thumb:hover { "
            "background: linear-gradient(180deg, #ea580c, #c2410c); "
        "} "

        "@media (max-width: 768px) { "
            "body { padding: 10px; } "
            ".container { padding: 20px; } "
            "h1 { font-size: 2em; } "
            "p { font-size: 12px; padding: 12px 15px; } "
        "} "
        "</style>";

        file << "<h1>ConditionSortLogger By Chanbin</h1>" << std::endl;

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
