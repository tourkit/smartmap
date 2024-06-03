#pragma once

struct File;

struct Config{

    bool open(File* file);
    bool save(File* file);

};
