#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <Windows.h>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

std::string findExeInDirec(const std::string& direc, const std::string programName) {
	std::string lowerProgramName = programName;
	std::transform(lowerProgramName.begin(), lowerProgramName.end(), lowerProgramName.begin(), ::tolower);

	for (const auto& entry : std::filesystem::recursive_directory_iterator(direc)) {
		std::string filenameStem = entry.path().filename().stem().string(); 
		std::transform(filenameStem.begin(), filenameStem.end(), filenameStem.begin(), ::tolower);
		if (filenameStem == lowerProgramName) {
			std::string entryPathway = entry.path().string();
			return entryPathway;
		}
	}
	return "";
}

std::vector<std::string> findSuggInDirec(const std::string& entryPathway, const std::string& programName) {
	std::vector<std::string> suggestions;

	for (const auto& entry : std::filesystem::directory_iterator(entryPathway)) {
		if (entry.is_regular_file() && (entry.path().extension() == ".exe")) {
			suggestions.push_back(entry.path().filename().string());
		}
	}
    return suggestions;
}

void launchProgram(const std::string& launchDirec, const std::string& selectedProgram) {
    std::wstring launchCmd;

    if (selectedProgram.find(':') != std::string::npos) {
        launchCmd = L"\"" + std::wstring(selectedProgram.begin(), selectedProgram.end()) + L"\"";
    }
    else {
        launchCmd = L"\"" + std::wstring(launchDirec.begin(), launchDirec.end()) +
            L"\\" + std::wstring(selectedProgram.begin(), selectedProgram.end()) + L"\"";
    }

    std::wcout << L"Launch cmd: " << launchCmd << std::endl << std::endl;

    ShellExecuteW(NULL, NULL, launchCmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


int main() {
    std::system(CLEAR_SCREEN);

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

            for (int p = 0; p < appTitles.size(); p++) {
                std::cout << "app titles: " << appTitles[p] << std::endl;
            }
            

            if (!appTitles.empty() && appTitles.front().substr(appTitles.front().size() - 4) != ".exe") {

                std::vector<std::string> suggestions = findSuggInDirec(appTitles.front(), programName);

                for (int p = 0; p < suggestions.size(); p++) {
                    std::cout << "suggestions: " << suggestions[p] << std::endl;
                }

                if (!suggestions.empty()) {

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

                        launchProgram(launchDirectory, selectedProgram);

                        runPg = false;
                        break;
                    }
                    else {
                        std::cout << "Invalid choice.\n" << std::endl;
                    }
                }
            }
            else if (appTitles.front().substr(appTitles.front().size() - 4) == ".exe") {
                std::string selectedProgram = appTitles.front();
                std::string launchDirec = appTitles.front().substr(0, appTitles.front().size());

                std::cout << "Launch direc: " << launchDirec << std::endl;

                launchProgram(launchDirec, selectedProgram);

                runPg = false;
                break;
            }
            else {
                std::cout << "There is some error here. I am not sure what is causing it, but it does exist\n"
                          << "Good luck soldier.\n";
            }
        }
        else if (startUp == 3) {
            std::cout << "\n"
                << "This program is intended to make launching programs easy from a CLI.\n"
                << "To add new files to the directory, you must go to where the .exe to launch is located, or the file just above it\n"
                << "For example, with steam you should add the steamapps\\common directory (meaning the full file location) into the system\n"
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