#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc,char * argv[]) {
	mqd_t mq;
	struct timeval heure;

	if (argc != 2) {
		fprintf(stderr, "usage: %s nom_file_message\n", argv[0]);
		exit(EXIT_FAILURE);
	}/* fin if */

	mq = mq_open(argv[1], O_WRONLY | O_CREAT, 0600, NULL);
	if (mq == (mqd_t) -1) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}/* fin if */
	while (1) {
		gettimeofday(& heure, NULL);
		mq_send(mq, (char *) & heure, sizeof(heure), 1);
	}/* fin while */
	return EXIT_SUCCESS;
}/* fin main() */
