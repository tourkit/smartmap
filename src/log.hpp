#pragma once

#define PLOG_CHAR_IS_UTF8 1

#include "include/vendor/plog/Log.h"
#include "include/vendor/plog/Init.h"
#include "include/vendor/plog/Formatters/TxtFormatter.h"
#include "include/vendor/plog/Formatters/FuncMessageFormatter.h"
#include "include/vendor/plog/Appenders/ColorConsoleAppender.h"
#include "include/vendor/plog/Appenders/RollingFileAppender.h"

#include <list>
#include <vector>
#include <string>


namespace plog
{
    template<class Formatter> // Typically a formatter is passed as a template parameter.
    class MyAppender : public IAppender // All appenders MUST inherit IAppender interface.
    {
    public:
        virtual void write(const Record& record) PLOG_OVERRIDE // This is a method from IAppender that MUST be implemented.
        {
            util::nstring str = Formatter::format(record); // Use the formatter to get a string from a record.
             // Use the formatter to get a string from a record.

            m_messageList.push_back(std::string(str)); // Store a log message in a list.
        }

        std::vector<std::string>& getMessageList()
        {
            return m_messageList;
        }

    private:
        std::vector<std::string> m_messageList;
    };
}

struct Log {

    plog::RollingFileAppender<plog::TxtFormatter> fileAppender; 
    // plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; 

    
    plog::MyAppender<plog::FuncMessageFormatter> myAppender; // Create our custom appender.

    Log();

};;