
void* changeDirectory(void* args, int argc){
	if (argc<2){
		chdir(userInfo->pw_dir);
	}
	else{
		const char ** path = (const char**)args;
		if (chdir(path[1])<0){
			printf("%s\n", strerror(errno));
			return (void*)-1;
		}
	}
	return (void*)0;
}

void* hello (void* args, int argc){
	printf("Hello, world!\n");
	return (void*)0;
}

void* bashHelp (void* args, int argc){
	if (argc<2){
		printf( "Mard bash, version 1.1.0(1)-release (x86_64-pc-linux-gnu)\nThese shell commands are defined internally.  Type `help' to see this list.\nType `help name' to find out more about the function `name'.\n");
		for (int i = 0; builtin_function_lookup_table[ i ].fn; i++){
		    printf("%s\n", builtin_function_lookup_table[ i ].key);
		}
	}
	else if (argc == 2) {
		printf("help: ");
//		char ** argv = (char**) args;
//		for (int i = 0; builtin_function_lookup_table[ i ].fn; i++){
//		    if (strcmp(builtin_function_lookup_table[i].key, argv[1])==0){
////				printf(argv[1]);
//		    	printf(builtin_function_lookup_table[i].man);
//		    	return NULL;
//		    }
//		}
//		printf("No such function found\n");


		char** argv = (char**)args;
		int childPid = 0;
		if ((childPid = fork()) == 0){
			char* manToOpen= argv[1];
			std::string defpath = DEFAULT_PATH;
			defpath += "/usr/share/man/";

			char* Path = (char *)malloc(defpath.size() * sizeof(char) + 1);

			for(int i=0; i<(signed int)defpath.size(); i++)
				Path[i]=defpath[i];
			Path[defpath.size()]='\0';

			const char* argumentsToChangePath [] = {"cd", Path};
			changeDirectory((void*)argumentsToChangePath, 2);

			char* const argToSend [] = {(char*)"nroff", (char*)"-man", manToOpen, NULL};
			std::string path = "/bin/nroff";
			execv(path.c_str(), argToSend);


			path = "/usr/bin/nroff";
			execv(path.c_str(), argToSend);

			path = "/usr/local/bin/nroff";
			execv(path.c_str(), argToSend);


			printf("Function '%s' not found\n",manToOpen);
			exit(-1);
		}
		//wait for game to end
		else{
			waitpid(childPid,NULL,0);
		}

	}
	return (void*)0;
}

void* exitBash (void* args, int argc){
	printf("Bye!\n");
	exit(1);
}

void* printWorkingDirectory (void* args, int argc){
	size_t currentpathsize = 50;
	char* CURRENT_PATH = (char *)malloc(pathsize * sizeof(char));
	if (getcwd(CURRENT_PATH, currentpathsize)<0){
		return (void*)-1;
	}
	printf("%s\n", CURRENT_PATH);
	free (CURRENT_PATH);
	return (void*)0;
}

void* clearScreen (void* args, int argc){
	printf("\e[1;1H\e[2J");
	return (void*)0;
}

void* listDirectory (void* args, int argc){
    const char** argv = (const char**) args;
    std::string path;
    if (argc<2){
    	path = "./";
    }
	else {
		path = argv[1];
	}
	int dir_count = 0;
	int file_count = 0;
    struct dirent* dent;
    std::vector<std::string>dirsOrFiles;
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

        dirsOrFiles.push_back(dent->d_name);

        if (S_ISDIR(st.st_mode)) dir_count++;
        else file_count++;
    }
    sort(dirsOrFiles.begin(), dirsOrFiles.end());

    for (int i=0; i<(signed int)dirsOrFiles.size(); i++)
        printf("%s\n", dirsOrFiles[i].c_str());

    printf("--- Total %d directories, %d files found\n", dir_count, file_count);
//    if (dir_count>0 and file_count>0)
	closedir(srcdir);
    return (void*)true;
}

