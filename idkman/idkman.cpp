#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <Windows.h>

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

    bool runPg = true;

    while (runPg) {

        int startUp;
        std::cout << "To add a new library route, press 1\n"
            << "To launch a game or program, press 2\n"
            << "To view instructions, press 3\n"
            << "To quit out, press 4\n"
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
                std::cout << std::endl << std::endl;
            }
            else {
                std::cerr << "Error opening the file." << std::endl;
                std::cout << std::endl;
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

                        std::wstring launchCommand = L"\"" + std::wstring(launchDirectory.begin(), launchDirectory.end()) + L"\\" + std::wstring(selectedProgram.begin(), selectedProgram.end()) + L"\"";
                        std::wcout << L"launch command: " << launchCommand << std::endl;

                        // Use ShellExecuteW to launch the process without a command prompt window
                        ShellExecuteW(NULL, NULL, launchCommand.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    }
                    else {
                        std::cout << "Invalid choice. Exiting...\n";
                    }
                }
                else {
                    std::cout << "No other .exe files found in the same directory\n"
                        << "Some common errors here could be that the launcher or .exe application is located in a subdirectory\n"
                        << "Please check the file directory and add the correct route\n"
                        << "For more information, view the instruction list\n";
                }
            }
        }
        else if (startUp == 3) {
            std::cout << "\n"
                << "This program is intended to make launching programs easy from a CLI.\n"
                << "To add new files to the directory, you must go to where the .exe to launch is located, or the file just above it\n"
                << "For example, with steam you should add the steamapps\\common directory(meaning the full file location) into the system\n"
                << "For certain games where the launcher is located deeper within the files, you will need to add the game file location in\n"
                << "For example of this, look at Baldurs Gate 3. The launcher is located in the launcher file within the game file\n"
                << "So, for the program to access it the file directory must be to the common\\Baldurs Gate 3 file, which will allow the system to search deeper\n"
                << "If there are typos within the way you typed the file name, it will not launch.\n"
                << "The program is not case sensitive, but it does require proper spelling as this is a file system, and files can be difficult to find with typos\n"
                << "So, if you get no output when you type something into the CLI, make sure your spelling was correct\n";
            std::cout << std::endl << std::endl;
        }
        else if (startUp == 4) {
            runPg = false;
            break;
        }
        else {
            std::cout << "Invalid input\n";
        }
    }

    return 0;
}
