
void* changeDirectory(void* args, int argc){
	if (argc<2){
		chdir(userInfo->pw_dir);
	}
	else{
		const char ** path = (const char**)args;
		if (chdir(path[1])<0){
			printf("%s\n", strerror(errno));
		}
	}
}

void* hello (void* args, int argc){
	printf("Hello, world!\n");
}

void* bashHelp (void* args, int argc){
	printf( "Mard bash, version 1.1.0(1)-release (x86_64-pc-linux-gnu)\nThese shell commands are defined internally.  Type `help' to see this list.\nType `help name' to find out more about the function `name'.\nUse `info bash' to find out more about the shell in general.\nUse `man -k' or `info' to find out more about commands not in this list.\n");
	for (int i = 0; builtin_function_lookup_table[ i ].fn; i++){
	    printf("%s\n", builtin_function_lookup_table[ i ].key);
	}
}

void* exitBash (void* args, int argc){
	printf("Bye!\n");
	exit(1);
}

void* printWorkingDirectory (void* args, int argc){
	size_t currentpathsize = 50;
	char* CURRENT_PATH = (char *)malloc(pathsize * sizeof(char));
	getcwd(CURRENT_PATH, currentpathsize);
	printf("%s\n", CURRENT_PATH);
	free (CURRENT_PATH);
}

void* clearScreen (void* args, int argc){
	printf("\e[1;1H\e[2J");
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
        printf("%s\n", dent->d_name);

        if (S_ISDIR(st.st_mode)) dir_count++;
    }
    closedir(srcdir);
    return (void*)dir_count;
}

void* runGame (void* args, int argc){
	if (argc<2){
		printf("Use play [name] to play any of the following games!\n");
		std::vector<char*> arguments;
		arguments.push_back("ls");
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

			for(int i=0; i<defpath.size(); i++)
				Path[i]=defpath[i];
			Path[defpath.size()]='\0';

			char* argumentsToChangePath [] = {"cd", Path};
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
			exit(1);
		}
		//wait for game to end
		else{
			waitpid(childPid,NULL,0);
		}

	}
}

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


