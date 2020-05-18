#ifndef MRD_FN_H_
#define MRD_FN_H_

#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <signal.h>	//overriding interrupt signal

#include <sys/types.h>

#include <dirent.h> //for ls implementation

#include <algorithm> //for sort in ls

#include <errno.h>


void* changeDirectory		(void*, int);
void* hello 			(void*, int);
void* bashHelp 			(void*, int);
void* exitBash 			(void*, int);
void* printWorkingDirectory 	(void*, int);
void* clearScreen 		(void*, int);
void* listDirectory 		(void*, int);
void* runGame 			(void*, int);
void* setEnvironment 		(void*, int);
void* unsetEnvironment 		(void*, int);
void* getEnvironment 		(void*, int);
void* getAllEnvironment 	(void*, int);

struct {
  void* (*fn)(void*, int);
  const char* key;
} builtin_function_lookup_table[] =
  {
	  { &changeDirectory,   		"cd"		},
	  { &clearScreen, 			"clear"		},
	  { &getAllEnvironment, 		"environ"	},
	  { &exitBash, 				"exit"		},
	  { &getEnvironment, 			"getenv"	},
	  { &hello, 				"hello"		},
	  { &bashHelp, 				"help"		},
	  { &listDirectory, 			"ls"		},
	  { &runGame, 				"play"		},
	  { &printWorkingDirectory, 		"pwd"		},
	  { &setEnvironment, 			"setenv"	},
	  { &unsetEnvironment, 			"unsetenv"	},
	  { NULL,    				NULL		}
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
