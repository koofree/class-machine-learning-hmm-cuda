#include <stdio.h>
#include <stdlib.h>

#include "hmm.h"

void print_hmm(Hmm *hmm)
{
	int j;
	int k;
	int n_st = hmm->nstates;
	int n_sy = hmm->nsymbols;
	
	printf("# state transition probability (A)\n");
	for (j = 0; j < n_st; j++) {
		for (k = 0; k < n_sy; k++) {
			printf(" %.4f", hmm->a[(j * n_st) + k]);
		}
		printf("\n");
	}
	
	printf("# state ouput probability (B)\n");
	for (j = 0; j < n_sy; j++) {
		for (k = 0; k < n_st; k++){
			printf(" %.4f", hmm->b[(j * n_st) + k]);
		}
		printf("\n");
	}
	
	printf("# initial state probability (Pi)\n");
	for (j = 0; j < n_st; j++) {
		printf(" %.4f", hmm->pi[j]);
	}
	printf("\n\n");
}

void print_obs(Obs *obs)
{
	int j;
	printf("Oservation sequence:\n");
	for (j = 0; j < obs->length; j++) {
		printf(" %i", obs->data[j]);
	}
	printf("\n\n");
}

void free_vars(Hmm *hmm, Obs *obs)
{
	if (hmm != NULL) {
		if (hmm->a != NULL) {
			free(hmm->a);
		}
		if (hmm->b != NULL) {
			free(hmm->b);
		}
		if (hmm->pi != NULL) {
			free(hmm->pi);
		}
		free(hmm);
	}
	if (obs != NULL) {
		if (obs->data != NULL) {
			free(obs->data);
		}
		free(obs);
	}
}
