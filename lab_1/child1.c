#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void){
    char *line = NULL;
    size_t size = 0;

    setvbuf(stdout, NULL, _IONBF, 0);

    while (getline(&line, &size, stdin) != -1) {
        for (size_t i = 0; line[i] != '\0'; ++i)
            line[i] = (char)tolower((unsigned char)line[i]);

        fputs(line, stdout);
    }

    free(line);
    return 0;
}