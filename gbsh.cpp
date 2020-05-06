#include <sys/utsname.h> //getting system info
#include <signal.h>	//overriding interrupt signal

#include <list> //for command history

#include <pwd.h> //getting user info

#include "./include/color_modder.h" //to alter bash output colors

const int KEY_UP []= {27, 91, 65};
const int KEY_DOWN []= {27, 91, 66};
const int KEY_LEFT []= {27, 91, 68};
const int KEY_RIGHT []= {27, 91, 67};

void* helloWorld(){
	printf("Hello world\n");
	return (void*)0;
}

//struct {
//	int values [3];
//	int number_of_values;
//    const char* key;
//    void* (*fn)();
//} specialkeys_lookup_table [] =
//{
//		{{27, 91, 65}, 3, "KEY_UP", &helloWorld},
//		{{27, 91, 66}, 3, "KEY_DOWN", &helloWorld}
//};


std::list <std::string> CommandHistory;
std::list<std::string>::iterator cmdhist_it = CommandHistory.begin();


void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

struct environmentVariable{
	void* string_value;
	int integer_value;
	char* key;
};

environmentVariable SHELL;



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



// Include all builtin functions of mard
#include "./include/mrd_fn.h"




/* ================ Assistive Terminal Functions START ================ */
void setCWD(){
	size_t currentpathsize = 100;
	char* CURRENT_PATH = (char *)malloc(currentpathsize * sizeof(char));
	getcwd(CURRENT_PATH, currentpathsize);

	int j;
	for (j=0;j<(signed int)strlen(CURRENT_PATH);j++){
		if (userInfo->pw_dir[j] == '\0'){
			break;
		}
		else if (CURRENT_PATH[j] != userInfo->pw_dir[j])
			break;
	}

//	printf ("j is %d, strlen is %d", j, strlen(userInfo->pw_dir));
	if (j!=(signed int)strlen(userInfo->pw_dir)){
		int i;
		for (i=0; CURRENT_PATH[i]!='\0'; i++){
			CURRENT_PATH_RELATIVE[i] = CURRENT_PATH[i];
		}
		CURRENT_PATH_RELATIVE[i] = '\0';
		if (CURRENT_PATH)
			free (CURRENT_PATH);
	}
	else{
		CURRENT_PATH_RELATIVE[0]='~';
		int i;
		for (i=1; CURRENT_PATH[j]!='\0'; i++, j++){
			CURRENT_PATH_RELATIVE[i] = CURRENT_PATH[j];
		}
		CURRENT_PATH_RELATIVE[i] = '\0';
	}

	pathsize = strlen(CURRENT_PATH_RELATIVE);

	free(CURRENT_PATH);
}

