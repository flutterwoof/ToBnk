This tool creates a .bnk file, which is used by Rez Infinite to hold .wav and .xwma files, which are the audio samples used by the MIDI instruments (Programs) defined in .mpb files (MIDI Program Banks). With ToBnk, you provide a folder as input and the tool outputs a .bnk file that contains all the files in that folder, excluding subfolders or files with the extension ".bnk".

In Rez Infinite, the .bnk files only ever contain .wav and .xwma files, while all other files for the game are stored in its .dat files. The .bnk container is very generic, however, and you could put any file type into here. There would be no point doing that and I expect that the game would ignore them, but you could do that, and ToBnk will include those files in the end .bnk.

We're gonna post the .bnk extractor script soon but we use QuickBMS for that and we only made this tool because QuickBMS doesn't let you reimport with extra files. There'll be a new repo on here with the BMS scripts for .bnk and .dat at some point.

This tool can be used in three ways:

- `bnk.exe` (asks you for the folder to use. names the output file with the folder name plus ".bnk" and places it in the folder's parent folder)

- `bnk.exe [input folder]` (does not ask for further input. names the output file with the folder name plus ".bnk" and places it in the folder's parent folder)

- `bnk.exe [input folder] [output file]` (does not ask for further input. places the output file whereever the user states. if [output file] is just a file name it will be treated as a relative path and thus placed in the same directory as as bnk.exe, not in the input folder's parent folder)

I believe that building this on macOS and Linux would work fine, but we haven't tried that yet.
