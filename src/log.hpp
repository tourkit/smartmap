#pragma once

#define PLOG_CHAR_IS_UTF8 1

#include "file.hpp"
#include "vendors/plog/Log.h"
#include "vendors/plog/Init.h"
#include "vendors/plog/Util.h"

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

        bool cmd = false;
        bool is_verbose = false;

        struct Message {

            std::string msg;
            plog::Severity severity;
            plog::util::Time time;
            int id = 0;

        };

        Appender();

        virtual void write(const plog::Record& record) PLOG_OVERRIDE;

        std::vector<Message> list;
    };

    Appender appender;

    bool cout(bool is_verbose = false) { appender.cmd = true; appender.is_verbose = is_verbose; return is_verbose;}

    static Log& getInstance() { static Log instance;  return instance; }

private:

    std::string project_name;

    Log();

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    ~Log();

};

static inline Log &logger = Log::getInstance();
