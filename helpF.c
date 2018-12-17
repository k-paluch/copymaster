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

