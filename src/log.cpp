#include "log.hpp"

#include "pch.hpp"
#include "src/file.hpp"

Log::Log() : fileAppender(std::string(REPO_DIR+"logs.txt").c_str()) {

    plog::init(plog::verbose, &consoleAppender);
    plog::init(plog::debug, &fileAppender);
    
}