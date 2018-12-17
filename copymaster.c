#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <pwd.h>
#include <grp.h>

#include <time.h>

#include "options.h"

#define OPTIONS_AMOUNT 14;

int banned_for_fast[]       = {1, 6, 11};
int banned_for_slow[]       = {6, 11};
int banned_for_create[]     = {3, 11};
int banned_for_overwrite[]  = {5, 8, 10, 11};
int banned_for_append[]     = {2, 3, 5, 10, 11};
int banned_for_lseek[]      = {6, 8, 10, 11, 13};
int banned_for_directory[]  = {0, 1, 5, 7, 11};
int banned_for_delete_opt[] = {6, 11, 12};
int banned_for_chmod[]      = {11};
int banned_for_inode[]      = {11};
int banned_for_umask[]      = {3, 4, 5, 11};
int banned_for_link[]       = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13};
int banned_for_truncate[]   = {6, 7};
int banned_for_sparse[]     = {5, 6, 11};

void init_legal_options(int options_amount, int legal_options[]);
void parse_given_options(struct CopymasterOptions cpm_options, int give_options[]);
void ban_one_option(int number, int legal_options[]);
void ban_options(int options_amount, int given_options[], int legal_options[]);
void check_options(int options_amount, int given_options[], int legal_options[]);

void open_infile(int *file, struct CopymasterOptions cpm_options, char flag);
void open_outfile(int *file, struct CopymasterOptions cpm_options, char flag);

void fast_copy(struct CopymasterOptions cpm_options, char flag);
void slow_copy(struct CopymasterOptions cpm_options);

int is_regular_file(const char *path);

void FatalError(char c, const char* msg, int exit_status);

