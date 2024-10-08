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

#include <deque>
#include <string>
#include <fstream>

using Sev = plog::Severity;
// using plog::Severity::debug;
// using plog::Severity::error;
// using plog::Severity::verbose;
// using plog::Severity::none;
// using plog::Severity::fatal;
// using plog::Severity::warning;
// using plog::Severity::info;

struct Node;

struct Log {

    struct Appender : public plog::IAppender {

        bool cmd = false;
        plog::Severity severity = plog::Severity::none, max_severity = plog::Severity::error;

        struct Message {

            std::string msg;
            plog::Severity severity;
            plog::util::Time time;
            uint32_t id = 0;

        };

        Appender();

        virtual void write(const plog::Record& record) PLOG_OVERRIDE;

        std::deque<Message> list;
    };

    Appender appender;

    void cout(plog::Severity max_severity = plog::Severity::error, plog::Severity severity = plog::Severity::debug) { appender.cmd = true; appender.severity = severity; appender.max_severity = max_severity; }

    void cout( int max_severity = 5,  int severity = 2) { appender.cmd = true; appender.severity = plog::Severity(severity); appender.max_severity = plog::Severity(max_severity); }

    static Log& getInstance() { static Log instance;  return instance; }

private:

    std::string project_name;

    Log();

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    ~Log();

};

static inline Log &logger = Log::getInstance();
