#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

bool isExecutable(const std::filesystem::path& path) {
    if (path.extension() != ".exe") {
        return false;
    }

#ifdef _WIN32
    return _access(path.string().c_str(), 0) == 0;
#else
    return access(path.c_str(), X_OK) == 0;
#endif
}

std::string findExeInDirec(const std::string& direc, const std::string& programName, std::vector<std::string>& suggestions) {
    std::string lowerProgramName = programName;
    std::transform(lowerProgramName.begin(), lowerProgramName.end(), lowerProgramName.begin(), ::tolower);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(direc)) {
        std::string filenameStem = entry.path().filename().stem().string();
        std::transform(filenameStem.begin(), filenameStem.end(), filenameStem.begin(), ::tolower);

        if (filenameStem == lowerProgramName && entry.path().extension() == ".exe" && isExecutable(entry.path())) {
            std::cout << "Found executable at: " << entry.path() << std::endl;
            suggestions.push_back(entry.path().string());
            return entry.path().parent_path().string(); // Return the parent directory
        }
    }

    std::cout << "No executable files found in subdirectories of: " << direc << std::endl;
    return "";
}

std::vector<std::string> findExeSuggestionsInDirec(const std::string& direc, const std::string& programName) {
    std::string lowerProgramName = programName;
    std::transform(lowerProgramName.begin(), lowerProgramName.end(), lowerProgramName.begin(), ::tolower);

    std::vector<std::string> suggestions;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(direc)) {
        std::string filenameStem = entry.path().filename().stem().string();
        std::transform(filenameStem.begin(), filenameStem.end(), filenameStem.begin(), ::tolower);

        if (filenameStem == lowerProgramName && entry.path().extension() == ".exe" && isExecutable(entry.path())) {
            suggestions.push_back(entry.path().string());
        }
    }

    return suggestions;
}

int main() {
    int startUp;
    std::cout << "To add a new library route, press 1\n"
        << "To launch a game or program, press 2\n"
        << "To view instructions, press 3\n"
        << "Enter input: ";
    std::cin >> startUp;

    std::fstream file("fileDirectories.csv", std::ios::app);
    std::string direcFileStr = "fileDirectories.csv";
    file.close();

    if (startUp == 1) {
        file.open(direcFileStr, std::ios::app);

        if (file.is_open()) {
            std::string addedDir;
            std::cout << "What is the file directory you'd like to add: ";
            std::cin.ignore();
            std::getline(std::cin, addedDir);

            file << addedDir << std::endl;

            file.close();
        }
        else {
            std::cerr << "Error opening the file." << std::endl;
        }
    }
    else if (startUp == 2) {
        file.open(direcFileStr);
        std::vector<std::string> fileDirVec;
        std::string line;

        while (std::getline(file, line)) {
            fileDirVec.push_back(line);
        }

        std::string programName;
        std::cout << "What is the name of the program you'd like to open: ";
        std::cin.ignore();
        std::getline(std::cin, programName);

        std::vector<std::string> appTitles;
        std::string subdirectory; 

        for (const auto& dir : fileDirVec) {
            subdirectory = findExeInDirec(dir, programName, appTitles);
            if (!subdirectory.empty()) {
                std::transform(appTitles.back().begin(), appTitles.back().end(), appTitles.back().begin(), ::tolower);
            }
        }

        std::cout << "Subdirectory: " << subdirectory << std::endl; 

        std::cout << "File direct front: " << fileDirVec.front() << std::endl;
        for (int i = 0; i < fileDirVec.size(); i++) {
            std::cout << "File direct: " << fileDirVec[i] << std::endl;
        }
        std::cout << "Program name: " << programName << std::endl;

        std::vector<std::string> suggestions = findExeSuggestionsInDirec(subdirectory, programName);

        for (int j = 0; j < suggestions.size(); j++) {
            std::cout << "Suggestion: " << suggestions[j] << std::endl;
        }

        if (!suggestions.empty()) {
            std::cout << "Executable files found in directory: \n";
            for (const auto& suggestion : suggestions) {
                std::cout << "--" << suggestion << std::endl;
            }

            return 0;
        }
        else {
            std::cout << "No executable files found in directory\n";
        }
    }

    else if (startUp == 3) {

    }
    else {
        std::cout << "Invalid input\n";
    }

    return 0;
}
