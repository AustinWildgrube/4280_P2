I am using option 3 with the driver and FSA table. The driver is in the testScanner.cpp file starting on line 16.
The FSA table is in the scanner.cpp file and starts on line 38. I am also counting lines.

Title: P2: The Parser
Author: Austin Wildgrube - akww6@umsystems.edu
Class: CMPSCI4280 - 001
Date Finished: Sat, April 4th 2021
Name of File: main.cpp scanner.cpp parser.cpp
Files Created by Program: frontEnd
External Files: N/A
External Sources: N/A

This program is the parser for our compiler. It accepts characters and checks to see if they are tokens of our
language and then checks to see if they fit our languages grammar.

To make the project just simply type "make" in the directory.
To clean this project just simply type "make clean" in the directory.

Usage: frontEnd
Usage: frontEnd [fileName].fs
Usage: frontEnd < fileName.fs

For example: ./frontEnd [fileName].fs

This program ONLY accepts .fs files.

An example file is included and can be ran using the command:
./frontEnd test_data.fs

My Github repo for this is private, and if you would like access I am more than happy to open it or add you as a member.
