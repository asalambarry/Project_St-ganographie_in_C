#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

void showSecret(const char *ptr) {
    char needle[strlen(ptr) + 1];
    strcpy(needle, ptr);
	char *f1;
	char *f2;
	int index1 = -1, index2 = -1;
	f1 = strstr(needle, "0x");
	f2 = strstr(needle, "x0");
	if(f1 != NULL) {
		index1 = (int)(f1 - needle);
	}

	if(f2 != NULL) {
		index2 = (int)(f2 - needle);
	}

	index1 += 2;

	if(index1 > -1 && index2 > -1 && index2 > index1) {
		int length = index2 - index1;
		char secretStr[length];
		for (int i = index1, j = 0; i < index2; i++, ++j)
		{
			secretStr[j] = needle[i];
		}
		printf("Processus %d, Nombre Secret: %s\n", getpid(), secretStr);
	}
}

int main(int argc, char *argv[]) {

    srand(time(NULL));
    pid_t wpid;
    int status = 0;

    if(argc < 2) {
        perror("Argument incorrect!");
        exit(EXIT_FAILURE);
    }

    // 2.
    const char *filePath = "bovary.txt";
    int fd = open(filePath, O_RDWR);
    if(fd < 0) {
        perror("Erreur fichier!");
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err < 0) {
        perror("Erreur buffer!");
        exit(EXIT_FAILURE);
    }

    printf("Taille du fichier: %ld bytes\n", statbuf.st_size);   

    char * ptr = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("Echec Mapping!");
        exit(EXIT_FAILURE);
    }

    int secretNumber = (rand() + 8) % 16;
    char secretChr[8];
    sprintf(secretChr, "0x%dx0", secretNumber);
    // printf("secret: %s\n", secretChr);

    int pos = rand() % (statbuf.st_size - strlen(secretChr));
    for (int i = 0; i < strlen(secretChr); i++)
    {
        ptr[pos + i] = secretChr[i];
    }

    // 3.
    int N = 0;
    sscanf(argv[1], "%d", &N);

    for (int i = 0; i < N; i++)
    {
        
        int pid = fork();
        if(pid == 0) {
            showSecret(ptr);
            exit(EXIT_SUCCESS);
        }
    }

    while ((wpid = wait(&status)) > 0);

    if(close(fd) < 0) {
        perror("Echec close!");
    }

    return EXIT_SUCCESS;
}
