#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

//La fonction du thread
void* lire_entier(void *arg) {
  int un_entier;
  int val_arg = (int) arg;
  printf("Bienvenue chez le thread ayant comme argument %d\n", val_arg);
  printf("Priere de saisir un entier:");
  scanf("%d", &un_entier);
  pthread_exit((void *) un_entier); 
}//fin lire_entier

void* afficher_octal(void *arg) {
	printf("Vous avez lu une valeur octal egale a : %o\n", (int) arg);
}

void* afficher_hex(void *arg) {
	printf("Vous avez lu une valeur hexadecimal egale a : %x\n", (int) arg);
}

int main(void) {
  clock_t time_req; 
  time_req = clock();
  pthread_t thread1;
  int i, val_retour;
  srand(time(NULL));
  i = 1 + rand() % 100;
  
  int succes = pthread_create(&thread1, NULL, lire_entier, (void *) i);
  if (succes != 0) {
    fprintf(stderr, "Erreur de creation du thread ...");
    exit(0);
  }//fin if (succes != 0)

  pthread_join(thread1, (void *)&val_retour);
  printf("Temps pour la lecture : %d\n", clock() - time_req);
  time_req = clock();
  printf("Vous avez lu une valeur entiere egale a : %d\n", val_retour);
  pthread_t thread2,thread3;
  pthread_create(&thread2, NULL, afficher_octal, (void *) val_retour);
  pthread_create(&thread3, NULL, afficher_hex, (void *) val_retour);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  printf("Temps pour l'affichage' : %d\n", clock() - time_req);
  time_req = clock();
  return 0;
}//fin main()









