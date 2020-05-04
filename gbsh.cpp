#include <sys/utsname.h> //getting system info
#include <signal.h>	//overriding interrupt signal

#include <pwd.h> //getting user info

#include "./include/color_modder.h" //to alter bash output colors

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

/* declaring global Env variables for this terminal */
size_t defaultpathsize = 50;
char* DEFAULT_PATH  = (char *)malloc(defaultpathsize * sizeof(char));					//default path for terminal commands (Virtual System)
size_t pathsize = 100;
char* CURRENT_PATH_RELATIVE = (char *)malloc(pathsize * sizeof(char));

size_t hostnamesize = 50;
char* HOST_NAME = (char *)malloc(hostnamesize * sizeof(char));

struct utsname systemInfo;
struct passwd * userInfo;
/* end declaration */


#include "./include/mrd_fn.h"




/* ================ Assistive Terminal Functions START ================ */
void setCWD(){
	size_t currentpathsize = 50;
	char* CURRENT_PATH = (char *)malloc(pathsize * sizeof(char));
	getcwd(CURRENT_PATH, currentpathsize);
	int i;
	for (i=0; CURRENT_PATH[i]!='\0'; i++){
		CURRENT_PATH_RELATIVE[i] = CURRENT_PATH[i];
	}
	free (CURRENT_PATH);
	pathsize = i;
	CURRENT_PATH_RELATIVE[pathsize] = '\0';
	int j;
	for (j=0;;j++){
		if (userInfo->pw_dir[j] == '\0'){
			CURRENT_PATH_RELATIVE[j-1] = '~';
//			printf("Pathsize was %d\n",pathsize);
//			pathsize = pathsize - (j-1);
//			printf("Pathsize is now %d\n",pathsize);
//			if (pathsize>0){
//				for (int k=0; k<pathsize; k++){
//					CURRENT_PATH_RELATIVE[k] = CURRENT_PATH_RELATIVE[k+j-1];
//				}
//				CURRENT_PATH_RELATIVE[pathsize] = '\0';
//			}
			CURRENT_PATH_RELATIVE = CURRENT_PATH_RELATIVE + j - 1;
			break;
		}
		else if (CURRENT_PATH_RELATIVE[j] != userInfo->pw_dir[j])
			break;
	}

}

void inputLoop(){
	char * input;
	size_t inputsize = 100;
	bool isDaemon = false;
    input = (char *)malloc(inputsize * sizeof(char));
	do {
	    Color::Modifier red(Color::FG_RED);
	    Color::Modifier def(Color::FG_DEFAULT);
	    Color::Modifier green(Color::FG_GREEN);
	    setCWD();
	    printf("%s%s@%s%s:%s%s%s > ", red.getColor().c_str(), userInfo->pw_name, HOST_NAME, def.getColor().c_str(), green.getColor().c_str(), CURRENT_PATH_RELATIVE, def.getColor().c_str());

	    auto characters = getline(&input, &inputsize, stdin);
	    input[characters-1] = '\0';


		std::vector <char *> arguments;
		arguments.push_back( strtok (input," ") );
		while (arguments[arguments.size()-1] != NULL) {
			arguments.push_back(strtok (NULL, " "));
		}
		int childPid = 0;

		if (arguments[0] == NULL)
			continue;

	    // checks if Daemon process or not (ampersand '&' in the end)
	    for (int i = strlen(arguments[arguments.size()-2]) - 1; i>=0; i--){
			//std::cout<<"Checking daemon\n";
			//std::cout<<"i is "<<i<<" \n";
			//std::cout<<"arguments size is "<<arguments.size()<<std::endl;
			//std::cout<<arguments[arguments.size()-2]<<std::endl;
	    	if (arguments[arguments.size()-2][i] != ' ' && arguments[arguments.size()-2][i] != '\0'){
	    		if (arguments[arguments.size()-2][i] == '&'){
	    			std::cout<<"Is daemon\n";
	    			isDaemon = true;
	    			arguments[arguments.size()-2][i] = '\0';
	    			characters--;
	    		}
	    		break;
	    	}
	    	else{
	    		arguments[arguments.size()-2][i] = '\0';
	    	}
	    }


		//check if a builtin bash command
		if (lookup_and_call(&arguments[0], arguments.size())){
//			printf("Called a built-in function\n");
		}


		else if ((childPid = fork()) == 0){
			//is a Virtual System command
			if (strchr(arguments[0],'/') == nullptr){
				std::string defpath = DEFAULT_PATH;
				defpath += "/bin/";
				std::string path = defpath + arguments[0];
				char ** argv  = &arguments[0];
				execv(path.c_str(), argv);
				printf("Command '%s' not found\n",arguments[0]);
			}
			//is a file or directory
			else{
				std::string path = arguments[0];
				char ** argv  = &arguments[0];
				execv(path.c_str(), argv);
				printf("mard: %s: No such file or directory\n", arguments[0]);
			}
			//everything failed
			exit(1);

		}
		else{
			if (!isDaemon)
				waitpid(childPid,NULL,0);
		}
	} while(1);
}

void initializeTerminal(){
    signal(SIGINT, sigintHandler);
	clearScreen(NULL, 0);
	getcwd(DEFAULT_PATH, defaultpathsize);


	for (int i=0;; i++){
		if (DEFAULT_PATH[i]=='\0'){
				defaultpathsize=i;
				break;
		};
	}

//	std::string appendThis = "/bin/";
//
//	for (int i=defaultpathsize,j=0; appendThis[j]!='\0'; i++, j++){
//		DEFAULT_PATH[i]=appendThis[j];
//	}

	gethostname(HOST_NAME, hostnamesize);
	uname(&systemInfo);
	userInfo = getpwuid(getuid());

	std::string userName = userInfo->pw_gecos;
	for (int i=0, foundComma = false; i<userName.size(); i++){
		if (userName[i] == ','){
			foundComma = true;
		}
		if (foundComma){
			userName.pop_back();
		}
	}
	userName.pop_back();

	printf("Welcome back to your %s (%s) system, %s.\n\n", systemInfo.sysname, systemInfo.machine, userName.c_str());

	printf("mard is a chad 3rd-party bash. It comes with command history, a virtual machine, and even built-in games.\n");
	printf("Use 'system' before a Linux/UNIX system call to use the machine's default system calls.\n\n");

	printf("The default path is: %s\n\n", DEFAULT_PATH);

}
/* ================ Assistive Terminal Functions END ================ */




/* driver code */
int main(int argc, char **argv) {
	initializeTerminal();
	inputLoop();
	return 0;
}
