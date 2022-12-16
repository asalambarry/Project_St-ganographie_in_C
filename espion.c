#include "head.h"

void showSecret() {

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

    char * ptr = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }

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

    if(close(fd) < 0) {
        perror("close()");
    }
}

void handReveil() { 
    showSecret();
}

int main(int argc, char *argv[]) {    

    struct sigaction action;
    int desc_requete;

    sigset_t ens;
    sigset_t ens_vide;
    struct requete requete;
    desc_requete = open(REQUETE, O_WRONLY);

    if (desc_requete == -1 ) {
        perror("open()");
        exit(2);
    }

    action.sa_handler = handReveil;
    sigaction(SIGUSR1, &action, NULL);
    sigemptyset(&ens_vide); 
    sigemptyset(&ens);
    sigaddset(&ens, SIGUSR1);

    requete.pid_client = getpid();
    sigprocmask (SIG_SETMASK, &ens, NULL);

    if (write(desc_requete, &requete, sizeof(struct requete)) == -1) {
        perror("write");
        exit(2);
    }
    printf("pid envoyé!\n");
    sigsuspend(&ens_vide);

    return EXIT_SUCCESS;
}
