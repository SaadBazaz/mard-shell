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
		char** argv = (char**)args;
		int childPid = 0;
		if ((childPid = fork()) == 0){
			char* manToOpen= argv[1];
			std::string defpath = getenv("SHELL");
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
	else{
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

    while((dent = readdir(srcdir)) != NULL){
        struct stat st;

        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            continue;

        if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0){
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
		std::string path = getenv("SHELL");
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
			std::string defpath = getenv("SHELL");
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

void* setEnvironment (void* args, int argc){
	char** argv = (char**) args;
	if (argc>2){
		bool OverWrite = false;
		if(getenv(argv[1])) { //the Env Variable exists, so prompt overwrite
			printf("The environment variable \"%s\" already exists.\n",argv[1]);
			printf("Do you wish to overwrite its value? [Y/N] ");
			char var;
			var = getchar();
			if (var == 'Y' or var == 'y'){
				OverWrite = true;
			}
			else{
				printf("The value was not set.\n");
				return (void*)NULL;
			}
		}
		printf("setenv called on %s to %s\n",argv[1], argv[2]);
	    setenv(argv[1], argv[2], OverWrite);
	}
	return (void*)getenv(argv[1]);
}

void* unsetEnvironment (void* args, int argc){
	if (argc>1){
		char** argv = (char**) args;
		if(getenv(argv[1])) { //the Env Variable exists, so unset it directly
			printf("unsetenv called on %s\n",argv[1]);
		    unsetenv(argv[1]);
			return (void*)true;
		}
		else{
			printf("Cannot unset as variable \"%s\" does not exist.\n",argv[1]);
		}
	}
}

void* getEnvironment (void* args, int argc){
	if (argc==0){
		getAllEnvironment(NULL, 0);
		return (void*)true;
	}
	char** argv = (char**) args;
	auto value = getenv(argv[1]);
	if (value){
		printf("%s=%s\n",argv[1],value);
		return (void*)value;
	}
	else{
		printf("Cannot get as variable \"%s\" does not exist.\n",argv[1]);
		return (void*)NULL;
	}

}

void* getAllEnvironment (void* args, int argc){
	for (int i=0; environ[i]!=NULL; i++){
		printf("%d. %s\n", i+1, environ[i]);
	}

}
