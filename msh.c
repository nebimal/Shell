/**********************************************************************************************
* Author: Nebi Malik
* Date: September 24, 2024
* File: msh.c
* Description: This file implements a basic shell that executes commands from Bash.
* Copyright (C) 2024 Nebi Malik. All rights reserved.
**********************************************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>

#define WHITESPACE " \t\n"      

#define MAX_COMMAND_SIZE 255    

#define MAX_NUM_ARGUMENTS 32     

void exec1(char **token);
void exec2(char **token);

int main(int argc, char *argv[])
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  if((argv[1] != NULL) && (argv[2] == NULL))
  {
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    if(fp == NULL)
    {
      char error_message[30] = "An error has occurred\n";
      write(STDERR_FILENO, error_message, strlen(error_message)); 
      exit(1);
    }
    while(fgets(command_string, MAX_COMMAND_SIZE, fp)) // Gets input from file.
    {
      char *token[MAX_NUM_ARGUMENTS];

      int token_count = 0;                                 
                                                           
      char *argument_pointer;                                         
                                                           
      char *working_string  = strdup( command_string );                
    
      while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
      {
        token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
          token[token_count] = NULL;
        }
          token_count++;
      }
      int j = 0;   // These forloops and if statements moves the command-
      for(int i = 0; i < token_count; i++)  // all the way to the first-
      {         // index if the user did lets say a space and the command.
        if(token[i] != NULL) 
        {
          j = i;
          break;
        }
      }
      if(j > 0) //This if statement checks to see if there wear any leading null characters.
      {
        for(int i = 0; i < token_count; i++)
        {
          token[i] = token[i+j];
          token[i+j] = NULL;
        }
      }

      exec1(token);
    }
  }
  else if((argv[1] != NULL) && (argv[2] != NULL))
  {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
    exit(1);
  }


  while(( 1 ) && argv[1] == NULL)
  {
    printf ("msh> ");

    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    char *argument_pointer;                                         
                                                           
    char *working_string  = strdup( command_string );                
    
    char *head_ptr = working_string;
    
    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    int j = 0;
      for(int i = 0; i < token_count; i++)
      {
        if(token[i] != NULL)
        {
          j = i;
          break;
        }
      }
      if(j > 0)
      {
        for(int i = 0; i < token_count; i++)
        {
          token[i] = token[i+j];
          token[i+j] = NULL;
        }
      }

      exec1(token);

    free( head_ptr );

  }
  return 0;

}

void exec2(char **token)
{  
  int index = 0;
  int i = 0;
  while(token[i] != NULL)
  {
    if(strcmp(token[i],">") == 0)
    {
      index = i;
      break;
    }

    i++;
  }
  
  pid_t pid_1 = fork(); // creates a new process because calling execvp-
                        // ignores all the code below the execvp,

  if(pid_1 == 0)        // making a child process makes it so it only-
  {                     // ignores the rest of the code in the child process.
    if(index != 0) //redirecting since > was found
    {
      
      int k = index + 1;
      while(token[k] != NULL)
      {
        k++;
      }
      if(k != index + 2)
      {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(0);
      }


      int fd = open( token[index + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
      if( fd < 0 ) // Above creates the file and redirects the program there.
        {
          char error_message[30] = "An error has occurred\n";
          write(STDERR_FILENO, error_message, strlen(error_message)); 
          exit( 0 );                    
        }
      dup2( fd, 1 );
      token[index] = NULL; // This makes the > character null.
      execvp(token[0], &token[0]); // This is what actually allows you to write in file.
      close( fd );

      exit(0);
    }

    int ret = execvp(token[0], &token[0]); // if this suceeds, it calls the function and-
    if(ret == -1)                          // ignores the rest of the code, if it fails/the-
    {                                      // command isnt valid, it returns -1.
      char error_message[30] = "An error has occurred\n"; // if there are 0 or more than-
                                                          // 1 arguments after cd, we write this.
      write(STDERR_FILENO, error_message, strlen(error_message)); 
    }
    exit(1); //terminates the child process if the execvp function fails.
  }
  else if(pid_1 > 0)
  {
    wait(NULL); // parent process waits until child finishes, after child is-
                // terminated, it recalls the while loop in the main function.
  }
  else
  {
    char error_message[30] = "An error has occurred\n"; 
    write(STDERR_FILENO, error_message, strlen(error_message));  // if it is neither a child nor a-
    exit(1);                          // parent process, ITS AN ERROR THAT MUST BE TERMINATED!
  }
}

void exec1(char **token)
{
  if(token[0] != NULL)  
  {
    if(((strcmp(token[0],"quit") == 0) || (strcmp(token[0],"exit") == 0))) 
    {      //comparing to see if user typed "quit" or "exit", then exits program if user input that.
      if(token[1] == NULL)
      {
        exit(0);
      }
      else
      {
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message)); 
      }
    }
    else if(strcmp(token[0],"cd") == 0) 
    {          // why are we doing this? to see if the user has type in "cd" as the parameter.
      if(token[1] != NULL && token[2] == NULL) // we check to see if there are no arguments-
      {      // after cd, and we also check to see if-
        int ret = chdir(token[1]);   // there are 2 or more arguments after cd, we just want 1.
        if(ret != 0)
        {
          char error_message[30] = "An error has occurred\n"; 
          write(STDERR_FILENO, error_message, strlen(error_message)); 
        }
      }
      else
      {
        char error_message[30] = "An error has occurred\n"; //if there is 0 or more than 1 argument-
        write(STDERR_FILENO, error_message, strlen(error_message)); // after cd, we write this.
      }
    }
    else
    {
      exec2(token);
    }
  }
}
