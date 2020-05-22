#include <sys/utsname.h> //getting system info
#include <signal.h>	//overriding interrupt signal

#include <list> //for command history

#include <pwd.h> //getting user info

#include <pthread.h> //making background processes

#include "./include/color_modder.h" //to alter bash output colors

#include <stdlib.h> //for atoi function

#define ever (;;)

const int KEY_UP []= {27, 91, 65};
const int KEY_DOWN []= {27, 91, 66};
const int KEY_LEFT []= {27, 91, 68};
const int KEY_RIGHT []= {27, 91, 67};

//void* helloWorld(){
//	printf("Hello world\n");
//	return (void*)0;
//}



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



//bool special_key_lookup_and_call( char** arguments, int argument_count ){
//	for (int i = 0; specialkeys_lookup_table[ i ].fn; i++)
//    if (strcmp(specialkeys_lookup_table[ i ].key, arguments[0]) == 0){
//      (*(specialkeys_lookup_table[ i ].fn))(arguments, argument_count-1); //-1 because we are excluding the NULL at the end
//      return true;
//     }
//  return false;
//}


std::list <std::string> CommandHistory;
std::list <std::string>::iterator cmdhist_it = CommandHistory.begin();


pthread_mutex_t lock;


void sigintHandler(int sig_num){
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


/* Include all builtin functions of mard */
#include "./include/mrd_fn.h"


/* ================ Assistive Terminal Functions START ================ */
enum EXPRS {
	NO_INPUT_FLAG,
	DAEMON,
	STD_REDIR,
	PIPE,
	PIPE_END
};


int childPid = 0;


void* 	daemonListener(void * args){
//	printf("In the listener\n");
	int * argv = (int *)args;
	int childPid = argv[0];
	int flag = argv[1];
	if (flag != PIPE)
		printf("[%lu] %d\n", pthread_self(), childPid);
	waitpid(childPid, NULL, 0);
	if (flag != PIPE)
		printf("[%lu]\tDone\t\t\t%d\n", pthread_self(), childPid);
	if (argv){
		free (argv);
		argv = NULL;
	}

	/* toDo: return status of child */
	return (void*)0;
}
void 	setCWD(){
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
		if (CURRENT_PATH){
			free (CURRENT_PATH);
			CURRENT_PATH = nullptr;
		}
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

	if (CURRENT_PATH){
		free(CURRENT_PATH);
		CURRENT_PATH = nullptr;
	}

}
void 	fancyInput (int &characters, char* &input, int &CURSOR_POS){
    int c; // used to capture the ASCII value of the input

    /* use system call to make terminal send all keystrokes directly to stdin */
    system ("/bin/stty raw");
    int specialKeyCount = 0;
    std::string nbInput="";
    int specialInput [] = {0, 0, 0};

    CURSOR_POS = 0;

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
						CURSOR_POS = nbInput.size();
//							printf("[%d]", CURSOR_POS);
//							printf("[%d]", nbInput.size());

//							printf("[%d]", CURSOR_POS);

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

						CURSOR_POS = nbInput.size();

//						printf("[nbInput is %s]", nbInput.c_str());
					}
					else{
						printf("\b\b\b\b    \b\b\b\b");
					}
//						printf("\b\b\b\b[User pressed DOWN!]");
					specialKeyCount = 0;
				}
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
						int num = nbInput.size() - CURSOR_POS;

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
//				printf("[%d]", CURSOR_POS);
		}


//	    	std::cout<<"[Cursor position is "<<CURSOR_POS<<"]";


//	    	moveCursor();


    }

    /* use system call to set terminal behavior to more normal behavior */
    system ("/bin/stty cooked");

    /* remove Enter ASCII ("^M") */
    printf("\b\b  \b\b%s\n", &nbInput[CURSOR_POS]);

    /* copy the nbInput string to the official input char* */
    strcpy( input, nbInput.c_str());

    /* add command to Command History */
    if (nbInput != ""){
    	if (nbInput != CommandHistory.back()) //avoid repeating commands
    		CommandHistory.push_back(nbInput);
    }

}
void 	normalInput (int &characters, char* &input, unsigned long int &inputsize){
		characters = getline(&input, &inputsize, stdin);
		input[characters-1] = '\0';
}

