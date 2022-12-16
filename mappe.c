#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    pid_t wpid;
    int status = 0;
    int myPid = getpid();

    if(argc < 3) {
        perror("Argument incorrect!");
        exit(EXIT_FAILURE);
    }

    const char *filePath = argv[1];
    int fd = open(filePath, O_RDWR);
    if(fd < 0) {
        perror("Erreur fichier!");
        exit(EXIT_FAILURE);
    }

    long extendLength = sizeof(&myPid) * 8;
    if(ftruncate(fd, extendLength) < 0) {
        perror("ftruncate");
    }

    char * ptr = mmap(NULL, extendLength, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("Echec Mapping!");
        exit(EXIT_FAILURE);
    }

    // ---
    sprintf(ptr, "%d", myPid);
    if(close(fd) < 0) {
        perror("Echec close!");
    }

    int err = munmap(ptr, extendLength);
    if(err < 0) {
        perror("Echec unmapping!");
    }

    int N = 0;
    sscanf(argv[2], "%d", &N);
    for (int i = 0; i < N; i++)
    {
        int pid = fork();
        if(pid == 0) {
           printf("Parent PID: %d\n", getppid());
           exit(EXIT_SUCCESS);
        }
    }

    while ((wpid = wait(&status)) > 0);

    return EXIT_SUCCESS;
}