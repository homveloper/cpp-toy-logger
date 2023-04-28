#pragma once

#ifndef CPP_TOY_LOGGER_HPP
#define CPP_TOY_LOGGER_HPP

#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <thread>
#include <condition_variable>
#include <queue>

enum ELogLevel 
{
    INFO,
    WARNING,
    ERROR,
};

template<class LogerImpl>
class CLoggerBase
{
public:
    CLoggerBase() : m_stop(false)
    {
        m_thread = std::thread(&CLoggerBase::_Run, this);
        m_BatchSize = 1;
    }

    CLoggerBase(const int batchSize) : m_stop(false)
    {
        m_thread = std::thread(&CLoggerBase::_Run, this);
        m_BatchSize = batchSize;
    }
    ~CLoggerBase()
    {
        EndThread();
    }

    void EndThread()
    {
        if(m_stop == true)
        {
            return;
        }

        // TODO
        // 큐에 남은 로그가 있을 경우, 로그를 모두 출력하고 종료해야 한다.

        m_stop = true;
        m_cv.notify_all();
        m_thread.join();
    }

    void Log(const ELogLevel level, const std::wstring& message)
    {
        std::unique_lock<std::mutex> lock(m_queryMutex);
        m_logQueryQueue.push({ _GetLogHeader(level), message });
        m_cv.notify_one();
    }

    constexpr static const wchar_t* GetLogLevelName(const ELogLevel level)
    {
        switch (level)
        {
        case INFO:
            return L"INFO";
        case WARNING:
            return L"WARNING";
        case ERROR:
            return L"ERROR";
        default:
            return L"UNKNOWN";
        }
    }

protected:
    void _Run()
    {
        while(true)
        {
            std::unique_lock<std::mutex> lock(m_queryMutex);
            m_cv.wait(lock, [this] { return !m_logQueryQueue.empty() || m_stop; });
            if (m_stop)
            {
                break;
            }

            if(m_BatchSize == 1)
            {
                LogQuery query = m_logQueryQueue.front();
                m_logQueryQueue.pop();

                static_cast<LogerImpl*>(this)->_LogImpl(query.header, query.message);
            }
            else
            {
                std::vector<LogQuery> logQueryArray;
                logQueryArray.reserve(m_BatchSize);
                while (!m_logQueryQueue.empty() &&
                        logQueryArray.size() <= m_BatchSize)
                {
                    LogQuery query = m_logQueryQueue.front();
                    m_logQueryQueue.pop();

                    logQueryArray.push_back(query);
                }

                static_cast<LogerImpl*>(this)->_LogBatchImpl(logQueryArray);
            }
        }
    }

    std::wstring _GetLogHeader( const ELogLevel level)
    {
        auto now = std::chrono::system_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        std::time_t nowT = std::chrono::system_clock::to_time_t(now);

        wchar_t timeStamp[100];
        std::wcsftime(timeStamp, sizeof(timeStamp), L"%Y-%m-%d %H:%M:%S", std::localtime(&nowT));

        std::wstringstream headerOss;
        headerOss << L'[' << timeStamp << L"." << std::setw(3) << std::setfill(L'0') << milliseconds << L"][" << GetLogLevelName(level) << L']';
        return headerOss.str();
    }

    struct LogQuery
    {
        std::wstring header;
        std::wstring message;
    };

private:
    std::thread m_thread;
    std::queue<LogQuery> m_logQueryQueue;
    std::mutex m_queryMutex;
    std::condition_variable m_cv;
    bool m_stop;

    // config
    int m_BatchSize;
};

class CConsoleLogger : public CLoggerBase<CConsoleLogger>
{
public:
    CConsoleLogger(const int batchSize) : CLoggerBase<CConsoleLogger>(batchSize) {}

    void _LogImpl(const std::wstring& header, const std::wstring& message)
    {
        std::wcout << header << L" " << message << std::endl;
    }

    void _LogBatchImpl(const std::vector<LogQuery>& rLogQueryArray)
    {
        std::wstring logString;
        for (const LogQuery& query : rLogQueryArray)
        {
            logString += query.header + L" " + query.message + L"\n";
        }
        
        std::wcout << logString;
    }
};

#endif //CPP_TOY_LOGGER_HPP