struct 	Token{
	const char token;
	bool isIncluded;
} tokenTypes_lookup_table[] =
	{
		{	' ', false	 },
		{	'<', true	 },
		{	'>', true	 },
		{	'|', true	 },
		{	'&', true	 },
		{	 0 , false	 },
		{	'?', false	 }  	//ending delimiter
	};

Token 	token_lookup (const char token){
	int i;
	for (i=0; tokenTypes_lookup_table[i].token != '?'; i++){
		if (token == tokenTypes_lookup_table[i].token){
			return tokenTypes_lookup_table[i];
		}
	}
	return tokenTypes_lookup_table[i];
}
void 	tokenizeInput (char* &input, std::vector<char*>& arguments, const char* token = " "){
	int START_POS = 0;
	int END_POS = 0;

	for (int i=0; i<strlen(input)+1; i++){
		auto found = token_lookup(input[i]);
		if (found.token != '?'){
			END_POS = i;
			if (END_POS - START_POS>0){
				char* arg = (char*)malloc( (END_POS-START_POS+1) * sizeof(char));
				strncpy(arg, &input[START_POS], END_POS - START_POS);
				arg[END_POS-START_POS] = '\0';
				if (strcmp(arg, " ") != 0) //prevents pre-spaces in command
					arguments.push_back(arg);
				if (found.isIncluded){
					char* argt = (char*)malloc(2 * sizeof(char));
					argt[0] = found.token;
					argt[1] = '\0';
					arguments.push_back(argt);
//					free (argt);  //cannot free because it would create a dangling pointer!
				}
				i++;
				if (input[i]=='\0')
					break;

				char foundc;
				do {
					auto foundnext = token_lookup(input[i]);
					foundc = foundnext.token;
					if (foundnext.isIncluded){
						char* argt = (char*)malloc(2 * sizeof(char));
						argt[0] = foundnext.token;
						argt[1] = '\0';
						arguments.push_back(argt);
	//					free (argt);  //cannot free because it would create a dangling pointer!
					}
					i++;
				}while (foundc!='?');
				START_POS = i-1;
			}
		}
	}

//	printf("Printing arguments\n");
//	printf("%d\n", arguments.size());
//	for (int i=0; i<arguments.size(); i++){
//		printf("%d. %s\n", i, arguments[i]);
//	}
	arguments.push_back(NULL);
}

#define DEFAULT_STD_OUT 1
#define DEFAULT_STD_IN 0

struct Command {
	std::vector<char*> arguments;
	int output_fd;
	int input_fd;
	int flag;

	Command ():output_fd(DEFAULT_STD_OUT),  input_fd(DEFAULT_STD_IN), flag(NO_INPUT_FLAG){}
//	Command (int ofd, int ifd, int flg=NO_INPUT_FLAG):output_fd(ofd),  input_fd(ifd), flag(flg){}
};


