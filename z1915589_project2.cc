/********************************************************************
CSCI 480 - Assignment 2 - Fall 2023

Programmer: Alexander Bertolasi
Section: 1
TA: Sai Dinesh Reddy Bandi
Date Due: 09/27/2023

Purpose: This program implements a rudimentary shell, allowing users to enter and execute shell commands, as well as handling
output redirection. It aditionally implements the fcfs command to compute the First Come First Serve algorithm on randomly
generated CPU burst values.
*********************************************************************/

#include <cstring>
#include <cstdio>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <algorithm>

using namespace std;

int main(void) {
    char buf[1024];             //Character input buffer
    pid_t pid;                  //Variable to hold the pid of fork processes
    int status;                 //Variable to hold waitpid status
    string token;               //Token string for reading input arguments
    vector<string> args;        //Array to hold input string arguments
    vector<char*> c_args;       //Array to hold input string arguments in C style
    int consoleOut = dup(1);    //Variable to hold value of original file descriptor, used to restore stdout to console

    //Print opening myshell> output
    cout << "myshell>";

    while (fgets(buf, 1024, stdin) != NULL) {
        buf[strlen(buf) - 1] = 0;

        //Turn buf into stringstream for ease of parsing
        istringstream f(buf);

        if ((pid = fork()) < 0) {
            //If fork failed, print error message
            cout << "fork failed";
        } else if (pid == 0) {  //Child
            //If user types q or quit, end child process
            if (string(buf) == "q" || string(buf) == "quit")
                return 0;

            //Write output variables
            bool writeCommand = false;      //Boolean to determine whether write command was requested
            bool fscsCommand = false;       //Boolean to determine whether fscs command was requested
            string outFile;                 //String to hold the name of output file
            int fscsOption = 5;             //Int to hold the optional value of fscs command

            //Read all standard input
            while (f >> token) {
                //If '>' command was entered
                if (token.find(">") != string::npos) {
                    //Set the writeCommand to true
                    writeCommand = true;
                    if(token == ">") {                             //If '>' is surounded by whitespaces
                        //Get name of output file and exit loop
                        f >> outFile;
                        break;
                    }else if(token[token.length() - 1] == '>'){    //If '>' has whitespace after it
                        //Remove '>' from token string
                        token.pop_back();
                        //Push command into args list
                        args.push_back(token);
                        //Get name of output file and exit loop
                        f >> outFile;
                        break;
                    }else if(token[0] == '>') {                    //If '>' has whitespace before it
                        //Remove '>' from token string
                        token.erase(0, 1);
                        //Get name of output file and exit loop
                        outFile = token;
                        break;
                    }else{                                         //If '>' has no space in front or behind it
                        //Find position of '>' character in input string
                        size_t position = token.find('>');
                        //Push command into args list
                        args.push_back(token.substr(0, position));
                        //Get name of output file and exit loop
                        outFile = string(token.substr(position+1, token.length()));
                        break;
                    }
                }
                //Put command into args list
                args.push_back(token);
            }

            //Convert arguments into C style strings, putting them in c_args
            for (const string& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            //Null terminate arguments list
            c_args.push_back(nullptr);

            //If fscs command was entered, set flag to true
            if(args[0] == "fcfs") {
                fscsCommand = true;
                //If an fscs option was given
                if(c_args[1] != nullptr) {
                    //If more than 1 option was given or if a character was in given option, print error message and exit child process
                    if(c_args[2] != nullptr || args[1].find_first_not_of("0123456789") != string::npos) {
                        cout << "couldn't execute: " << buf << endl;
                        return 0;
                    }
                    //Set fcfsOption to given value
                    fscsOption = stoi(args[1]);
                }
            }
            // If write command is entered, open the writeFile and redirect standard output
            if (writeCommand) {
                //Open outFile
                int fd1 = open(outFile.c_str(), O_TRUNC | O_WRONLY | O_CREAT, 0644);
                //If outFile opened unsucessfully
                if(fd1 == -1) {
                    //Print error message and exit child process
                    cout << "Error opening output file: " << outFile << endl;
                    return 0;
                }
                //Redirect standard output to the requested file
                dup2(fd1, STDOUT_FILENO);
                //Close outfile
                close(fd1);
            }

            //If fscs was entered, calculate and print fcfs algorithm
            if(fscsCommand) {
                int W_total = 0;        //Value to hold the total wait time
                int currentWait = 0;    //Value to hold the current wait time for CPU bursts
                int randTime;           //Value to hold the current random number
                srand(10);              //Generate random numbers using seed 10
                cout << "FCFS CPU scheduling simulation with " << fscsOption << " process";
                //Determine whether to print "process." or "processes."
                if(fscsOption > 1) {
                    cout << "es." << endl;
                }else{
                    cout << "." << endl;
                }
                //Generate CPU bursts = to fscsOption
                for(int i = 0; i < fscsOption; ++i) {
                    //Generate random value and print it out
                    randTime = 1+(rand() % 100);
                    cout << "CPU burst " << randTime << " ms" << endl;
                    //Add current wait time to total wait time (This is done first because the first burst does not need to wait)
                    W_total += currentWait;
                    //Add random CPU burst time to current wait time
                    currentWait += randTime;
                }

                //Print the total wait time, calculate and print the average wait time, then exit child process
                cout << "Total waiting time in the ready queue: " << W_total << " ms" << endl;
                cout << "Average waiting time in the ready queue: " << W_total/fscsOption << " ms" << endl;
                return 0;
            }

            //Run execvp to execute input arguments
            execvp(c_args[0], c_args.data());
            //Restore standard output to console
            dup2(consoleOut, 1);
            //If execvp fails, print an error message and exit child process
            cout << "couldn't execute: " << buf << endl;
            return 0;

        } else {    //Parent
            //If user types q or quit, end parent process
            if (string(buf) == "q" || string(buf) == "quit")
                return 0;

            //Handle waitpid error, print error message
            if ((pid = waitpid(pid, &status, 0)) < 0) {
                cout << "waitpid error";
            }

            //Print next myshell> output after the previous command is executed
            cout << "myshell>";
        }
    }
    return 0;
}