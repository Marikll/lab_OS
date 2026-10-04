#include <stdio.h>
#include <stdlib.h>

int main(void){
    char *line = NULL;
    size_t size = 0;

    setvbuf(stdout, NULL, _IONBF, 0);

    while (getline(&line, &size, stdin) != -1) {
        size_t read_index = 0;
        size_t write_index = 0;
        int previous_space = 0;

        while (line[read_index] != '\0') {
            if (line[read_index] == ' ') {
                if (!previous_space) {
                    line[write_index++] = line[read_index];
                    previous_space = 1;
                }
            } else {
                line[write_index++] = line[read_index];
                previous_space = 0;
            }

            ++read_index;
        }

        line[write_index] = '\0';
        fputs(line, stdout);
    }

    free(line);
    return 0;
}