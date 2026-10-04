#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void){
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];

    if (pipe(pipe1) == -1){
        perror("pipe1");
        return 1;
    }

    if (pipe(pipe2) == -1){
        perror("pipe2");
        close(pipe1[0]);
        close(pipe1[1]);
        return 1;
    }

    if (pipe(pipe3) == -1){
        perror("pipe3");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return 1;
    }

    pid_t pid1 = fork();

    if (pid1 == -1){
        perror("fork child1");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);
        return 1;
    }

    if (pid1 == 0){

        if (dup2(pipe1[0], STDIN_FILENO) == -1){
            perror("dup2 child1 stdin");
            _exit(1);
        }

        if (dup2(pipe3[1], STDOUT_FILENO) == -1){
            perror("dup2 child1 stdout");
            _exit(1);
        }

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);

        execl("./child1", "child1", (char *)NULL);

        perror("execl child1");
        _exit(1);
    }

    pid_t pid2 = fork();

    if (pid2 == -1){
        perror("fork child2");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);
        return 1;
    }

    if (pid2 == 0){

        if (dup2(pipe3[0], STDIN_FILENO) == -1) {
            perror("dup2 child2 stdin");
            _exit(1);
        }

        if (dup2(pipe2[1], STDOUT_FILENO) == -1) {
            perror("dup2 child2 stdout");
            _exit(1);
        }

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);

        execl("./child2", "child2", (char *)NULL);

        perror("execl child2");
        _exit(1);
    }

    close(pipe1[0]);
    close(pipe3[0]);
    close(pipe3[1]);
    close(pipe2[1]);

    FILE *output = fdopen(pipe2[0], "r"); 
    if (output == NULL){ 
        perror("fdopen");

        close(pipe1[1]); 
        close(pipe2[0]); 

        waitpid(pid1, NULL, 0); 
        waitpid(pid2, NULL, 0); 
        return 1; 
    }

    char *line = NULL;
    size_t size = 0;

    while (getline(&line, &size, stdin) != -1){

        size_t length = 0;

        while(line[length] != '\0')
            length++;
        
        size_t written_total = 0;

        while (written_total < length){

            ssize_t written = write(pipe1[1], line + written_total, length - written_total);

            if (written == -1){
                perror("write pipe1");
                free(line);
                fclose(output);

                waitpid(pid1, NULL, 0); 
                waitpid(pid2, NULL, 0);
                return 1;
            }

            written_total += (size_t)written;
        }

        char *result = NULL;
        size_t result_size = 0;

        if (getline(&result, &result_size, output) == -1){
            free(result);
            break;
        }

        printf("%s", result);

        free(result);
    }

    free(line);

    close(pipe1[1]);
    fclose(output);

    if (waitpid(pid1, NULL, 0) == -1){ 
        perror("waitpid child1"); 
        return 1; 
    } 

    if (waitpid(pid2, NULL, 0) == -1){ 
        perror("waitpid child2"); 
        return 1; 
    }
    return 0;
}