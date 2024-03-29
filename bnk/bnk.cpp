// bnk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cctype>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

struct fileStructure
{
    std::vector<char> name;
    std::vector<char> data;

    uint32_t dataOffset;
    uint32_t nameOffset;
    uint32_t fileSize;
};


int main(int argc, char** argv) // receive the number of arguments and the arguments
{
    std::cout << ".wav folder to Rez .bnk converter" << std::endl;

    bool selectedDirectory = false;

    fs::path directory;
    fs::path fileout;

    // check arguments for paths
    for (int i = 1; i < argc; ++i) // start at 1 to skip the process name
    {
        std::string argument = argv[i];

        if (!selectedDirectory && argument.length() > 0 && argument[0] != '-')
        {
            // first path argument
            if (fs::is_directory(argument))
            {
                directory = argument;

                directory = fs::weakly_canonical(directory);
                directory = directory.make_preferred();
                fileout = directory.parent_path();
                fileout.append(directory.filename().string() + ".bnk");
                std::cout << "Selected path: " << directory << std::endl;
                std::cout << "Autoselected file: " << fileout << std::endl;

                selectedDirectory = true;
            }
            else
            {
                std::cout << "Folder does not exist." << std::endl;
                return 2;
            }
        }
        else if (selectedDirectory && argument.length() > 0 && argument[0] != '-') {
            // second path argument
            fileout = argument;
            std::cout << "Selected file: " << fileout << std::endl;
        }
    }


    // User input and real directory check, if not passed as argument
    while (!selectedDirectory)
    {
        std::cout << "Enter folder:" << std::endl;

        std::cin >> directory;
        if (fs::is_directory(directory))
        {
            directory = fs::weakly_canonical(directory);
            directory = directory.make_preferred();
            fileout = directory.parent_path();
            fileout.append(directory.filename().string() + ".bnk");
            std::cout << "Selected path: " << directory << std::endl;
            std::cout << "Autoselected file: " << fileout << std::endl;
            char type;

            do
            {
                std::cout << "Are you sure you want to select this folder? [y/n]" << std::endl;
                std::cin >> type;
                type = tolower(type);

            } while (!std::cin.fail() && type != 'y' && type != 'n');
            if (type == 'y')
            {
                // checking if file already exists
                std::fstream checkExistsStream;
                checkExistsStream.open(fileout);
                if (checkExistsStream) {
                    do
                    {
                        std::cout << ".bnk exists here, do you want to overwrite it? [y/n]" << std::endl;
                        std::cin >> type;
                        type = tolower(type);
                    } while (!std::cin.fail() && type != 'y' && type != 'n');
                    if (type == 'y')
                    {
                        selectedDirectory = true;
                    }
                }
                else
                {
                    selectedDirectory = true;
                }
            }
        }
        else
        {
            std::cout << "Folder does not exist, try again." << std::endl;
        }
    }


    char zero[] = {0,0,0,0};
    uint32_t numberOfFiles = 0;
    uint32_t sizeOfNamesBlock = 0;


    uint32_t currentDataOffset = 0;
    uint32_t currentNameOffset = 0;
    std::vector<fileStructure> files;

    int i = 1;
    for (const fs::directory_entry entry : fs::directory_iterator(directory))
    {
        if (entry.path().extension() != ".bnk" && !entry.is_directory()) // skip any subfolders or .bnk files within the folder
        {
            fileStructure file;

            std::ifstream loadingStream (entry.path(), std::ios::in | std::ios::binary);
            if (!loadingStream.eof() && !loadingStream.fail())
            {
                std::vector<char> fileContents;
                loadingStream.seekg(0, std::ios_base::end);
                std::streampos fileSize = loadingStream.tellg();
                fileContents.resize(fileSize);

                loadingStream.seekg(0, std::ios_base::beg);
                if (fileSize > 0) {
                    loadingStream.read(&fileContents[0], fileSize);
                }
                file.data = fileContents;
                std::cout << fileSize << " bytes in ";
                file.fileSize = fileSize;
            }
            std::cout << entry.path() << std::endl;
            
            std::string fileNameString = entry.path().filename().string();
            std::vector<char> fileName(fileNameString.begin(), fileNameString.end());
            fileName.push_back(0); // adds ending 0x00 byte to name;
            file.name = fileName;

            file.dataOffset = currentDataOffset;
            file.nameOffset = currentNameOffset;

            currentDataOffset += file.data.size();
            currentNameOffset += file.name.size();

            numberOfFiles = i;

            files.push_back(file);
            i++;
        }
    }
    
    // round current name offset up to next 16 bytes to determine size of names block
    int namesBlockPadding = 16 - (currentNameOffset % 16);
    sizeOfNamesBlock = currentNameOffset + namesBlockPadding;

    std::cout << "Padding bytes: " << namesBlockPadding << std::endl;
    std::cout << "Writing to disk..." << std::endl;

    // Write the header

    std::ofstream fout (fileout, std::ios::out | std::ios::binary);

    fout.write((char*)&zero, sizeof zero);
    fout.write((char*)&numberOfFiles, sizeof numberOfFiles);
    fout.write((char*)&sizeOfNamesBlock, sizeof sizeOfNamesBlock);
    fout.write((char*)&zero, sizeof zero);

    fout.flush();

    // Write file&name offsets and file size
    for (fileStructure file : files)
    {
        fout.write((char*)&file.dataOffset, sizeof file.dataOffset);
        fout.write((char*)&file.nameOffset, sizeof file.nameOffset);
        fout.write((char*)&file.fileSize, sizeof file.fileSize);
        fout.write((char*)&zero, sizeof zero);

        fout.flush();
    }

    // Write file names
    for (fileStructure file : files)
    {
        for (char nameByte : file.name)
        {
            fout.write((char*)&nameByte, sizeof nameByte);
        }

        fout.flush();
    }
    
    // pad file name block
    for (int i = 0; i < namesBlockPadding; i++)
    {
        char zerobyte = 0;
        fout.write((char*)&zerobyte, sizeof zerobyte);

        fout.flush();
    }

    // Write file data
    for (fileStructure file : files)
    {
        for (char dataByte : file.data)
        {
            fout.write((char*)&dataByte, sizeof dataByte);
        }

        fout.flush();
    }

    fout.close();
    std::cout << "Done!" << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