void* runGame (void* args, int argc){
	if (argc<2){
		printf("Use play [name] to play any of the following games!\n");
		std::vector<char*> arguments;
		arguments.push_back((char*)"ls");
		std::string path = DEFAULT_PATH;
		path += "/usr/games";
		arguments.push_back(&path[0]);
//		std::cout<<"Path is "<<path<<std::endl;
		listDirectory(&arguments[0], arguments.size());
	}
	else {
//		printf("Playing game...\n");
		char** argv = (char**)args;
		int childPid = 0;
		if ((childPid = fork()) == 0){
			const char* gameToPlay= argv[1];
			std::string defpath = DEFAULT_PATH;
			defpath += "/usr/games/";
			defpath += gameToPlay;

			char* Path = (char *)malloc(defpath.size() * sizeof(char) + 1);

			for(int i=0; i<(signed int)defpath.size(); i++)
				Path[i]=defpath[i];
			Path[defpath.size()]='\0';

			const char* argumentsToChangePath [] = {"cd", Path};
			changeDirectory((void*)argumentsToChangePath, 2);

			defpath += "/";
			std::string path = defpath + gameToPlay;

			std::vector<char*>argsVector;
			for (int i=1; i<argc; i++){
				argsVector.push_back(argv[i]);
			}
			argsVector.push_back(NULL);

			char ** argToSend = &argsVector[0];
//			std::cout<<"path is "<<path<<std::endl;
//			std::cout<<"argToSend is "<<argToSend[0]<<std::endl;
			execv(path.c_str(), argToSend);
			printf("Game '%s' not found\n",argToSend[0]);
			exit(-1);
		}
		//wait for game to end
		else{
			waitpid(childPid,NULL,0);
		}

	}
	return (void*)0;
}





//================== DEPRECATED BUILT-IN FUNCTIONS ==================

//void* memeGenerator (void* args, int argc){
//
//	srand(time(NULL));
//
//    const char** argv = (const char**) args;
//
//    std::vector<std::string>Memes;
//
//    std::string path = "./usr/games/meme-generator/memes";
//
//	int dir_count = 0;
//    struct dirent* dent;
//    DIR* srcdir = opendir(path.c_str());
//
//    if (srcdir == NULL)
//    {
//        perror("opendir");
//        return (void*)-1;
//    }
//
//    while((dent = readdir(srcdir)) != NULL)
//    {
//        struct stat st;
//
//        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
//            continue;
//
//        if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
//        {
//            perror(dent->d_name);
//            continue;
//        }
//
//        Memes.push_back(dent->d_name);
//
//        if (S_ISDIR(st.st_mode)) dir_count++;
//    }
//    closedir(srcdir);
////    return (void*)dir_count;
//
//
//    FILE * fp;
//     char * line = NULL;
//     size_t len = 0;
//     ssize_t read;
//
//
//    fp = fopen("./usr/games/meme-generator/captions", "r");
//    if (fp == NULL)
//        exit(EXIT_FAILURE);
//
//    std::vector<std::string> content;
//    content.reserve(100);
//    while ((read = getdelim(&line, &len, '%', fp)) != -1) {
////        printf("Retrieved line of length %zu:\n", read);
//        line[read-1]='\0';
////        line[read]='\0';
//
////        printf("%s", line);
//        content.push_back(line);
//    }
////    printf ("Length of vector is %d", content.size());
////    for (int i=0; i<content.size(); i++){
////        printf ("%s\n", content[i].c_str());
////    }
//
//    printf("%s\n", content[rand()%content.size()].c_str());
//
//
//    fclose(fp);
//    if (line)
//        free(line);
//
//
//
//
//
//
//
//	char * buffer = 0;
//	long length;
//
//	std::string memePath = "./usr/games/meme-generator/memes/" + Memes[rand()%Memes.size()];
//
//	FILE * f = fopen (memePath.c_str(), "rb");
//
//	if (f)
//	{
//	  fseek (f, 0, SEEK_END);
//	  length = ftell (f);
//	  fseek (f, 0, SEEK_SET);
//	  buffer = (char*) malloc (length);
//	  if (buffer)
//	  {
//	    fread (buffer, 1, length, f);
//	  }
//	  fclose (f);
//	}
//
//	printf(R"EOF(
//%s
//)EOF", buffer);
//
//}

//void* systemCall (void* args, int argc){
//    std::cout<<"System call activated\n";
//    if (argc >= 2){
//        char** newArgs= new char* [argc];
//        char** argv = (char**) args;
//        for (int i=1; i<argc; i++){
//            newArgs[i-1] = argv[i];
//        }
//        newArgs[argc - 1] = NULL;
//        std::string path = "/bin/";
//        std::string command = newArgs[0];
//        path = path + command;
//        int childPid = 0;
////        if ( childPid =)
//        execv(path.c_str(), newArgs);
//        std::cout<<"System call failed\n"<<std::endl;
//    }
//    else
//        std::cout<<"Could not call: Accepts more than one argument\n";
//}


