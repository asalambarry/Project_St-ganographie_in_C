#include "head.h"

void writeSecretCode() {

    const char *filePath = "bovary.txt";
    int fd = open(filePath, O_RDWR);
    if(fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err < 0) {
        perror("fstat()");
        exit(EXIT_FAILURE);
    }

    printf("Taille du fichier: %ld bytes\n", statbuf.st_size);

    char * ptr = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }

    int secretNumber = (rand() + 8) % 16;
    char secretChr[8];
    sprintf(secretChr, "0x%dx0", secretNumber);

    int pos = rand() % (statbuf.st_size - strlen(secretChr));
    for (int i = 0; i < strlen(secretChr); i++)
    {
        ptr[pos + i] = secretChr[i];
    }

    printf("Code secret ecrit avec success!\n");

    if(close(fd) < 0) {
        perror("close()");
    }
}

int main(int argc, char *argv[]) {

    srand(getpid());

    struct sigaction action;
    int desc_requete, desc_reponse, ind, sig;
    sigset_t ens;
    sigset_t ens_vide;
    struct requete requete;

    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;

    if (mkfifo(REQUETE, mode) == -1 ){
        perror("mkfifo()");
        exit(2);
    }

    desc_requete = open(REQUETE, O_RDONLY);

    pid_t pidClient[4];
    int i = 0;

    while (TRUE) {

        printf("waiting..\n");

        if (read(desc_requete, &requete,sizeof(struct requete)) <= 0){
            close(desc_requete);
            desc_requete = open(REQUETE, O_RDONLY);
            continue;
        }

        printf("Recu %d\n", i+1);

        pidClient[i] = requete.pid_client;

        if(i == 3) {
            writeSecretCode();
            for(int j = 0; j < 4; ++j) {
                kill(pidClient[j], SIGUSR1);
            }
            printf("Signal envoyé aux 04 espions.\n");
            i = -1;
        }

        i++;
    }

    return EXIT_SUCCESS;
}
