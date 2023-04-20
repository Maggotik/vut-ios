#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h> 

#define sem_name(name) "xvanoj00_ios_sem-" #name

#define freeSem(name) do { \
    l##name: \
    sem_close(name); \
    sem_unlink(sem_name(#name)); \
} while(0) \

#define create_sem(name,starting_value) do { \
    (name) = sem_open(sem_name(#name), O_CREAT | O_EXCL, 0666, (starting_value)); \
if ((name) == NULL) { ret = 1; goto l##name; }\
} while (0)

#define writeSync(file, fmt, ...) do { \
    sem_wait(semWrite); \
    fprintf(file, "%-5d: "fmt, (*A)++,##__VA_ARGS__); \
    fflush(file); \
    sem_post(semWrite); \
} while(0)

#define mymap(pointer) \
        (pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)


#define unmap(pointer) \
        munmap((pointer), sizeof((pointer)))

#define msleep(time) do { \
    if ((time) != 0) \
    usleep((time) * 1000); \
} while(0)

int parse(char* arg);
int processSanta(FILE *file, int reinDeersCount);
int processElf(FILE *file, int ID, int worktime);
int processReinDeer(FILE *file, int ID, int holidayTime);

sem_t* semWorkshop = NULL;
sem_t* semWrite = NULL;
sem_t* semMain = NULL;
sem_t* semSanta = NULL;
sem_t* semSleigh = NULL;
sem_t* semSantaHelps = NULL;

int* A = NULL;
int* waitingElfsCount = NULL;
int* workshopElfsCount = NULL;
int* workshopOpened = NULL;
int* reinDeersOnHoliday = NULL;