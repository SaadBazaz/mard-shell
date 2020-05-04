
#ifndef MRD_FN_H_
#define MRD_FN_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>


#include <iostream>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <signal.h>	//overriding interrupt signal

#include <sys/types.h>
#include <pwd.h> //user info

#include <dirent.h> //for ls implementation

#include <errno.h>

void* changeDirectory(void* args, int argc);
void* hello (void* args, int argc);
void* bashHelp (void* args, int argc);
void* exitBash (void* args, int argc);
void* printWorkingDirectory (void* args, int argc);
void* clearScreen (void* args, int argc);
void* listDirectory (void* args, int argc);
void* runGame (void* args, int argc);
//void* memeGenerator (void* args, int argc);
//void* systemCall (void* args, int argc);

struct {
  void* (*fn)(void*, int);
  const char* key;
  const char* man;
} builtin_function_lookup_table[] =
  {
  { &changeDirectory,   "cd", R"EOF(

		This function changes the directory.

	)EOF"},
  { &hello, "hello", R"EOF(

		This function prints a friendly hello.

	)EOF"},
//  { &systemCall, "system"},		/*ditched system as a built-in command in favor of a separate exec'd program */
  { &bashHelp, "help", R"EOF(help [pattern ...]
    Display information about mardaana builtin commands.
    
    Displays brief summaries of builtin commands.  If PATTERN is
    specified, gives detailed help on all commands matching PATTERN,
    otherwise the list of help topics is printed.
    
    Arguments:
      PATTERN	Pattern specifiying a help topic
    
    Exit Status:
    Returns success unless PATTERN is not found or an invalid option is given.
)EOF"},
  { &printWorkingDirectory, "pwd", R"EOF(

		This function prints the current working directory.

	)EOF"},
  { &listDirectory, "ls", R"EOF(

		This function lists the directories and files within the current directory.

	)EOF"},
  { &runGame, "play", R"EOF(

		This function executes a game file.

	)EOF"},
  { &clearScreen, "clear", R"EOF(

		This function clears the screen.

	)EOF"},
  { &exitBash, "exit", R"EOF(

		This function exits mards.

		)EOF"},
//  { &memeGenerator, "meme-generator"},
  { NULL,    NULL     }
  };



bool lookup_and_call( char** arguments, int argument_count ){
	for (int i = 0; builtin_function_lookup_table[ i ].fn; i++)
    if (strcmp(builtin_function_lookup_table[ i ].key, arguments[0]) == 0){
      (*(builtin_function_lookup_table[ i ].fn))(arguments, argument_count-1); //-1 because we are excluding the NULL at the end
      return true;
     }
  return false;
}

#include "./mrd_fn.cpp"

#endif /* MRD_FN_H_ */
