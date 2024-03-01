#pragma once

#define PLOG_CHAR_IS_UTF8 1

#include "file.hpp"
#include "vendors/plog/Log.h"
#include "vendors/plog/Init.h"
#include "vendors/plog/Formatters/TxtFormatter.h"
#include "vendors/plog/Formatters/FuncMessageFormatter.h"
#include "vendors/plog/Appenders/ColorConsoleAppender.h"
#include "vendors/plog/Appenders/RollingFileAppender.h"

#include <list>
#include <vector>
#include <string>
#include <fstream>

struct Log {

    struct Appender : public plog::IAppender {

        struct Message {

            std::string msg;
            plog::Severity severity;
            plog::util::Time time;

        };

        Appender();

        virtual void write(const plog::Record& record) PLOG_OVERRIDE;

        std::vector<Message> list;
    };

    Appender appender;

    Log();

};;