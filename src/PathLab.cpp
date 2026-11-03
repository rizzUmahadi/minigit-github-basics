#include "PathLab.hpp"
#include <fstream>

namespace minigit {
    std::filesystem::path metadataDirectory(const std::filesystem::path& root){
        return root / ".minigit";
    }

    std::filesystem::path headFilePath(const std::filesystem::path& root){
        return metadataDirectory(root) / "HEAD";
    }

    bool writeText(const std::filesystem::path& filePath, const std::string &text){
            std::filesystem::path parentPath = filePath.parent_path();
            if(!parentPath.empty()){
                std::error_code error; 
                std::filesystem::create_directories(parentPath, error);
                if (error) {
                    return false;
                }
            }
            std::ofstream file(filePath);
            if(!file.is_open()){
                return false;
            }
            file << text;
            if(file.good()){
                return true;
            }
            else{
            return false; 
            }
    }
} // namespace minigit
