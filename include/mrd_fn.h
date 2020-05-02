
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
void* memeGenerator (void* args, int argc){

	srand(time(NULL));

    const char** argv = (const char**) args;

    std::vector<std::string>Memes;

    std::string path = "./usr/games/meme-generator/memes";

	int dir_count = 0;
    struct dirent* dent;
    DIR* srcdir = opendir(path.c_str());

    if (srcdir == NULL)
    {
        perror("opendir");
        return (void*)-1;
    }

    while((dent = readdir(srcdir)) != NULL)
    {
        struct stat st;

        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            continue;

        if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
        {
            perror(dent->d_name);
            continue;
        }

        Memes.push_back(dent->d_name);

        if (S_ISDIR(st.st_mode)) dir_count++;
    }
    closedir(srcdir);
//    return (void*)dir_count;


    FILE * fp;
     char * line = NULL;
     size_t len = 0;
     ssize_t read;


    fp = fopen("./usr/games/meme-generator/captions", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    std::vector<std::string> content;
    content.reserve(100);
    while ((read = getdelim(&line, &len, '%', fp)) != -1) {
//        printf("Retrieved line of length %zu:\n", read);
        line[read-1]='\0';
//        line[read]='\0';

//        printf("%s", line);
        content.push_back(line);
    }
//    printf ("Length of vector is %d", content.size());
//    for (int i=0; i<content.size(); i++){
//        printf ("%s\n", content[i].c_str());
//    }

    printf("%s\n", content[rand()%content.size()].c_str());


    fclose(fp);
    if (line)
        free(line);







	char * buffer = 0;
	long length;

	std::string memePath = "./usr/games/meme-generator/memes/" + Memes[rand()%Memes.size()];

	FILE * f = fopen (memePath.c_str(), "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = (char*) malloc (length);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	printf(R"EOF(
%s
)EOF", buffer);

}
//void* systemCall (void* args, int argc);

struct {
  void* (*fn)(void*, int);
  const char* key;
} builtin_function_lookup_table[] =
  {
  { &changeDirectory,   "cd"},
  { &hello, "hello" },
//  { &systemCall, "system"},		/*ditched system as a built-in command in favor of a separate exec'd program */
  { &bashHelp, "help"},
  { &printWorkingDirectory, "pwd"},
  { &listDirectory, "ls"},
  { &runGame, "play"},
  { &clearScreen, "clear"},
  { &exitBash, "exit"},
  { &memeGenerator, "meme-generator"},
  { NULL,    NULL     }
  };



bool lookup_and_call( char** arguments, int argument_count ){
//	printf("in lookup, arguments[0] is '%s'\n", arguments[0]);
//	printf("in lookup, argument_count is %d\n", argument_count);


	for (int i = 0; builtin_function_lookup_table[ i ].fn; i++)
    if (strcmp(builtin_function_lookup_table[ i ].key, arguments[0]) == 0){
      (*(builtin_function_lookup_table[ i ].fn))(arguments, argument_count-1); //-1 because we are excluding the NULL at the end
      return true;
     }
//	printf("exit lookup\n");
  return false;
}

#include "./mrd_fn.cpp"

#endif /* MRD_FN_H_ */
