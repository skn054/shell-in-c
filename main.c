#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFSIZE 1024
#define DELIM " \t\r\n\a"


char* built_str[] = {"cd","help","exit"};

int shell_cd(char **args){
    if(args[1] == NULL){
         fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }else{
        if(chdir(args[1]) !=0){
            perror("lsh");
        }
    }

    return 1;

}

int num_of_built_ins(){
    return sizeof(built_str) / sizeof(char *);
}

int shell_help(char **args){
    printf("The following are built in:\n");

    for(int i=0;i<num_of_built_ins;i++){
        printf("%s \n",built_str[i]);
    }
    return 1;
}

int shell_exit(char **args){
    
    return 0;
}

int (*built_func[]) (char**) = {
    &shell_cd,
    &shell_help,
    &shell_exit
};



char* read_from_stdin(){
    int buff_size = BUFFSIZE;
    int ch;
    int count =0;

    char *std_input = malloc(BUFFSIZE * sizeof(char));
    if(std_input == NULL){
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    /** read from stdin character by charcater and store it in stdinput */
    
    while(1){
        ch = getchar();

        if(ch == EOF || ch == '\n'){
            std_input[count] = '\0';
            return std_input;
        }

        if(count >= BUFFSIZE){
            buff_size +=BUFFSIZE;
            std_input = realloc(std_input, buff_size);
            if(std_input == NULL){
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }

        }

        std_input[count++] = ch;

    }
    

}

char **break_into_commans_args(char *line){
    // break input into array of stirngs using a delimiter.
    char *input = strdup(line);
    int buff_size = BUFFSIZE;
    int count = 0;
    char **tokens = malloc(sizeof(char *) * BUFFSIZE);
    if(tokens == NULL){
        fprintf(stderr, "args : allocation error\n");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input,DELIM);
    
    
   
    while ((token!=NULL))
    {
       tokens[count++] = token;
       
       if(count >= BUFFSIZE){
            buff_size+=BUFFSIZE;
            tokens = realloc(tokens,sizeof(char *) * buff_size);
            if(tokens == NULL){
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }

       }
       token = strtok(NULL,DELIM);


    }
    tokens[count] = NULL;
    free(input);

    return tokens;
    

}

int launch_shell(char **args){
    pid_t pid,wpid;
    pid = fork(); // creates a duplicate process. pid of child prcoess will be 0. pid of parnet process will pid of child.
    int status;
    if(args[0] == NULL){
        return 0;
    }
    char *command = args[0];
    // how to pass args[1...n] to exec command?

    
    if(pid == 0){
        // child process
        // call exec system call.
        if(execvp(args[0],args) == -1){
            perror("error executing execvp command");
        }
        exit(EXIT_FAILURE);
    }else if(pid <0){
        // error creating a fork.
        perror("error creating child process");

    }else{
        // parent process
        //wait until child gets executed using wait system call.
        do{
            wpid = waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status))// wait for child process to completely terminated before giving control to parent.


        
    }

    return 1;
}

void launch_loop(){
    char *line;
    char **args;
    int status;


     do{
         printf("> ");
        line = read_from_stdin(); // returns a string of input from stdin.
        args = break_into_command_args(line); // splits above string into array of stings.
        status = launch_execute(args); // run the command given in stdin and execute it. 
        /** status returns 0, if we exit choose to exit from shell. */
        /** execute the loop until status is 0. */

        free(line); // free block of memory pointed to by line.
        free(args);
     }while(status);
   
}

void launch_execute(char **args){

    // if any built in executed, then execute as a command in current process.
    if(args[0] == NULL){
        return 1;
    }

    for(int i=0;i<num_of_built_ins;i++){
        if(strcmp(args[0],built_str[i]) == 0){
            return (*built_func[i])(args);
        }
    }

    return launch_shell(args);

}



void main(){
    
    printf("welcome to shell!");
    launch_loop();

     
}