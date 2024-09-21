#include "log.hpp"
#include "engine.hpp"

Log::~Log() {}
Log::Log() {

    File::write("build/logs/logs.txt", "");

    plog::init(plog::verbose, &appender);

}

Log::Appender::Appender() : plog::IAppender() { } // useless !

void Log::Appender::write(const plog::Record& record) {

    if (record.getSeverity() <= plog::Severity::error){
    
        cmd = true;
        
    }

    list.push_back(Message{plog::FuncMessageFormatter::format(record), record.getSeverity(), record.getTime(), (int)list.size() });

    // post first line
    std::ifstream ifile(File::loc()+"build/logs/logs.txt");
    std::stringstream buffer;
    buffer << plog::FuncMessageFormatter::format(record);
    if (cmd) 
        if (record.getSeverity() <= max_severity) {
            std::string cout = plog::FuncMessageFormatter::format(record);
            if (cout.length() > 200) 
                cout = cout.substr(0,200) + "...\n";

            if (engine.gui_v && engine.gui_v->editors.back()->selected != engine.tree->find("debug"))
                engine.gui_v->editors.push_back(std::make_shared< EditorWidget>(engine.gui_v, engine.tree->find("debug")));
            
            // std::cout << cout;
    }
    
    buffer << ifile.rdbuf();
    ifile.close();

    std::ofstream file(File::loc()+"build/logs/logs.txt");
    file << buffer.rdbuf();
    file.close();

}
