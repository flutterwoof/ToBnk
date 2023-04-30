This tool creates a .bnk file, which is used by Rez Infinite to hold the audio samples used by the MIDI instruments (Programs) contained in MIDI Program Banks (.mpb files).

We've not posted the .bnk extractor script yet but we use QuickBMS for that - there'll be a new repo on here with the BMS script at some point.

This tool can be used in three ways:

- `bnk.exe` (asks you for the folder to use. names the output file with the folder name plus ".bnk" and places it in the folder's parent folder)

- `bnk.exe [input folder]` (does not ask for further input. names the output file with the folder name plus ".bnk" and places it in the folder's parent folder)

- `bnk.exe [input folder] [output file]` (does not ask for further input. places the output file whereever the user states. if [output file] is just a file name it will be treated as a relative path and thus placed in the same directory as as bnk.exe, not in the input folder's parent folder)
