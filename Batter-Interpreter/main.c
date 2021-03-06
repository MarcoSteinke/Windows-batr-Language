#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_checking.h"
#include "win.h"
#include "file_writer.h"
#include "string_list.h"
#include "error.h"
#include "printer.h"
#include "parser.h"

// replication of the (String) indexOf() method from Java
int find_symbol(char* string, char symbol)
{
    // iterate through the string until the symbol occurs.
    for (size_t i = 0; i < strlen(string); i++)
    {
        // if symbol is found, return its index
        if(string[i] == symbol)
        {
            return i;
        }
    }
    return -1;
}

char* cut_file_ending(char* file_name)
{
    unsigned int _index = 0;
    size_t file_name_length = strlen(file_name);
    char* result = malloc(sizeof(char) * (file_name_length+1));

    // iterate through the filename until a . appears
    for(int i = 0; i < file_name_length; i++)
    {
        if(*(file_name + i) == '.')
        {
            _index = i;
            break;
        }
    }

    // copy the filename to a new variable
    strcpy(result, file_name);

    // cut the file ending starting at _index
    result[_index] = '\0';

    return result;
}

int main(int argc, char const *argv[])
{
    // alloc memory for the file names
    char source_file_name[20];
    char compiled_file_name[20];

    char* compared_version = "-v";

    // determine which program call was used
    if(argc > 0) {

        // copy the source file's name from the args
        strcpy(source_file_name, argv[1]);

        // if first parameter was -v
        if(argc == 2 && strcmp(source_file_name, compared_version) == 0)
        {
            print_to_console("Version: 0.1");
            printf("This version of batter is the first working build. It is only able to\nreceive arguments, but not to interpret anything yet.");

            return 0;
        } 
        else if(argc == 2 && strcmp(source_file_name, "-v") != 0) 
        {

            // copy the source file's name from the args
            strcpy(source_file_name, argv[1]);

            int res = check_file_type(source_file_name);
            if(res == -1)
            {
                print_error("File is not of the type \".batr\"");
                return 1;
            }

            // remove file ending from source_file_name and store it into compiled_file_name
            char* cut = cut_file_ending(source_file_name);
            strcpy(compiled_file_name, cut);
            free(cut);

            strcat(compiled_file_name, ".bat");
        }
        else if(argc > 2) {

            // used later for commands during compilation
            // if arg == something  

            if(argc == 3) {
                strcpy(compiled_file_name, argv[2]);
                strcat(compiled_file_name, ".bat");

            }          
        }
    }

    // setup the source_file
    FILE* source_file;
    source_file = fopen(source_file_name, "r+");
    if(source_file == NULL) exit(-1);

    // check if a module was defined in the source_file
    int state = check_for_module(source_file);

    // if there was no module, throw an error
    if(state == -1)
    {
        print_error("Stopped the interpretation of:");
        set_color_green();
        printf("           %s\n", source_file_name);
        restore_color();

        fclose(source_file);
        return 1;
    }
    // this is the point where a valid .batr with a module exists.
    fclose(source_file);
    source_file = fopen(source_file_name, "r+");

    // now it is required to find the main method.
    int main_exists = find_method("main", source_file);

    if(main_exists != 0)
    {

        if(main_exists == 1)
        {
            print_error("There was no main defined");
        }
        else if(main_exists == 2)
        {
            print_error("There was a problem with the scopes.");
        }

        fclose(source_file);
    }

    // initialize lists for the resulting bat code and collect error messages.
    string_list* interpreted_bat_code = create();
    string_list* error_list = create();

    // reset source_file pointer
    fclose(source_file);
    source_file = fopen(source_file_name, "r+");

    unsigned int line_counter = 1;

    unsigned short interpretation_state = interpret(
        source_file, 
        &line_counter, 
        interpreted_bat_code, 
        error_list
    );

    // close source_file and complete the algorithm.
    fclose(source_file);

    delete(interpreted_bat_code);
    delete(error_list);

    return 0;
} 
