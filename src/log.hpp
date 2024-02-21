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

struct Log {

    struct Appender : public plog::IAppender {

        struct Message {

            std::string msg;
            plog::Severity severity;
            plog::util::Time time;

        };

        virtual void write(const plog::Record& record) PLOG_OVERRIDE { 
            
            list.push_back(Message{plog::FuncMessageFormatter::format(record), record.getSeverity(), record.getTime()}); 

        }

        std::vector<Message> list;
    };

    Appender appender;

    Log();

};;