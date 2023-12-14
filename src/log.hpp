#pragma once


#include "include/vendor/plog/Log.h"
#include "include/vendor/plog/Init.h"
#include "include/vendor/plog/Formatters/TxtFormatter.h"
#include "include/vendor/plog/Appenders/ColorConsoleAppender.h"
#include "include/vendor/plog/Appenders/RollingFileAppender.h"

struct Log {

    plog::RollingFileAppender<plog::TxtFormatter> fileAppender; 
    plog::ConsoleAppender<plog::TxtFormatter> consoleAppender; 
        
    enum // Define log instances. Default is 0 and is omitted from this enum.
    {
        SecondLog = 1
    };

    Log();

};