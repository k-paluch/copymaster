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
#include <math.h>
#include "options.h"



int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}



void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno);
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}


int main(int argc, char* argv[])
{
	struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);
	//KONTROLA PREPINACOV
	if(cpm_options.directory && (cpm_options.lseek + cpm_options.delete_opt + cpm_options.link + cpm_options.truncate)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if(cpm_options.link && (cpm_options.append + cpm_options.create + cpm_options.directory + cpm_options.sparse + cpm_options.lseek + cpm_options.overwrite)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if(cpm_options.sparse && cpm_options.link){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if(cpm_options.overwrite && (cpm_options.append + cpm_options.create + cpm_options.link)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
	if(cpm_options.create && cpm_options.delete_opt){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if(cpm_options.append && (cpm_options.create + cpm_options.overwrite + cpm_options.link)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	 if(cpm_options.lseek && (cpm_options.append + cpm_options.directory + cpm_options.link)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);	
	}
	if(cpm_options.create && (cpm_options.append + cpm_options.overwrite + cpm_options.link)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}	
	if(cpm_options.truncate && (cpm_options.directory + cpm_options.delete_opt)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	if(cpm_options.delete_opt && (cpm_options.directory + cpm_options.truncate)){
		fprintf(stderr, "CHYBA PREPINACOV\n");
		exit(EXIT_FAILURE);
	}
	//KONTROLA PARAMETROV
    if(argc == 3){
	long sizeofbuffer;
    int infile, outfile, temp;
	//INFILE,OURFILE CHECK
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('?', "infile", 21);
    	}
	if( (outfile = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1){ 
        	FatalError('?', "outfile", 21);
    	}
    	sizeofbuffer = lseek(infile, 0, SEEK_END);
    	char buffer[sizeofbuffer];
    	lseek(infile, 0, SEEK_SET);
    	if((temp = read(infile, &buffer, sizeofbuffer)) > 0){
        	write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
    }

    
	//SLOW
    if(cpm_options.slow)
    {
        char buffer[2];
    	int infile, outfile, temp;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('s', "infile", 21);
    	}
	if( (outfile = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1){ 
        	FatalError('s', "outfile", 21);
    	}
    	while((temp = read(infile, &buffer, 1)) > 0){
        	write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
    }
	//FAST
	if(cpm_options.fast){
	long sizeofbuffer;
    	int infile, outfile, temp;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('f', "infile", 21);
    	}
	if( (outfile = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1){ 
        	FatalError('f', "outfile", 21);
    	}
    	sizeofbuffer = lseek(infile, 0, SEEK_END);
    	char buffer[sizeofbuffer];
    	lseek(infile, 0, SEEK_SET);
    	if((temp = read(infile, &buffer, sizeofbuffer)) > 0){
        	write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
    }
	//LINK
    if(cpm_options.link){
	    int infile,outfile;
	    if((infile = open(cpm_options.infile,O_RDONLY)) ==-1){
		FatalError('K',"infile",30);
	    }
	    if((outfile = open(cpm_options.outfile,O_RDONLY)) == -1){
		FatalError('K',"outfile",30);
	    }
	    link(cpm_options.infile,cpm_options.outfile);
	    close(infile);
	    close(outfile);
    }
	//LSEEK
if(cpm_options.lseek){
        int infile, outfile;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1)
    	{
        	FatalError('l', "infile", 21);
    	}
        if ( (outfile = open(cpm_options.outfile, O_WRONLY) ) ==-1 ){
			FatalError('l',"SUBOR NEEXISTUJE",24);
        }
        if(lseek(infile, cpm_options.lseek_options.pos1, SEEK_SET) < 0){
			FatalError('l',"CHYBA POZICIE infile",33);
        }
        if(lseek(outfile, cpm_options.lseek_options.pos2, cpm_options.lseek_options.x) < 0){
			FatalError('l',"CHYBA POZICIE outfile",33);
        }
        char buffer[ (int) cpm_options.lseek_options.num];
        int read_bytes, written_bytes;
        if((read_bytes = read(infile, &buffer, cpm_options.lseek_options.num) ) > 0){

            printf("RET_IN: %d\n", read_bytes);
            written_bytes = write(outfile, &buffer, (size_t)read_bytes);
            printf("RET_OUT: %d\n", written_bytes);
            if(read_bytes != written_bytes){
				FatalError('l',"INA CHYBA",33);
            }
        }
    }
	//CREATE
    if(cpm_options.create){
        char buffer[2];
        int temp, infile, outfile;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('c', "infile", 21);
    	}
        umask(0);
        if ((outfile = open(cpm_options.outfile, O_WRONLY | O_EXCL | O_CREAT, cpm_options.create_mode)) == -1){
			FatalError('c',"SUBOR NEEXISTUJE",23);
        }
        while((temp = read(infile, &buffer, 1)) > 0)
        {
            write(outfile, &buffer, temp);
        }
        close(infile);
        close(outfile);
    }

    //TRUNCATE
if (cpm_options.truncate) {
	long sizeofbuffer;
    	int infile, outfile, temp;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1)
    	{
        	FatalError('t', "infile", 21);
    	}
	if( (outfile=open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1)
    	{ 
        	FatalError('t', "infile", 21);
    	}
    	sizeofbuffer = lseek(infile, 0, SEEK_END);
    	char buffer[sizeofbuffer];
    	lseek(infile, 0, SEEK_SET);
    	if((temp = read(infile, &buffer, sizeofbuffer)) > 0)
    	{
        	write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
        int truncate_code = truncate(cpm_options.infile, cpm_options.truncate_size);
        if (truncate_code == -1)
        {
            FatalError('t', "VSTUPNY SUBOR NEZMENENY", 31);
        }
    }
	//APPEND
    if(cpm_options.append){
        char buffer[2];
        int temp, infile, outfile;

	if((infile = open(cpm_options.infile, O_RDONLY)) == -1)
    	{
        	FatalError('a', "infile", 21);
    	}

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
	//OVERWRITE
    
	if(cpm_options.overwrite){
        	char buffer[2];
        	int temp, infile, outfile;
		if((infile = open(cpm_options.infile, O_RDONLY)) == -1)
    		{
        	FatalError('o', "infile", 21);
    		}
        	if ((outfile = open(cpm_options.outfile, O_WRONLY|O_TRUNC))==-1)
        	{
				FatalError('o',"SUBOR NEEXISTUJE",24);
        	}
        	while( (temp = read(infile, &buffer, 1)) > 0 )
        	{	
            		write(outfile, &buffer, temp);
        	}
        	close(infile);
        	close(outfile);
    }
	//DELETE
    if (cpm_options.delete_opt) {
	long sizeofbuffer;
    	int infile, outfile, temp;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('d', "infile", 21);
    	}
	if( (outfile = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1){ 
        	FatalError('d', "infile", 21);
    	}
    	sizeofbuffer = lseek(infile, 0, SEEK_END);
    	char buffer[sizeofbuffer];
    	lseek(infile, 0, SEEK_SET);
    	if((temp = read(infile, &buffer, sizeofbuffer)) > 0)
    	{
       		write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
        if (is_regular_file(cpm_options.infile)) {
            unlink(cpm_options.infile);
        } else {
            FatalError('D', "SUBOR NEBOL ZMAZANY", 26);
        }
    }
	//CHMOD
    if (cpm_options.chmod) {
	long sizeofbuffer;
    	int infile, outfile, temp;
	if((infile = open(cpm_options.infile, O_RDONLY)) == -1){
        	FatalError('m', "infile", 21);
    	}
	if( (outfile= open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644) ) == -1)
    	{ 
        	FatalError('m', "infile", 21);
    	}
    	sizeofbuffer = lseek(infile, 0, SEEK_END);
    	char buffer[sizeofbuffer];
    	lseek(infile, 0, SEEK_SET);
    	if((temp = read(infile, &buffer, sizeofbuffer)) > 0){
        	write(outfile, &buffer, temp);
    	}
    	close(infile);
    	close(outfile);
        int chmod_code = chmod(cpm_options.infile, cpm_options.chmod_mode);
        if (chmod_code < 0) {
            if (chmod_code == EINVAL) {
                printf("ZLE PRAVA");
            } else {
                printf("INA CHYBA");
            }
        }
    }

    //DIRECTORY
	if (cpm_options.directory) {
		DIR *dir_var;
		struct dirent *dir_struct;
		struct stat stat_struct_check_dir;
		char *file_type;
		char buffer[200];
		if(stat(cpm_options.infile, &stat_struct_check_dir) < 0){
		    perror("stat()"); 
		}
		if (S_ISDIR(stat_struct_check_dir.st_mode)){
		    file_type = "directory";
		    printf("Je to Dir, the file_type temp is %s\n", file_type);
		} else {
		    printf("Nieje to Dir");
		}
		dir_var = opendir(cpm_options.infile);
		if(dir_var){
		    printf("Dir existuje a je otvoreny\n");
		    while( ( dir_struct = readdir(dir_var) ) != NULL){
				//get rid of "." and ".."
		        if(strcmp(dir_struct->d_name, ".")!=0 && strcmp(dir_struct->d_name, "..")!=0){
		            char filePath[100];
		            strcat(filePath, cpm_options.infile);
		            strcat(filePath, "/");
		            strcat(filePath, dir_struct->d_name);
		            struct stat stat_struct_main;
		            if(stat(filePath, &stat_struct_main) < 0){
		                perror("stat()"); 
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
		//umask
	if (cpm_options.umask){
		mode_t mask = 0;
	int k = 0, per = 0;

	for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options.umask_options[i][0] == 0) {
            break;
        }
		
		if (cpm_options.umask_options[i][1] == '-'){
			switch(cpm_options.umask_options[i][0]){
				case 'u': k=0; break;
				case 'g': k=1; break;
				case 'o': k=2; break;
			}

			switch(cpm_options.umask_options[i][2]){
				case 'r': per=0; break;
				case 'w': per=1; break;
				case 'x': per=2; break;
			}
		}

		mask += pow(2, 8 - (k * 3 + per));
    }
	umask(mask);
	}
	
}
