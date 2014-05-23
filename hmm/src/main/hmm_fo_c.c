#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hmm.h"

float run_hmm_fo(Hmm *in_hmm, Obs *in_obs)
{
	int j;
	int k;
	int t;
	float log_lik;
	float *alpha;
	float *scale;
	float *a = in_hmm->a;
	float *b = in_hmm->b;
	float *pi = in_hmm->pi;
	int nstates = in_hmm->nstates;
	int *obs = in_obs->data;
	int length = in_obs->length;

	alpha = (float *) malloc(sizeof(float) * nstates * length);
	scale = (float *) malloc(sizeof(float) * length);

	scale[0] = 0;
	for (j = 0; j < nstates; j++) {
		alpha[j] = pi[j] * b[(obs[0] * nstates) + j];
		scale[0] += alpha[j];
	}

	for (j = 0; j < nstates; j++) {
		alpha[j] /= scale[0];
	}
	
	for (t = 1; t < length; t++) {
		scale[t] = 0;
		for (j = 0; j < nstates; j++){
			alpha[(t * nstates) + j] = 0;
			for (k = 0; k < nstates; k++){
				alpha[(t * nstates) + j] += alpha[((t-1) * nstates) + k] *
							a[(k * nstates) + j] *
							b[(obs[t] * nstates) + j];
			}
			scale[t] += alpha[(t * nstates) + j];
		}
		for (j = 0; j < nstates; j++) {
			alpha[(t * nstates) + j] /= scale[t];
		}
	}
	
	log_lik = 0;
	for (t = 0; t < length; t++) {
		log_lik += log10(scale[t]);
	}
	
	free(alpha);
	free(scale);

	return log_lik;
}
