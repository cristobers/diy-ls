// given a path, print all of the files within that path

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <sys/stat.h>
#include <sys/types.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void print_directory(DIR* directory, char* path);
void print_file(char* filename, char* colour);
int check_current_directory(int argc, char* argv[]);

int main(int argc, char* argv[]) { 
    DIR* directory;
    char cwd[PATH_MAX]; 
    char* directory_name = NULL;
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
        if ((directory = opendir(argv[1])) == NULL) {
            printf("Error opening directory \'%s\': %s\n", argv[1], strerror(errno));
            return EXIT_FAILURE;
        }
        directory_name = argv[1];
    }
    print_directory(directory, directory_name);
    return EXIT_SUCCESS;
}

int check_current_directory(int argc, char* argv[]) {
    if (argc != 2) 
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void print_directory(DIR* directory, char* path) {
    struct dirent* dir;
    
    if (path == NULL) {
        // path should NEVER be NULL, but you never know
        printf("how have you managed this\n"); 
        exit(EXIT_FAILURE);
    }

    if (chdir(path) == -1) {
        printf("%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((dir = readdir(directory)) != NULL) {
        switch(dir->d_type) {
            case DT_DIR:
                print_file(dir->d_name, BLU);
                break;
            case DT_REG:
                print_file(dir->d_name, WHT);
                break;
            case DT_LNK:
                print_file(dir->d_name, MAG);
                break;
            default:
                print_file(dir->d_name, RED);
                break;
        }
    }
}

void print_file(char* filename, char* colour) {
    struct stat file_stats; 
    struct passwd* pwuser;
    struct group* groupuser;

    if (lstat(filename, &file_stats) == 0) {
        // https://pubs.opengroup.org/onlinepubs/7908799/xsh/pwd.h.html
        
        if ((pwuser = getpwuid(file_stats.st_uid))== NULL) {
           printf(RED "%s\n", strerror(errno));
           return;
        }
        
        if ((groupuser = getgrgid(file_stats.st_gid)) == NULL) {
            printf(RED "%s\n", strerror(errno));
            return;
        }
        
        int width = 10;
        printf(WHT"%s ", groupuser->gr_name);
        printf(WHT"%s ", pwuser->pw_name);
        printf(YEL"%*lu ", width, file_stats.st_size);
        
        if ((strcmp(colour, BLU)) == 0) {
            printf("%s%s/", colour, filename);
        } else {
            printf("%s%s", colour, filename);
        }
        printf(RESET "\n");
    }
}