int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);
    int options_amount = OPTIONS_AMOUNT;

    int legal_options[options_amount];
    int given_options[options_amount];

    init_legal_options(options_amount, legal_options);
    parse_given_options(cpm_options, given_options);
    ban_options(options_amount,given_options, legal_options);
    check_options(options_amount, given_options, legal_options);
    
    if(argc == 3)
    {
        fast_copy(cpm_options, '?');
    }

    if(cpm_options.fast)
    {
        fast_copy(cpm_options, 'f');
    }

    if(cpm_options.slow)
    {
        slow_copy(cpm_options);
    }

    // -c 0777 (-- create 0777)
    if(cpm_options.create){
        char buffer[2];
        int temp, infile, outfile;

        open_infile(&infile, cpm_options, 'c');

        umask(0);

        if ((outfile = open(cpm_options.outfile, O_WRONLY | O_EXCL | O_CREAT, cpm_options.create_mode)) == -1)
        {
            printf("Error:\nc:%d\nc:%s\nc:%s\n", errno, strerror(errno), "SUBOR EXISTUJE");
            exit(23);
        }

        while((temp = read(infile, &buffer, 1)) > 0)
        {
            write(outfile, &buffer, temp);
        }
        close(infile);
        close(outfile);
    }

    // -o (--overwrite)
    if(cpm_options.overwrite){

        char buffer[2];
        int temp, infile, outfile;

        open_infile(&infile, cpm_options, 'o');

        if ((outfile = open(cpm_options.outfile, O_WRONLY|O_TRUNC))==-1)
        {
            printf("Error:\no:%d\no:%s\no:%s\n", errno, strerror(errno), "SUBOR NEEXISTUJE");
            exit(24);
        }


        while( (temp = read(infile, &buffer, 1)) > 0 )
        {
            write(outfile, &buffer, temp);
        }

        close(infile);
        close(outfile);
    }

    // -a (--append)
    if(cpm_options.append){
        char buffer[2];
        int temp, infile, outfile;

        open_infile(&infile, cpm_options, 'a');

        if ( (outfile = open(cpm_options.outfile, O_WRONLY | O_APPEND)) == -1)
        {
            FatalError('a', "SUBOR NEEXISTUJE", 22);
        }


        while((temp = read(infile, &buffer, 1)) > 0 ){
            write(outfile, &buffer, temp);
        }
        close(infile);
        close(outfile);
    }

    // -l (--lseek)
    if(cpm_options.lseek){
        int infile, outfile;

        open_infile(&infile, cpm_options, 'l');


        if ( (outfile = open(cpm_options.outfile, O_WRONLY) ) ==-1 ){
            printf("Error:\no:%d\no:%s\no:%s\n", errno, strerror(errno), "SUBOR NEEXISTUJE");
            exit(24);
        }

        if(lseek(infile, cpm_options.lseek_options.pos1, SEEK_SET) < 0){
            printf("Error:\nl:%d\nl:%s\nl:%s\n", errno, strerror(errno), "CHYBA POZICIE infile");
            return 33;
        }

        if(lseek(outfile, cpm_options.lseek_options.pos2, cpm_options.lseek_options.x) < 0){
            printf("Error:\nl:%d\nl:%s\nl:%s\n", errno, strerror(errno), "CHYBA POZICIE outfile");
            return 33;
        }

        char buffer[ (int) cpm_options.lseek_options.num];
        int read_bytes, written_bytes;

        if((read_bytes = read(infile, &buffer, cpm_options.lseek_options.num) ) > 0){

            printf("RET_IN: %d\n", read_bytes);

            written_bytes = write(outfile, &buffer, (size_t)read_bytes);

            printf("RET_OUT: %d\n", written_bytes);

            if(read_bytes != written_bytes){
                printf("Error:\nl:%d\nl:%s\nl:%s\n", errno, strerror(errno), "INA CHYBA");
                return 33;
            }
        }
    }

    if (cpm_options.directory) {

        DIR *dir_var;
        struct dirent *dir_struct;
        struct stat stat_struct_check_dir;
        char *file_type;

        char buffer[200];

        if(stat(cpm_options.infile, &stat_struct_check_dir) < 0){
            perror("stat()"); // no such file o directory
        }

        if (S_ISDIR(stat_struct_check_dir.st_mode)){
            file_type = "directory";
            printf("It is a directory, the file_type temp is %s\n", file_type);
        } else {
            printf("It is not a directory");
        }

        dir_var = opendir(cpm_options.infile);

        if(dir_var){
            printf("Directory exists and it is opened\n");
            while( ( dir_struct = readdir(dir_var) ) != NULL){
                if(strcmp(dir_struct->d_name, ".") && strcmp(dir_struct->d_name, "..")){
                    char filePath[100];
                    strcat(filePath, cpm_options.infile);
                    strcat(filePath, "/");
                    strcat(filePath, dir_struct->d_name);

                    struct stat stat_struct_main;

                    if(stat(filePath, &stat_struct_main) < 0){
                        perror("stat()"); // no such file o directory
                    }

                    struct passwd *pw_uid = getpwuid(stat_struct_main.st_uid);
                    struct group  *gr_gid = getgrgid(stat_struct_main.st_gid);

                    if(dir_struct->d_type == DT_DIR){
                        strcat(buffer, "d");
                    } else {
                        strcat(buffer, "-");
                    }

                    stat_struct_main.st_mode & S_IRUSR ? strcat(buffer, "r") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IWUSR ? strcat(buffer, "w") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IXUSR ? strcat(buffer, "x") : strcat(buffer, "-");

                    stat_struct_main.st_mode & S_IRGRP ? strcat(buffer, "r") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IWGRP ? strcat(buffer, "w") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IXGRP ? strcat(buffer, "x") : strcat(buffer, "-");

                    stat_struct_main.st_mode & S_IROTH ? strcat(buffer, "r") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IWOTH ? strcat(buffer, "w") : strcat(buffer, "-");
                    stat_struct_main.st_mode & S_IXOTH ? strcat(buffer, "x") : strcat(buffer, "-");

                    strcat(buffer, " ");

                    char tempStr[10];
                    sprintf(tempStr, "%ld", (long)stat_struct_main.st_nlink);
                    strcat(buffer, tempStr);
                    tempStr[0] = 0;

                    strcat(buffer, " ");

                    if(pw_uid == NULL) {
                        perror("getpwuid");
                    } else {
                        strcat(buffer, pw_uid->pw_name);
                    }

                    strcat(buffer, " ");

                    if(gr_gid == NULL) {
                        perror("getgrgid");
                    } else {
                        strcat(buffer, gr_gid->gr_name);
                    }

                    strcat(buffer, " ");

                    sprintf(tempStr, "%d", (int)stat_struct_main.st_size);
                    strcat(buffer, tempStr);
                    tempStr[0] = 0;
                    strcat(buffer, "\t");

                    char timeString[32];
                    strftime(timeString, 32, "%b %e %H:%M", localtime(&stat_struct_main.st_mtime));
                    strcat(buffer, timeString);

                    strcat(buffer, " ");

                    strcat(buffer, dir_struct->d_name);

                    printf("%s\n", buffer);

                    buffer[0] = 0;
                    filePath[0] = 0;
                }
            }
            closedir(dir_var);
        } else {
            printf("Directory does not exist\n");
            closedir(dir_var);
        }
    }

    if (cpm_options.delete_opt) {
        fast_copy(cpm_options, 'd');

        if (is_regular_file(cpm_options.infile)) {
            unlink(cpm_options.infile);
        } else {
            FatalError('D', "SUBOR NEBOL ZMAZANY", 26);
        }
    }

    if (cpm_options.chmod) {
        fast_copy(cpm_options, 'm');

        int chmod_code = chmod(cpm_options.infile, cpm_options.chmod_mode);

        if (chmod_code < 0) {
            if (chmod_code == EINVAL) {
                printf("ZLE PRAVA");
            } else {
                printf("INA CHYBA");
            }
        }
    }

    if (cpm_options.truncate) {
        fast_copy(cpm_options, 't');
        int truncate_code = truncate(cpm_options.infile, cpm_options.truncate_size);
        if (truncate_code == -1)
        {
            FatalError('t', "VSTUPNY SUBOR NEZMENENY", 31);
        }
    }
    exit(0);
}

