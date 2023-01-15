// bnk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

struct fileStructure
{
    vector<char> name;
    vector<char> data;

    uint32_t dataOffset;
    uint32_t nameOffset;
    uint32_t fileSize;
};

int main()
{
    cout << ".wav folder to Rez .bnk converter" << endl;


    fs::path directory;
    fs::path fileout;
    bool selectedDirectory = false;

    // User input and real directory check
    while (!selectedDirectory)
    {
        cout << "Enter folder:" << endl;

        cin >> directory;
        if (fs::is_directory(directory))
        {
            directory = fs::weakly_canonical(directory);
            directory = directory.make_preferred();
            fileout = directory;
            fileout.append("bankfile.bnk");
            cout << "Selected path: " << directory << endl;
            cout << "Selected file: " << fileout << endl;
            char type;
            do
            {
                cout << "Are you sure you want to select this folder? [y/n]" << endl;
                cin >> type;
            } while (!cin.fail() && type != 'y' && type != 'n');
            if (type == 'y')
            {
                // checking if file already exists
                fstream checkExistsStream;
                checkExistsStream.open(fileout);
                if (checkExistsStream) {
                    do
                    {
                        cout << ".bnk exists here, do you want to overwrite it? [y/n]" << endl;
                        cin >> type;
                    } while (!cin.fail() && type != 'y' && type != 'n');
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
            cout << "Folder does not exist, try again." << endl;
        }
    }


    char zero[] = {0,0,0,0};
    uint32_t numberOfFiles = 0;
    uint32_t sizeOfNamesBlock = 0;


    uint32_t currentDataOffset = 0;
    uint32_t currentNameOffset = 0;
    vector<fileStructure> files;

    int i = 1;
    for (const fs::directory_entry entry : fs::directory_iterator(directory))
    {
        if (entry.path().extension() != ".bnk" && !entry.is_directory()) // ignore a bnk file if it already exists and skip subfolders
        {
            fileStructure file;

            ifstream loadingStream (entry.path(), ios::in | ios::binary);
            if (!loadingStream.eof() && !loadingStream.fail())
            {
                vector<char> fileContents;
                loadingStream.seekg(0, ios_base::end);
                streampos fileSize = loadingStream.tellg();
                fileContents.resize(fileSize);

                loadingStream.seekg(0, ios_base::beg);
                loadingStream.read(&fileContents[0], fileSize);
                file.data = fileContents;
                cout << fileSize << " in ";
                file.fileSize = fileSize;
            }
            cout << entry.path() << endl;
            
            string fileNameString = entry.path().filename().string();
            vector<char> fileName(fileNameString.begin(), fileNameString.end());
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
    if (namesBlockPadding == 0)
    {
        sizeOfNamesBlock = currentNameOffset;
    }
    else
    {
        sizeOfNamesBlock = currentNameOffset + namesBlockPadding;
    }

    cout << namesBlockPadding;

    // Write the header

    ofstream fout (fileout, ios::out | ios::binary);

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

    system("pause");
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
