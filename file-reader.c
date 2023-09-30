// given a path, print all of the files within that path
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "colours.h"

void print_directory(DIR* directory, char* path);
void print_file(char* filename, char* colour);
void argument_check(int argc, char* argv[]);
int check_current_directory(int argc, char* argv[]);

enum print_level {
    NORMAL,
    VERBOSE
};
enum print_level verbosity = NORMAL;
const int print_padding = 9; // dont worry about the hardcoded value...

int main(int argc, char* argv[]) { 
    DIR* directory;
    char cwd[PATH_MAX]; 
    char* directory_name = NULL;

    argument_check(argc, argv);

    // if we should check the current directory
    if (check_current_directory(argc, argv)) {
        // if we can't get the current directory, throw an error
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            printf("Error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        // otherwise, open the directory and set directory_name
        directory = opendir(cwd);
        directory_name = cwd;
    } else {
        if (argc == 3) {                    // ./list -v <path>
            directory = opendir(argv[2]);
            directory_name = argv[2];
        } else {
            directory = opendir(argv[1]);
            directory_name = argv[1];
        }

        if (directory == NULL) {
            printf("Error opening directory \'%s\': %s\n", argv[2], strerror(errno));
            return EXIT_FAILURE;
        }

    }
    print_directory(directory, directory_name);
    return EXIT_SUCCESS;
}

int check_current_directory(int argc, char* argv[]) {
    /*
         if we dont supply the command with a path, 
         we want to check the current working directory.
     */
    if (verbosity == VERBOSE && argc == 2) {
        return 1;
    } else if (verbosity == NORMAL && argc == 1) {
        return 1;
    } else {
        return 0;
    }
}

void argument_check(int argc, char* argv[]) {
    if (argc == 2 || argc == 3) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("./list <args> <path>\n"
                   "-h, --help:\tdisplays help\n"
                   "-v, --verbose\tverbose print\n");
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0) {
            verbosity = VERBOSE;
        } else if (argv[1][0] != '-' && argc == 3) {
            exit(EXIT_FAILURE); // this is still a little hacky for now
        }
    } else if (argc > 3) {
        printf("Too many arguments given.\n");
        exit(EXIT_FAILURE);
    }
}

void print_directory(DIR* directory, char* path) {
    /*
        print the directory given by DIR* and the path,
        also format the print based on whether the user asked for verbose print
     */
    struct dirent* dir;
    
    if (path == NULL) {
        // path should NEVER be NULL, but you never know
        printf("path is NULL, how have you managed this?\n"); 
        exit(EXIT_FAILURE);
    }

    if (chdir(path) == -1) {
        printf("%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while ((dir = readdir(directory)) != NULL && ++count) {
        switch(dir->d_type) {
            case DT_BLK:
                print_file(dir->d_name, YEL);
                break;
            case DT_CHR:
                print_file(dir->d_name, YEL);
                break;
            case DT_DIR:
                print_file(dir->d_name, BLU);
                break;
            case DT_FIFO:
                break;
            case DT_LNK:
                print_file(dir->d_name, CYN);
                break;
            case DT_REG:
                print_file(dir->d_name, WHT);
                break;
            case DT_SOCK:
                print_file(dir->d_name, MAG);
                break;
            default: // DT_UNKNOWN
                print_file(dir->d_name, RED);
                break;
        }
    } if (verbosity == VERBOSE) {
        // Prints the amount of files in a directory, excluding ../ and ./
        printf("%d files in directory\n", count-2);
    }
}

void print_file(char* filename, char* colour) {
    struct stat file_stats; 
    struct passwd* pwuser;
    struct group* groupuser;
    char date[40];

    if (lstat(filename, &file_stats) == 0) {
        // https://pubs.opengroup.org/onlinepubs/7908799/xsh/pwd.h.html
        
        if ((pwuser = getpwuid(file_stats.st_uid)) == NULL) {
           printf(RED "%s\n", strerror(errno));
           return;
        }
        
        if ((groupuser = getgrgid(file_stats.st_gid)) == NULL) {
            printf(RED "%s\n", strerror(errno));
            return;
        }
        
        if (verbosity == VERBOSE) {
            const char* format = "%b %d %Y - %H:%M";
            strftime(date, 80, format, localtime(&(file_stats.st_ctime)));
            printf(YEL"%s ", groupuser->gr_name);
            printf(YEL"%s ", pwuser->pw_name);
            printf(BLU"%s ", date);
            /*
                To nicely print out the filesize (rougly) we can do:
                file_stats.st_size >> 10 --> bytes to KB
                and then if we did that again, we would get
                file_stats.st_size >> 10 --> KB to MB
            */
            printf(GRN"%*lu ", print_padding, file_stats.st_size );
        }
        
        if ((strcmp(colour, BLU)) == 0) {
            printf("%s%s/", colour, filename);
        } else {
            printf("%s%s", colour, filename);
        }
        printf(RESET "\n");
    }
}