void parse_given_options(struct CopymasterOptions cpm_options, int give_options[])
{
    give_options[0] = cpm_options.fast ? 1 : 0;
    give_options[1] = cpm_options.slow ? 1 : 0;
    give_options[2] = cpm_options.create ? 1 : 0;
    give_options[3] = cpm_options.overwrite ? 1 : 0;
    give_options[4] = cpm_options.append ? 1 : 0;
    give_options[5] = cpm_options.lseek ? 1 : 0;
    give_options[6] = cpm_options.directory ? 1 : 0;
    give_options[7] = cpm_options.delete_opt ? 1 : 0;
    give_options[8] = cpm_options.chmod ? 1 : 0;
    give_options[9] = cpm_options.inode ? 1 : 0;
    give_options[10] = cpm_options.umask ? 1 : 0;
    give_options[11] = cpm_options.link ? 1 : 0;
    give_options[12] = cpm_options.truncate ? 1 : 0;
    give_options[13] = cpm_options.sparse ? 1 : 0;
}

void ban_one_option(int number, int legal_options[])
{
    switch (number)
    {
        case 0:
            for (int i = 0; i < sizeof(banned_for_fast)/sizeof(int); i++) {
                legal_options[banned_for_fast[i]] = 0;
            }
            break;
        case 1:
            for (int i = 0; i < sizeof(banned_for_slow)/sizeof(int); i++) {
                legal_options[banned_for_slow[i]] = 0;
            }
            break;
        case 2:
            for (int i = 0; i < sizeof(banned_for_create)/sizeof(int); i++) {
                legal_options[banned_for_create[i]] = 0;
            }
            break;
        case 3:
            for (int i = 0; i < sizeof(banned_for_overwrite)/sizeof(int); i++) {
                legal_options[banned_for_overwrite[i]] = 0;
            }
            break;
        case 4:
            for (int i = 0; i < sizeof(banned_for_append)/sizeof(int); i++) {
                legal_options[banned_for_append[i]] = 0;
            }
            break;
        case 5:
            for (int i = 0; i < sizeof(banned_for_lseek)/sizeof(int); i++) {
                legal_options[banned_for_lseek[i]] = 0;
            }
            break;
        case 6:
            for (int i = 0; i < sizeof(banned_for_directory)/sizeof(int); i++) {
                legal_options[banned_for_directory[i]] = 0;
            }
            break;
        case 7:
            for (int i = 0; i < sizeof(banned_for_delete_opt)/sizeof(int); i++) {
                legal_options[banned_for_delete_opt[i]] = 0;
            }
            break;
        case 8:
            for (int i = 0; i < sizeof(banned_for_chmod)/sizeof(int); i++) {
                legal_options[banned_for_chmod[i]] = 0;
            }
            break;
        case 9:
            for (int i = 0; i < sizeof(banned_for_inode)/sizeof(int); i++) {
                legal_options[banned_for_inode[i]] = 0;
            }
            break;
        case 10:
            for (int i = 0; i < sizeof(banned_for_umask)/sizeof(int); i++) {
                legal_options[banned_for_umask[i]] = 0;
            }
            break;
        case 11:
            for (int i = 0; i < sizeof(banned_for_link)/sizeof(int); i++) {
                legal_options[banned_for_link[i]] = 0;
            }
            break;
        case 12:
            for (int i = 0; i < sizeof(banned_for_truncate)/sizeof(int); i++) {
                legal_options[banned_for_truncate[i]] = 0;
            }
            break;
        case 13:
            for (int i = 0; i < sizeof(banned_for_sparse)/sizeof(int); i++) {
                legal_options[banned_for_sparse[i]] = 0;
            }
            break;
        default:
            break;
    }
}

