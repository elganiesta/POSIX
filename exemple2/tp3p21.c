#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define psleep(sec) sleep ((sec))
#define INITIAL_BALANCE   200
#define INITIAL_PLAFOND   1000
#define NB_CLIENTS        5

clock_t time_req;

/* Structure stockant les informations des threads clients et de la banque */
typedef struct {
   int balance;
   int plafond;
   pthread_t thread_bank;
   pthread_t thread_clients [NB_CLIENTS];
   pthread_mutex_t mutex_balance;
   pthread_cond_t cond_balance;
   pthread_cond_t cond_clients;
} bank_t;

static bank_t bank = {
   .plafond = INITIAL_PLAFOND,
   .balance = INITIAL_BALANCE,
   .mutex_balance = PTHREAD_MUTEX_INITIALIZER,
   .cond_balance = PTHREAD_COND_INITIALIZER,
   .cond_clients = PTHREAD_COND_INITIALIZER,
};


/* Fonction pour tirer au sort un nombre entre 0 et max. */
static int get_random (int max) {
   double val;
   val = (double) max * rand ();
   val = val / (RAND_MAX + 1.0);
   return ((int) val);
} //fin get_random()


/* Fonction pour le thread de la banque afin d'alimenter le compte */
static void *fn_bank () { 
  while (1) {
      /* Debut de la zone protegee */
      pthread_mutex_lock (& bank.mutex_balance);
      pthread_cond_wait (& bank.cond_balance, & bank.mutex_balance);
      bank.balance = INITIAL_BALANCE;
      printf ("\t\t Alimentation du compte bancaire avec %d dirhams!\n", bank.balance);
      pthread_cond_signal (& bank.cond_clients);
      pthread_mutex_unlock (& bank.mutex_balance);
      /* Fin de la zone protegee */
   }//fin while
   return NULL;
} /* fin fn_bank() */

/* Fonction pour les threads des clients */
static void *fn_clients (void *p_data) {
	time_req = clock();
   int nb = (int) p_data;
   while (1) {
      int val = get_random (50);
      psleep (get_random (4));
      /* Debut de la zone protegee */
      pthread_mutex_lock (& bank.mutex_balance);
      if(bank.plafond < val) {
      	printf("\t\t impossible de prend %d dirhams, la limite de 1000dh par jour!\n", nb, val, bank.balance);
	  } else {
		if (val > bank.balance) {
			 pthread_cond_signal (& bank.cond_balance);
			 pthread_cond_wait (& bank.cond_clients, & bank.mutex_balance);
		} /* fin if */
		bank.balance = bank.balance - val;
		bank.plafond = bank.plafond - val;
      	printf("Client %d prend %d dirhams, reste %d en solde!\n", nb, val, bank.balance);
      	printf("\t\t\t\t\t temps prise par transaction :  %d\n", clock() - time_req);
      	time_req = clock();
	  }
      pthread_mutex_unlock (& bank.mutex_balance);
      /* Fin de la zone protegee */
   } /* fin while */
   return NULL;
} /* fin fn_clients() */

int main (int argc, char *args[]) {
   int i = 0;
   int ret = 0;
   /* Creation des threads */
   printf ("Creation du thread de la banque!\n");
   ret = pthread_create (& bank.thread_bank, NULL, fn_bank, NULL);
   if (!ret) {
      printf ("Creation des threads clients !\n");
      for (i = 0; i < NB_CLIENTS; i++) {
         ret = pthread_create (& bank.thread_clients [i], NULL, fn_clients, (void *) i);
         if (ret) 
            fprintf (stderr, "%s", "Erreur de creation d'un thread client");
      } /* fin for */
   } /* fin if */
   else
      fprintf (stderr, "%s", "Erreur de creation du thread banque");
   /* Attente de la fin des threads */
   i = 0;
   for (i = 0; i < NB_CLIENTS; i++) 
      pthread_join (bank.thread_clients [i], NULL);
   pthread_join (bank.thread_bank, NULL);
//	printf("Processor time taken : %f\n",clock() - time_req);
   return EXIT_SUCCESS;
} /* fin main () */

