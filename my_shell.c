#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>
#include <signal.h>

char **split(char str[], char tok[], int *num_elements);
void handle_sigint(int sig);

int main(int argc, char **argv) {

    // catch Ctl-C signal
    signal(SIGINT, handle_sigint);

    // loop indefinitely
    while (true){

        // print shell prompt
        printf("CPSC405-jbuechlersh$ ");
		
        // get input
        char input[100];
        if (fgets(input, 100, stdin) == 0){
            // Ctl-D signal, terminate shell
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

		/* If the user wrote a command that is not just pressing ENTER and the first character in the array 
		 * is different from '\0' , then we treat his input. If not, we should just skip and prompt again for a command */
		
		if (*input != '\0')
		{
			// split input
			int num_args;
			char **args = split(input, " ", &num_args);
			char *cmd = args[0];

			// determine command args (pipe/redirect/background)
			bool pipe_cmd = false;
			bool redir_in_cmd = false;
			bool redir_out_cmd = false;
			bool background_cmd = false;
			for (int i = 0; i < num_args; i++){
				if ((strcmp(args[i], "|") == 0))
					pipe_cmd = true;
				else if ((strcmp(args[i], ">") == 0))
					redir_out_cmd = true;
				else if ((strcmp(args[i], "<") == 0))
					redir_in_cmd = true;
				else if ((strcmp(args[i], "&") == 0))
					background_cmd = true;
			}

			// ***PROCESS COMMANDS***
			// CD (internal)
			if (strcmp(cmd, "cd") == 0){
				if (num_args > 2)
					printf("%s: too many arguments\n", cmd);
				else if (chdir(args[1]) == -1)
					printf("%s: %s: no such file or directory\n", cmd, args[1]);
			}
			// PWD (internal)
			else if (strcmp(cmd, "pwd") == 0){
				// print cwd
				char cwd[100];
				printf("%s\n", getcwd(cwd, 100));
			}
			// EXIT (internal)
			else if (strcmp(cmd, "exit") == 0){
				break;
			}
			// EXTERNAL W/ PIPE
			else if (pipe_cmd == true){

				int count_left = 0;
				int count_right = 0;
				bool swap = false;
				// count left and right args
				for (int i = 0; i < num_args; i++){
					if ((strcmp(args[i], "|") == 0)){
						swap = true;
					}
					else {
						if (swap == true)
							count_right++;
						else
							count_left++;
					}
				}
            
				char *left_args[count_left+1];
				char *right_args[count_right+1];
				int index = 0;
				int temp_dex = 0;
				// populate left args array
				for (; index < num_args; index++){
					if ((strcmp(args[index], "|") == 0)){
						index++;
						break;
					}
					left_args[index] = args[index];
				}
				// null terminate left args array
				left_args[index-1] = 0;
            
				// populate right args array
				for (; index < num_args; index++){
					if ((strcmp(args[index], ">") == 0)){
						index++;
						break;
					}
					right_args[temp_dex] = args[index];
					temp_dex++;
				}
				// null terminate right args array
				right_args[temp_dex] = 0;
            
				int fd[2];
				pipe(fd);
				// fork left child, exec left cmds
				if (fork() == 0) {
					dup2(fd[1], 1);
					close(fd[0]);
					close(fd[1]);
					execvp(left_args[0], left_args);
					printf("%s: command not found\n", left_args[0]);
					exit(1);
				}
				else{
					// fork right child, exec right cmds
					if (fork() == 0) {
						dup2(fd[0], 0);
						close(fd[0]);
						close(fd[1]);
						execvp(right_args[0], right_args);
						printf("%s: command not found\n", right_args[0]);
						exit(1);
					}
					// parent process
					else{
						close(fd[0]);
						close(fd[1]);
						wait(NULL);
						wait(NULL);
					}
				}
			}
			// EXTERNAL W/ IO REDIRECTION
			else if (redir_out_cmd == true || redir_in_cmd == true){

				char *nt_args[num_args-1];
				char *redir_loc;
				int i = 0;
				// get redirection location and seperate args
				for (; i < num_args; i++){
					if ((strcmp(args[i], ">") == 0) || (strcmp(args[i], "<") == 0)){
						redir_loc = args[i+1];
						break;
					}
					nt_args[i] = args[i];
				}
				// null terminate args array
				nt_args[i] = 0;
            
				// fork and exec cmds
				if (fork() == 0) {

					// output redirection
					if (redir_out_cmd == true){
						// create fd for outfile and dup to stdout
						int redirect_fd = open(redir_loc, O_CREAT|O_TRUNC|O_WRONLY, 0644);
						dup2(redirect_fd, STDOUT_FILENO);
						close(redirect_fd);
					}
					// input redirection
					else if (redir_in_cmd == true){
						// create fd for infile and dup to stdin
						int redirect_fd = open(redir_loc, O_RDONLY);
						dup2(redirect_fd, STDIN_FILENO);
						close(redirect_fd);
					}
					execvp(nt_args[0], nt_args);
					printf("%s: command not found\n", cmd);
					exit(1);
				}
				wait(NULL);
			}
			// EXTERNAL BASIC (w/o pipe and IO redirection, w/ background running)
			else{
				if (fork() == 0) {
					if (background_cmd == true){
						char *nt_args[num_args];
						int i = 0;
						for (; i < num_args; i++){
							if (strcmp(args[i], "&") == 0)
								break;
							nt_args[i] = args[i];
						}
						// null terminate args array
						nt_args[i] = 0;

						execvp(nt_args[0], nt_args);
						printf("%s: command not found\n", cmd);
						exit(1);
					}
					else{
						char *nt_args[num_args+1];
						int i = 0;
						for (; i < num_args; i++){
							nt_args[i] = args[i];
						}
						// null terminate args array
						nt_args[i] = 0;

						execvp(nt_args[0], nt_args);
						printf("%s: command not found\n", cmd);
						exit(1);
					}
				}
				// if running in background, do not wait
				if (background_cmd == false)
					wait(NULL);
			}
		}
    }
}
// handler for SIGINT (Ctrl-C at keyboard)
void handle_sigint(int sig){
    printf(" caught signal: %d\n", sig);
}

// split.c helper
char **split(char str[], char tok[], int *num_elements) {
    char count_str[255];
    strcpy(count_str, str);
    char *tokens[100];  
    char *pch;
    int strings_in_str = 0;
    pch = strtok(count_str,tok);
    while (pch != NULL) {
        tokens[strings_in_str] = pch;
        pch = strtok (NULL,tok);
        strings_in_str++;
    }
    char **array = malloc (sizeof (char *) * strings_in_str);
    if (!array)
        return NULL;
    for (int i = 0; i < strings_in_str; i++) {
        array[i] = malloc(strlen(tokens[i])+1);
        if (!array[i])
            return NULL;
        strcpy(array[i], tokens[i]);
    }
    *num_elements = strings_in_str;
    return array;
}
