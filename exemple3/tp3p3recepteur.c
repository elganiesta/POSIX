#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc,char *argv[]) {
	mqd_t mq;
	int taille;
	char *buffer;
	struct mq_attr attr;
	struct timeval heure;
	struct timeval *recue;
	int nb_messages;
	long int duree;
	long int duree_max;
	long int duree_min;
	long int somme_durees;

	if (argc != 2) {
		fprintf(stderr, "usage: %s nom_file_message\n", argv[0]);
		exit(EXIT_FAILURE);
	}/* fin if */
	mq = mq_open(argv[1], O_RDONLY | O_CREAT, 0600, NULL);
	if (mq == (mqd_t) -1) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}/* fin if */
	if (mq_getattr(mq, & attr) != 0) {
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}/* fin if */
	taille = attr.mq_msgsize;
	buffer = malloc(taille);
	if (buffer == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}/* fin if */
	recue = (struct timeval *) buffer;
	while (1) {
		nb_messages = 0;
		duree_max = 0;
		duree_min = -1;
		somme_durees = 0;
		do {
			mq_receive(mq, buffer, taille, NULL);
			gettimeofday(& heure, NULL);
			duree  = heure.tv_sec - recue->tv_sec;
			duree *= 1000000;
			duree += heure.tv_usec - recue->tv_usec;
			if (nb_messages > 0) { // Ignorer le premier message (retarde)
				if (duree_max < duree)
					duree_max = duree;
				if ((duree_min == -1) || (duree_min > duree))
					duree_min = duree;
				somme_durees += duree;
			}/* fin if */
			nb_messages ++;
		} while (nb_messages < 100000); // arbitraire, de l'ordre de la seconde
		fprintf(stdout, "min =%3ld   max =%3ld moy=%5.1f\n",duree_min, duree_max, ((float) somme_durees) / (nb_messages - 1));
	}/* fin while */
	return EXIT_SUCCESS;
}/* fin main() */
