#include "log.hpp"

#include "src/file.hpp"

Log::Log() : fileAppender((REPO_DIR+"logs.txt").c_str()) {

    plog::init(plog::debug, &consoleAppender);
    plog::init<SecondLog>(plog::verbose, &fileAppender);
    
}