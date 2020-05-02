#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int main (int argc, char** argv){

	srand(time(NULL));

    std::vector<std::string>Memes;

    std::string path = "./memes";

	int dir_count = 0;
    struct dirent* dent;
    DIR* srcdir = opendir(path.c_str());

    if (srcdir == NULL)
    {
        perror("opendir");
        return -1;
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

    FILE * fp;
     char * line = NULL;
     size_t len = 0;
     ssize_t read;


    fp = fopen("./captions", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    std::vector<std::string> content;
    content.reserve(100);
    while ((read = getdelim(&line, &len, '%', fp)) != -1) {
        line[read-1]='\0';
        content.push_back(line);
    }

    printf("%s\n", content[rand()%content.size()].c_str());


    fclose(fp);
    if (line)
        free(line);




	char * buffer = 0;
	long length;

	std::string memePath = "./memes/" + Memes[rand()%Memes.size()];

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
