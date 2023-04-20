#include "proj2.h"

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 5) {
        printf("Wrong number of arguments\n");
        return 1;
    }
    unsigned int elfCount = parse(argv[1]);
    unsigned int reinDeerCount = parse(argv[2]);
    int elfSelfTime = parse(argv[3]);
    int reinDeerHolidayTime = parse(argv[4]);

    if(!(0 < elfCount && elfCount < 1000)) {
        printf("Wrong argument\n");
        return 1;
    }
    if(!(0 < reinDeerCount && reinDeerCount < 20)) {
        printf("Wrong argument\n");
        return 1;
    }
    if(!(0 <= elfSelfTime && elfSelfTime <= 1000)) {
        printf("Wrong argument\n");
        return 1;
    }
    if(!(0 <= reinDeerHolidayTime && reinDeerHolidayTime <= 1000)) {
        printf("Wrong argument\n");
        return 1;
    }

    create_sem(semSleigh, 0);
    create_sem(semSanta, 0);
    create_sem(semWorkshop, 0);
    create_sem(semMain, 0);
    create_sem(semWrite, 1);
    create_sem(semSantaHelps, 0);

    mymap(A);
    mymap(waitingElfsCount);
    mymap(workshopElfsCount);
    mymap(workshopOpened);
    mymap(reinDeersOnHoliday);


    *reinDeersOnHoliday = reinDeerCount;
    *workshopOpened = 1;
    *waitingElfsCount = 0;
    *workshopElfsCount = 1;
    *A = 1;

    FILE *file = fopen("proj2.out", "w");
    if (fork() == 0)
        processSanta(file, reinDeerCount);

    for (unsigned i = 1; i <= elfCount;i++) {
        if (fork() == 0)
            processElf(file, i, elfSelfTime);
    }

    for (unsigned i = 1; i <= reinDeerCount;i++) {
        if (fork() == 0)
            processReinDeer(file, i, reinDeerHolidayTime);
    }

    int semMainWaitCount = elfCount + reinDeerCount + 1;

    for (int i = 0; i < semMainWaitCount;i++)
        sem_wait(semMain);

    fprintf(file, "END\n");
    fclose(file);

    freeSem(semSantaHelps);
    freeSem(semWrite);
    freeSem(semMain);
    freeSem(semWorkshop);
    freeSem(semSanta);
    freeSem(semSleigh);
    return ret;
}

int processSanta(FILE *file, int reinDeerCount) {
    while (*reinDeersOnHoliday) {
        writeSync(file, "Santa  %s: going to sleep\n", "");
        sem_wait(semSanta);
        if (*reinDeersOnHoliday) {
            writeSync(file, "Santa  %s: helping elves\n", "");
            sem_post(semWorkshop);
            sem_post(semWorkshop);
            sem_post(semWorkshop);
            sem_post(semSantaHelps);
            sem_wait(semSanta);
            sem_post(semSantaHelps);
            sem_wait(semSanta);
            sem_post(semSantaHelps);
            sem_wait(semSanta);
        }
        else {
            *workshopOpened = 0;
            writeSync(file, "Santa  %s: closing workshop\n", "");
            sem_post(semWorkshop);
        }
    }

    for (int i = 0; i < reinDeerCount; i++)
    {
        sem_post(semSleigh);
        sem_wait(semSanta);
    }
    writeSync(file, "Santa  %s: Christmas started\n", "");
    sem_post(semMain);
    exit(0);
}

int processElf(FILE *file, int ID, int worktime) {
    msleep(rand() % worktime);
    writeSync(file, "Elf %3d: started\n", ID);
    msleep(rand() % worktime);
    ++ *waitingElfsCount;
    writeSync(file, "Elf %3d: need help\n", ID);
    while (*workshopOpened) {
        if (* waitingElfsCount >= 3 && -- *workshopElfsCount == 0)
        {
            sem_post(semSanta);
        }
        sem_wait(semWorkshop);
        -- *waitingElfsCount;
        if (!*workshopOpened)
            break;
        ++ *workshopElfsCount;
        sem_wait(semSantaHelps);
        writeSync(file, "Elf %3d: get help\n", ID);
        sem_post(semSanta);
        msleep(rand() % worktime);
        ++ *waitingElfsCount;
        writeSync(file, "Elf %3d: need help\n", ID);
    }

    writeSync(file, "Elf %3d: taking holidays\n", ID);
    sem_post(semWorkshop);
    sem_post(semMain);
    exit(0);
}

int processReinDeer(FILE *file, int ID, int holidayTime){
    msleep(rand() % holidayTime);
    writeSync(file, "RD %3d: rstarted\n", ID);

    msleep((holidayTime + rand() % holidayTime) / 2);
    writeSync(file, "RD %3d: return home\n", ID);
    
    if (!-- * reinDeersOnHoliday)
        sem_post(semSanta);
    
    sem_wait(semSleigh);
    writeSync(file, "RD %3d: get hitched\n", ID);
    sem_post(semSanta);
    sem_post(semMain);
    exit(0);
}

int parse(char *arg) {
    char *buff = NULL;
    int result = (int)strtol(arg, &buff, 10);
    if (*buff != 0) {
        fprintf(stderr, "ERROR: invalid argument\n");
        exit(1);
    }
    return result;
}