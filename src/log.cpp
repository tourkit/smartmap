#include "log.hpp"

#include "src/file.hpp"

Log::Log() : fileAppender(std::string(REPO_DIR+"logs.txt").c_str()) {

    plog::init(plog::debug, &consoleAppender);
    plog::init(plog::verbose, &fileAppender);
    
}