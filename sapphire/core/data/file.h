#ifndef SAPPHIRE_FILE_H
#define SAPPHIRE_FILE_H

#include <string>
#include <vector>

class File {
public:
    enum FileType {
        FILE_TYPE_UNKNOWN,
        FILE_TYPE_FILE,
        FILE_TYPE_FOLDER
    };

    FileType type;
    std::string path;
    std::string name;

    // Used by folders
    std::vector<File> child_files;
};


#endif