int 	evalExpression(std::vector<char*>& arguments, std::vector<Command>& commands){
	int CURRENT_FLAG = -1;

	for (int i=0; i<arguments.size()-1; i++){
		if (strcmp(arguments[i], ">") == 0){
			int fd = open(arguments[++i], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			commands.back().output_fd = fd;
			commands.back().flag = STD_REDIR;
		}
		else if (strcmp(arguments[i], "<") == 0){
			int fd = open(arguments[++i], O_RDWR, S_IRUSR | S_IWUSR);
			commands.back().input_fd = fd;
			commands.back().flag = STD_REDIR;
		}
		else if (strcmp (arguments[i], "|") == 0){
			int fd[2];
			pipe (fd);
			commands.back().output_fd = fd[1];
			commands.back().arguments.push_back(NULL);
			commands.back().flag = PIPE;

			Command pushTo;
			commands.push_back(pushTo);

			commands.back().input_fd = fd[0];
			commands.back().flag = PIPE_END;
		}
		else if (strcmp (arguments[i], "&") == 0 and i!=arguments.size()-2 and strcmp(arguments[i+1], "&") == 0){
			commands.back().arguments.push_back(NULL);
			Command pushTo;
			commands.push_back(pushTo);
			i++;
		}
		else{
			if (commands.empty()){
				Command pushTo;
				commands.push_back(pushTo);
			}
			commands.back().arguments.push_back(arguments[i]);
		}
		CURRENT_FLAG = 0;
	}

	commands.back().arguments.push_back(NULL);

// //	printing commands and their arguments
//	for (int i=0; i<commands.size(); i++){
//		printf("Command #%d: ", i);
//		for (int j=0; j<commands[i].arguments.size()-1; j++){
//			printf ("%s, ", commands[i].arguments[j]);
//		}
//		printf("\n");
//	}

	return CURRENT_FLAG;
}



int 	understandInput(char* &input, std::vector<char*>& arguments){

//	printf("%s\n", input);


    /* tokenize the input and place it into a vector of char* */
	tokenizeInput(input, arguments, " ");

	if (arguments[0] == NULL){
		return -1;
	}

	return NO_INPUT_FLAG;

}
int 	implementInput(std::vector<char*> &arguments, int FLAG = NO_INPUT_FLAG){
	int childPid = 0;

	std::vector <pthread_t> daemonListeners;

	/* before launching, we check if it is an expression or an environment variable definition */
	/* a vector of commands */
	std::vector<Command> commands;

	int _EXPR = evalExpression(arguments, commands);

	for (int i=0; i<commands.size(); i++){

		// checks if Daemon process or not (ampersand '&' in the end)
		if (strcmp(commands[i].arguments[commands[i].arguments.size()-2], "&") == 0){
			commands[i].arguments.pop_back();
			commands[i].arguments.pop_back();
			commands[i].arguments.push_back(NULL);
			FLAG = DAEMON;
		}

		char ** argv  = &commands[i].arguments[0];


		//checks if a builtin bash command
		if (FLAG != DAEMON and commands[i].input_fd==DEFAULT_STD_IN and commands[i].output_fd==DEFAULT_STD_OUT and lookup_and_call(argv, commands[i].arguments.size())) {}
		else if ((childPid = fork()) == 0){
			setenv("parent", getenv("SHELL"), true);


//			printf ("command is %s\n", commands[i].arguments[0]);
//			printf ("input fd is %d, output fd is %d\n", commands[i].input_fd, commands[i].output_fd);


			/* redirect I/O if the default input and output have been changed */
			if (commands[i].input_fd!=DEFAULT_STD_IN){
//				printf ("Second I'm here\n");
				dup2(commands[i].input_fd, DEFAULT_STD_IN);
//				close (3);
//				close (4);
			}
			if (commands[i].output_fd!=DEFAULT_STD_OUT){
//				printf ("First I'm here\n");
				dup2(commands[i].output_fd, DEFAULT_STD_OUT);
//				close (3);
//				close (4);
			}

			//If it's a daemon, checks if a builtin bash command
			if (lookup_and_call(argv, commands[i].arguments.size())) {
				exit(0);
			}


			//is a system command
			if (strchr(arguments[0],'/') == nullptr){
				//is a Virtual System command
				std::string defpath = getenv("SHELL");
				defpath += "/bin/";
				std::string path = defpath + commands[i].arguments[0];
				execv(path.c_str(), argv);


				//is a Native System command
				char* allpaths = getenv("PATH");
				path = strtok (allpaths,":");
				while (path.c_str() != nullptr) {
					path = path + "/" + commands[i].arguments[0];
//					printf("%s\n", path.c_str());
					execv(path.c_str(), argv);
					path = strtok(NULL, ":");
					if (path == "\0")
						break;
				}
	//			    printf("hello world");

				//if both commands fail
				printf("Command '%s' not found\n",commands[i].arguments[0]);
	//				fflush(stdout); // Will now print everything in the stdout buffer
			}
			//is a file or directory
			else{
				std::string path = commands[i].arguments[0];
				execv(path.c_str(), argv);
				printf("mard: %s: No such file or directory\n", commands[i].arguments[0]);
			}

			//everything failed
			exit(1);
		}
		else{


	//		printf("InpE.ThreadType is %d\n", InpE.THREAD_TYPE);
			if (FLAG == DAEMON or commands[i].flag == PIPE){
				if (commands[i].output_fd != DEFAULT_STD_OUT)
					close (commands[i].output_fd);
				daemonListeners.push_back(0);
				int *argsToSend = (int*)malloc(2*sizeof(int));
				argsToSend[0] = childPid;
				argsToSend[1] = commands[i].flag;
				pthread_create(&daemonListeners.back(), NULL, daemonListener, (void*)argsToSend);
			}
			else{
				if (commands[i].input_fd != DEFAULT_STD_IN and commands[i].flag == PIPE_END)
					close (commands[i].input_fd);
				waitpid(childPid, NULL, 0);
			}
			// clear argument list in the meantime...
	//		for (int i=0; i<arguments.size(); i++){
	//			if (arguments[i])
	//				free (arguments[i]);
	//		}
	//
	//		for (int i=0; i<arguments.size(); i++){
	//			arguments.pop_back();
	//		}

//			LHS.clear();
		}

//		printf ("Start point in expr loop is %d", START_POINT);
//		_EXPR = evalExpression(arguments, commands);
	}
}
void 	inputLoop(){
	char * input;
	size_t inputsize = 100;
    input = (char *)malloc(inputsize * sizeof(char));
    int CURSOR_POS = 0;

    /* Runs forever ;) */
    for ever {
	    Color::Modifier green(Color::FG_GREEN);
	    Color::Modifier def(Color::FG_DEFAULT);
	    Color::Modifier blue(Color::FG_BLUE);
	    setCWD();
	    printf("%s%s@%s%s:%s%s%s > ", green.getColor().c_str(), userInfo->pw_name, HOST_NAME, def.getColor().c_str(), blue.getColor().c_str(), CURRENT_PATH_RELATIVE, def.getColor().c_str());

	    /* contains the number of characters read by stdin
	     * in both normalInput and fancyInput
	     */
	    int characters = 0;

	    /* flush buffer to prevent leftover input from programs disturbing the terminal input */
	    fflush(stdin);

	    #ifdef linux
	    /* For Linux systems only, right now */
	    fancyInput(characters, input, CURSOR_POS);
		#else
	    /* For Mac, LSW, UNIX and other systems */
	    normalInput(characters, input, inputsize);
		#endif


		std::vector <char *> arguments;

		auto didItUnderstand = understandInput (input, arguments);
		if (didItUnderstand < 0)
			continue;


		/* causes segfaults and double free or corruption (fasttop) */
		// clear input...
//		if (input){
//			free (input);
////			input = nullptr;
//		}

		implementInput (arguments, didItUnderstand);
    }
}
void 	initializeTerminal(){
    signal(SIGINT, sigintHandler);
	clearScreen(NULL, 0);
	getcwd(DEFAULT_PATH, defaultpathsize);

	// Setting SHELL's default path
	setenv("SHELL", DEFAULT_PATH, 1);

	for (int i=0;; i++){
		if (DEFAULT_PATH[i]=='\0'){
				defaultpathsize=i;
				break;
		};
	}
	if (DEFAULT_PATH){
//		printf("boutta do a fasttop error");
		free(DEFAULT_PATH);
		DEFAULT_PATH = nullptr;
	}

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

	printf("Welcome back to your %s (%s) system", systemInfo.sysname, systemInfo.machine);
	if (userName.c_str())
		printf(", %s", userName.c_str());
	printf("!\n\n");

	printf("mard is a chad 3rd-party bash. It comes with command history, a virtual machine, and even built-in games.\n");
	printf("Use 'system' before a Linux/UNIX system call to use the machine's default system calls.\n\n");

	printf("The default path is: %s\n", getenv("SHELL"));

	printf("Env PATH variable is: %s\n\n", getenv("PATH"));

}
/* ================ Assistive Terminal Functions END ================ */


/* driver code */
int main(int argc, char **argv) {
	initializeTerminal();
	inputLoop();
	return 0;
}
