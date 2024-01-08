#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

std::string findExeInDirec(const std::string& direc, const std::string& programName) {
    std::string lowerProgramName = programName;
    std::transform(lowerProgramName.begin(), lowerProgramName.end(), lowerProgramName.begin(), ::tolower);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(direc)) {
        std::string filenameStem = entry.path().filename().stem().string();
        std::transform(filenameStem.begin(), filenameStem.end(), filenameStem.begin(), ::tolower);

        if (filenameStem == lowerProgramName) {
            std::string entryPathway = entry.path().string();
            std::cout << "Entry pathway: " << entryPathway << std::endl;
            return entryPathway;
        }
    }
    return "";
}

std::vector<std::string> findSuggInDirec(const std::string& entryPathway, const std::string& programName) {
    std::vector<std::string> suggestions;

    for (const auto& entry : std::filesystem::directory_iterator(entryPathway)) {
        if (entry.is_regular_file() && entry.path().extension() == ".exe") {
            suggestions.push_back(entry.path().filename().string());
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
        for (const auto& dir : fileDirVec) {
            std::filesystem::path executablePath = findExeInDirec(dir, programName);
            if (!executablePath.empty()) {
                appTitles.push_back(executablePath.string());
            }
        }

        if (!appTitles.empty()) {
            std::vector<std::string> suggestions = findSuggInDirec(appTitles.front(), programName);

            if (!suggestions.empty()) {

                std::cout << "Size of suggestions: " << suggestions.size() << std::endl;

                std::cout << "Here are some suggestions from the same directory: \n";
                for (int k = 0; k < suggestions.size(); k++) {
                    int suggNumber = k + 1;
                    std::cout << suggNumber << ": " << suggestions[k] << std::endl << std::endl;
                }

                int prgChoice;
                std::cout << "If you would like to launch one of these programs, type in the number corresponding to the program: ";
                std::cin >> prgChoice;

                if (prgChoice > 0 && prgChoice <= suggestions.size()) {
                    std::string selectedProgram = suggestions[prgChoice - 1];
                    std::string launchDirectory = appTitles.front();

                    std::string launchCommand = "start \"" + launchDirectory + "\\" + selectedProgram + "\"";
                    std::cout << "launch command: " << launchCommand << std::endl;

                    system(launchCommand.c_str());
                }
                else {
                    std::cout << "Invalid choice. Exiting...\n";
                }
            }
            else {
                std::cout << "No other .exe files found in the same directory\n";
            }
        }
    }
    else if (startUp == 3) {

    }
    else {
        std::cout << "Invalid input\n";
    }

    return 0;
}
