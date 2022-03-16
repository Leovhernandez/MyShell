/*

Name: Leonardo Hernandez
ID: 1001400160

*/


#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11    // Mav shell supports 10 arguments

//remember child process returns value of 0
//parent process returns non zero value

int main()
{
    char* comHist[15]; //array to store commands
    int countCom = 0; //command count limit
    int countHist = 0; //iterator of commands
    pid_t pidArr[15]; //array to store pids
    int pidCount = 0; //number of pids
    int pidLocate = 0; //index for pid

    char * command_string = (char*) malloc(MAX_COMMAND_SIZE);
    //dynamically allocate memory in the heap for command storage array
    for(int i = 0; i < 15; i++)
    {
        comHist[i] = (char*) malloc(MAX_COMMAND_SIZE);
    }

    while(1)
    {
        // Print out the msh prompt
        printf("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while(!fgets(command_string, MAX_COMMAND_SIZE, stdin));

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;                                 
                                                           
        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         
                                                           
        char *working_string  = strdup(command_string);                

        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *head_ptr = working_string;
        comHist[countHist] = strdup(command_string);
        //parse working_string[0] and use atoi()
        //to convert character n that into its int value
        //and copy converted int into working_string[1] to get ![n - 1]
        if(working_string[0] == '!')
        {
            strcpy(working_string, comHist[(atoi(&working_string[1]))- 1]);
        }

        // Tokenize the input strings with whitespace used as the delimiter
        while(((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) && (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
            if(strlen(token[token_count]) == 0)
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        if (pidLocate > 14)
        {
            pidLocate = 0;
        }
        if(pidCount > 14)
        {
            pidCount = 15;
        }

        //stores each new command_string in its respective
        //index of command history array 
        comHist[countHist] = strdup(command_string);
        countHist++;
        countCom++;
        if (countHist > 14)
        {
            countHist = 0;
        }
        if(countCom > 14)
        {
            countCom = 15;
        }


        //Now print the tokenized input as a debug check
        //verifying that user input is not null

        if(token[0] != NULL)
        {
            //will exit or quit
            if((strcmp(token[0], "exit") == 0) || (strcmp(token[0], "quit") == 0))
            {
                exit(0);
            }
            //change directory based on user input in token[1]
            //if none, return to parent directory
            if(strcmp(token[0], "cd") == 0)
            {
                chdir(token[1]);
            }
            //displaying total number of stored pids
            else if((strcmp(token[0], "pidhistory") == 0) || (strcmp(token[0], "showpids") == 0))
            {
                for(int i = 0; i < pidCount; i++)
                {
                    printf("%d: %d\n", i, pidArr[i]);
                }
                
            }
            //display command history using for loop to iterate through saved commands
            //in command history array
            else if(strcmp(token[0], "history") == 0)
            {
                for(int j = 0; j < countCom; j++)
                {
                    printf("%d: %s", j, comHist[j]);
                }
            }
            else
            {
                //creating a new child process that returns a value of 0 and triggers if statement
                pid_t pid = fork();
                pidArr[pidLocate] = pid;
                pidLocate++;
                pidCount++;
                if(pid == 0)
                {  
                    // Notice you can add as many NULLs on the end as you want
                    int ret = execvp(token[0], &token[0]);  
                    if(ret == -1)
                    {
                        //error if invalid input is consumed
                        printf("%s: execl failed: Command not found.\n", token[0]);
                        return 0;                       
                    }
                }
                else 
                {
                    int status;
                    wait(&status);
                }
            }
        }
        //deallocates the memory taken by the head pointer
        free(head_ptr);

    }

    //free() prevents memory leak of previously allocated memory of comHist
    for(int k = 0; k < 15; k++)
    {
        free(comHist[k]);
    }
    return 0;
}