void inputLoop(){
	char * input;
	size_t inputsize = 100;
	bool isDaemon = false;
    input = (char *)malloc(inputsize * sizeof(char));
	do {
	    Color::Modifier red(Color::FG_GREEN);
	    Color::Modifier def(Color::FG_DEFAULT);
	    Color::Modifier green(Color::FG_BLUE);
	    setCWD();
	    printf("%s%s@%s%s:%s%s%s > ", red.getColor().c_str(), userInfo->pw_name, HOST_NAME, def.getColor().c_str(), green.getColor().c_str(), CURRENT_PATH_RELATIVE, def.getColor().c_str());

	    int characters = 0;

	    /* Uncomment this part, and comment the next section, to take normal inputs instead of fancy nb input */
//	    auto characters = getline(&input, &inputsize, stdin);
//	    input[characters-1] = '\0';

	    int c; // used to capture the ASCII value of the input

	    /* use system call to make terminal send all keystrokes directly to stdin */
	    system ("/bin/stty raw");
	    int specialKeyCount = 0;
	    std::string nbInput="";
	    int specialInput [] = {0, 0, 0};

	    int CURSOR_POS = 0;

	    cmdhist_it = CommandHistory.end();

	    while((c=getchar())!= 13) {
	      /* Use the ASCII value of ENTER to break out of the loop, since CTRL-D or CTRL-C won't work in raw */
//	    	std::cout<<"{int is "<<c<<"}";
	    	if (c == 127){
	    		if (!nbInput.empty()){
		    		printf("\b\b\b   \b\b\b");
	    			nbInput.pop_back();
	    			characters--;
	    			CURSOR_POS--;
	    		}
	    		else {
		    		printf("\b\b  \b\b");
	    		}
	    	}
	    	else if (KEY_UP[specialKeyCount] ==  c){
//				std::cout << std::endl << "special input" << std::endl;//key up
				specialInput[specialKeyCount] = c;
				specialKeyCount++;
				if (specialKeyCount == 3){
					bool isSame=true;
					for (int i=0; i<3; i++){
						if (specialInput[i] != KEY_UP[i]){
							isSame = false;
							break;
						}
					}
					if (isSame){
						if (cmdhist_it!=CommandHistory.begin())
							cmdhist_it--;

						if (!CommandHistory.empty()){
							// remove the traces of previous input
							std::string backTrack = "";
							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += '\b';

							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += ' ';

							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += '\b';

							// reassign input
							nbInput = (*cmdhist_it);

							printf("\b\b\b\b    \b\b\b\b%s%s", backTrack.c_str(), nbInput.c_str());
	//						printf("[nbInput is %s]", nbInput.c_str());
						}
						else{
							printf("\b\b\b\b    \b\b\b\b");
						}
//						printf("\b\b\b\b[User pressed UP!]");
						specialKeyCount = 0;
					}
				}
			}
			else if (KEY_DOWN[specialKeyCount] ==  c){
//				std::cout << std::endl << "special input" << std::endl;//key up
				specialInput[specialKeyCount] = c;
				specialKeyCount++;
				if (specialKeyCount == 3){
					bool isSame=true;
					for (int i=0; i<3; i++){
						if (specialInput[i] != KEY_DOWN[i]){
							isSame = false;
							break;
						}
					}
					if (isSame){
						if (cmdhist_it!=CommandHistory.end())
							cmdhist_it++;

						if (!CommandHistory.empty()){
							// remove the traces of previous input
							std::string backTrack = "";
							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += '\b';

							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += ' ';

							for (int i=0; i<(signed int)nbInput.size(); i++)
								backTrack += '\b';

							// reassign input
							nbInput = (*cmdhist_it);

							printf("\b\b\b\b    \b\b\b\b%s%s", backTrack.c_str(), nbInput.c_str());
	//						printf("[nbInput is %s]", nbInput.c_str());
						}
						else{
							printf("\b\b\b\b    \b\b\b\b");
						}
//						printf("\b\b\b\b[User pressed DOWN!]");
						specialKeyCount = 0;					}
				}
			}
			else if (KEY_LEFT[specialKeyCount] ==  c){
//				std::cout << std::endl << "special input" << std::endl;//key up
				specialInput[specialKeyCount] = c;
				specialKeyCount++;
				if (specialKeyCount == 3){
					bool isSame=true;
					for (int i=0; i<3; i++){
						if (specialInput[i] != KEY_LEFT[i]){
							isSame = false;
							break;
						}
					}
					if (isSame){
//						printf("\b\b\b\b[User pressed LEFT!]");
						if (CURSOR_POS>0){
							CURSOR_POS--;
							std::string backTracks = "";
							int num = characters - CURSOR_POS;

							for (int i=0; i<num; i++)
								backTracks += '\b';
							printf("\b\b\b\b    \b\b\b\b\b%s%s", &nbInput[CURSOR_POS], backTracks.c_str());
						}
						else{
							printf("\b\b\b\b    \b\b\b\b");
						}
						specialKeyCount = 0;
//						std::cout<<num;
					}
				}
			}
			else{
				specialKeyCount = 0;
				if (CURSOR_POS<characters)
					nbInput[CURSOR_POS] = (char)c;
				else
					nbInput += (char)c;
				characters++;
				CURSOR_POS++;
			}


//	    	std::cout<<"[Cursor position is "<<CURSOR_POS<<"]";


//	    	moveCursor();


	    }
	    /* use system call to set terminal behaviour to more normal behaviour */
	    system ("/bin/stty cooked");

	    /* remove Enter ASCII ("^M") */
	    printf("\b\b  \n");

	    /* copy the nbInput string to the official input char* */
	    strcpy( input, nbInput.c_str());

	    /* add command to Command History */
	    if (nbInput != ""){
	    	if (nbInput != CommandHistory.back()) //avoid repeating commands
	    		CommandHistory.push_back(nbInput);
	    }

	    /* tokenize the input and place it into a vector of char* */
		std::vector <char *> arguments;
		arguments.push_back( strtok (input," ") );
		while (arguments[arguments.size()-1] != NULL) {
			arguments.push_back(strtok (NULL, " "));
		}
		int childPid = 0;

		if (arguments[0] == NULL)
			continue;

	    // checks if Daemon process or not (ampersand '&' in the end)
	    for (int i = (signed int)strlen(arguments[arguments.size()-2]) - 1; i>=0; i--){
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
	for (int i=0, foundComma = false; i<(signed int)userName.size(); i++){
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






// Garbage code
//	    int c = 0;
//	    while(1)
//	    {
//	        c = 0;

//	        char buff[2];
//
//	        int ret = read(0, buff, 2);
//	        if (ret == -1) continue;
//	        buff[ret] = '\0';
//
//	        printf("input was %c", buff);
//
//	        switch(c) {
//	        case KEY_UP:
//	            std::cout << std::endl << "Up" << std::endl;//key up
//	            break;
//	        case KEY_DOWN:
//	        	std::cout << std::endl << "Down" << std::endl;   // key down
//	            break;
//	        case KEY_LEFT:
//	        	std::cout << std::endl << "Left" << std::endl;  // key left
//	            break;
//	        case KEY_RIGHT:
//	        	std::cout << std::endl << "Right" << std::endl;  // key right
//	            break;
//	        default:
//	        	std::cout << std::endl << "null" << std::endl;  // not arrow
//	            break;
//	        }
//
//	    }




