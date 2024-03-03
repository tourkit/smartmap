#include "log.hpp"

Log::~Log() {}
Log::Log() {

    // plog::init(plog::verbose, &file); PLOGD << "init";
    plog::init(plog::verbose, &appender); 
    File::write("assets/logs/logs.txt", "");

}

Log::Appender::Appender() : plog::IAppender() { } // useless !

void Log::Appender::write(const plog::Record& record) { 
            
    list.push_back(Message{plog::FuncMessageFormatter::format(record), record.getSeverity(), record.getTime()}); 


    // post first line
    std::ifstream ifile(REPO_DIR+"assets/logs/logs.txt");
    std::stringstream buffer;
    buffer << plog::FuncMessageFormatter::format(record);
    if (cmd && record.getSeverity() != plog::Severity::verbose) std::cout << plog::FuncMessageFormatter::format(record);
    buffer << ifile.rdbuf();
    ifile.close();

    std::ofstream file(REPO_DIR+"assets/logs/logs.txt");
    file << buffer.rdbuf();
    file.close();


    // post last line
    // std::ofstream file(REPO_DIR+"assets/logs/logs.txt", std::ios_base::app);
    // if (file.is_open()) {
    //     file << plog::FuncMessageFormatter::format(record);
    //     file.close();
    // }

}