#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <getopt.h>
#include <ctype.h>


#include "options.h"


void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);


int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);

    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov
    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie
    
    PrintCopymasterOptions(&cpm_options);
    
    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------
    
    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    
    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...
    
    //-------------------------------------------------------------------
    // Kopirovanie suborov
    //-------------------------------------------------------------------
    
    // TODO Implementovat kopirovanie suborov
    
    // cpm_options.infile
    // cpm_options.outfile
	int sz=lseek(fd,0L,SEEK_END);
    char pole[sz];
	int fd2=(cpm_options.outfile,O_WRONLY|O_TRUNC|O_APPEND|O_CREAT,0666);
	 int fd = open (cpm_options.infile, O_RDONLY);
    int buf_size = 20;
 	char buf[buf_size]; 
 	int infile_des = open(cpm_options->infile,O_RDONLY); 
 	int outfile_des = open(cpm_options->outfile,O_WRONLY);
	int r = read(infile_des,&buf, buf_size);
	int w = write(outfile_des,&buf,r);
	close(infile_des); 
	close(outfile_des);
	printf("R: %d, W: %d\n",r,w);
	if(infile_des == -1){
		int errsv = errno;
 		printf("SUBOR NEEXISTUJE \n");
		return 21;
	};
	if(errno !=0){
		printf("INA CHYBA \n");
		return 21;
	};
	
	if(cpm_options.append){
		 fd2= open (cpm_options.outfile,O_WRONLY);
                   lseek(fd2,0L,SEEK_END);
                   write(fd2,&pole,counter);
                   break;

	}
	
	if(cpm_options.overwrite){
		fd2= open (filename2,O_WRONLY|O_TRUNC);
                   lseek(fd2,0L,SEEK_SET);
                   write(fd2,&pole,counter);
                   break;

	}
	
	if(cpm_options.fast){
			lseek(fd,0L,SEEK_SET);
                   read(fd,&pole,sz);
                   write(fd2,&pole,sz);
                   break;

	}
	if(cpm_options.inode){
		struct stat     buf; 

   long int inode;
   scanf("%ld",&inode);
   

   stat(cpm_options.infile,&buf);

   printf("I-node number of file %s is: %ld ",argv[1],buf.st_ino);
   printf("Entered inode : %ld\n", inode );

   if(inode==buf.st_ino){
      fd=open(cpm_options.infile,O_RDONLY);
      fd2=open(cpm_options.outfile,O_WRONLY);

      int sz=lseek(fd,0L,SEEK_END);
      char buffer[sz];
      read(fd,&buffer,sz);
      write(fd2,&buffer,sz);
   }

     close(fd);
     close(fd2);

	}
	
	if(cpm_options.slow){
			char znak[1];
                                    
   fd = open (cpm_options.infile, O_RDONLY);

   fd2 = open (cpm_options.outfile, O_WRONLY); 

   while(fd!=EOF){
   	fscanf(fd,"%c",znak);
   	fprintf(fd2,"%c",znak);
   }       
   
   close(fd);   
   close(fd2);     

	}

    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------
    
    if (cpm_options.directory) {
        // TODO Implementovat vypis adresara
    }
        
    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------
    
    // TODO Implementovat osetrenie prepinacov po kopirovani
    
    return 0;
}


void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno); 
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}