void ban_options(int options_amount, int given_options[], int legal_options[])
{
    for (int i = 0; i < options_amount; i++)
    {
        if (given_options[i] == 1)
        {
            ban_one_option(i, legal_options);
        }
    }
}

void check_options(int options_amount, int given_options[], int legal_options[])
{
    for (int i = 0; i < options_amount; i++)
    {
        if (given_options[i] == 1)
        {
            if (legal_options[i] == 0)
            {
                fprintf(stderr, "CHYBA PREPINACOV\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void open_infile(int *file, struct CopymasterOptions cpm_options, char flag)
{
    if((*file = open(cpm_options.infile, O_RDONLY)) == -1)
    {
        FatalError('f', "infile", 21);
    }
}

void open_outfile(int *file, struct CopymasterOptions cpm_options, char flag)
{
    if( (*file = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1)
    {
        FatalError(flag, "infile", 21);
    }
}

void fast_copy(struct CopymasterOptions cpm_options, char flag)
{
    long buffer_length;
    int infile, outfile, temp;

    open_infile(&infile, cpm_options, 'f');
    open_outfile(&outfile, cpm_options, 'f');

    buffer_length = lseek(infile, 0, SEEK_END);
    char buffer[buffer_length];

    lseek(infile, 0, SEEK_SET);

    if((temp = read(infile, &buffer, buffer_length)) > 0)
    {
        write(outfile, &buffer, temp);
    }

    close(infile);
    close(outfile);
}

void slow_copy(struct CopymasterOptions cpm_options)
{
    char buffer[2];
    int infile, outfile, temp;

    open_infile(&infile, cpm_options, 's');
    open_outfile(&outfile, cpm_options, 's');

    while((temp = read(infile, &buffer, 1)) > 0)
    {
        write(outfile, &buffer, temp);
    }

    close(infile);
    close(outfile);
}

void init_legal_options(int options_amount, int legal_options[])
{
    for (int i=0; i < options_amount; i++)
    {
        legal_options[i] = 1;
    }
}

void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno);
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

