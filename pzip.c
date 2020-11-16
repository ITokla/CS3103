/*
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your group information here.
 *
 * Group No.:  (Join a project group in Canvas)
 * First member's full name: Ryan Chan
 * First member's email address: 
 * Second member's full name: Andrew Low
 * Second member's email address: 
 * Third member's full name: Lam Chi Ting
 * Third member's email address: chitlam7-c@my.cityu.edu.hk
 */
/*  Work Progress
//Lam Ting 10/26 mmap worked
//Lam Ting 11/08 Producer get the mmap text and able to print it
*/
//One Producer produce the String from inputfile to give multiple Consumer
//if Producer get n+1 char is different with n 
//it will start give the char to another Consumer(2)
//Consumer (1) start count the result output to the stdout
//Consumer (1) will wait until next round.

// it will have multiple lock 
//One Lock will lock the given char to Consumer
//One Lock will lock the writing to stdout
//Only One Consumer take the char from Producer
//Only One Consumer output the result to stdout




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
//Global ///
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t smutex = PTHREAD_MUTEX_INITIALIZER;
int input_file_number;
int file_open;

//struct
struct buffer{
    char *str;
    int textcount;
};

void *compress(void* buf){
    struct buffer* input = (struct buffer*)buf;
    char compressC;
    int numOfc = 1;
    int loop = 0;
    compressC = input->str[0];
    pthread_mutex_lock(&mutex);
    while(loop<input->textcount){
        if(compressC==input->str[loop+1]){
            loop++;
            numOfc++;
        }else{
            fwrite(&numOfc,1,sizeof(int),stdout);
            fwrite(&compressC,1,sizeof(char),stdout);
            numOfc = 1;
            compressC = input->str[loop+1];
            loop++;

        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);//leave thread
}



int main(int argc, char** argv){   
    file_open = 0;
    //pthread_mutex_t lock; it is useless now until we decide what is out critical section
    if(argc<2){
        printf("pzip: file1 [file2 ...]\n");
        exit(1);
    }else{
        input_file_number = argc-1;//number of file
        pthread_t tid[argc-1];//thread number
        struct stat statbuf[input_file_number];//file info
        struct buffer getinput[input_file_number];//buffer for thread
        for(int i=0;i<input_file_number;i++){
            file_open = open(argv[i+1] , O_RDONLY);
            fstat (file_open,&statbuf[i]);
            getinput[i].textcount = statbuf[i].st_size;
            if(getinput[i].textcount<=0){
                return 0;
            }
            if ((getinput[i].str = mmap (NULL, getinput[i].textcount, PROT_READ, MAP_SHARED, file_open, 0)) == (caddr_t) -1){//check mmap successful
                    printf ("mmap error for input");
                    return 0;
            }
        }
        for (int i = 0; i < input_file_number; i++){
            pthread_create(&tid[i-1],NULL,compress,(void*) &getinput[i]);
        }

        for (int i = 0; i < input_file_number; i++){
            pthread_join(tid[i-1],NULL);
        }
        /*
        //print mmap value       
        for(int i=0;i<statbuf.st_size;i++){
            printf ("%c",src[i]); 
        }

    for(int i=0;i<input->textcount;i++){
        printf ("%c",input->str[i]); 
        buffer = input->str[i];
        if(buffer==compressC){
            numOfc++;
        }else{
            fwrite(&numOfc,1,sizeof(int),stdout);
            fwrite(&compressC,1,sizeof(char),stdout);
            i = numOfc;
            numOfc = 1;
            compressC = buffer;
        }
    }



        */
        return 0;
    }
}