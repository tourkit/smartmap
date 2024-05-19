#include "log.hpp"

Log::~Log() {}
Log::Log() {

    File::write("assets/logs/logs.txt", "");

    plog::init(plog::verbose, &appender);

}

Log::Appender::Appender() : plog::IAppender() { } // useless !

void Log::Appender::write(const plog::Record& record) {

    if (record.getSeverity() < plog::Severity::error) cmd = true;

    list.push_back(Message{plog::FuncMessageFormatter::format(record), record.getSeverity(), record.getTime()});

    // post first line
    std::ifstream ifile(File::loc()+"assets/logs/logs.txt");
    std::stringstream buffer;
    buffer << plog::FuncMessageFormatter::format(record);
    if (cmd) if (is_verbose || record.getSeverity() != plog::Severity::verbose) std::cout << plog::FuncMessageFormatter::format(record);
    buffer << ifile.rdbuf();
    ifile.close();

    std::ofstream file(File::loc()+"assets/logs/logs.txt");
    file << buffer.rdbuf();
    file.close();

}
