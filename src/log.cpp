#include "log.hpp"

Log::Log() {

    plog::init(plog::verbose, &appender); 

    
}