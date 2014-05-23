#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hmm.h"
#include "hmm_bwa_c.h"
#include "hmm_test.h"

float run_hmm_bwa(	Hmm *hmm,
			Obs *in_obs,
			int iterations,
			float threshold)
{
	int iter;
	float new_log_lik;
	float old_log_lik = 0;
	float *alpha;
	float *scale;
	float *beta;
	float *gamma_sum;
	float *xi_sum;
	float *a = hmm->a;
	float *b = hmm->b;
	float *pi = hmm->pi;
	int nstates = hmm->nstates;
	int nsymbols = hmm->nsymbols;
	int *obs = in_obs->data;
	int length = in_obs->length;
	int j;
	int k;

	alpha = (float *) malloc(sizeof(float) * nstates * length);
	scale = (float *) malloc(sizeof(float) * length);
	beta = (float *) malloc(sizeof(float) * nstates * length);
	gamma_sum = (float *) malloc(sizeof(float) * nstates);
	xi_sum = (float *) malloc(sizeof(float) * nstates * nstates);

	for (iter = 0; iter < iterations; iter++) {
		new_log_lik = calc_alpha(hmm, in_obs, alpha, scale);
		calc_beta(hmm, in_obs, scale, beta);
		calc_gamma_sum(alpha, beta, nstates, length, gamma_sum);
		calc_xi_sum(hmm, in_obs, alpha, beta, xi_sum);
		estimate_a(alpha, beta, nstates, length, gamma_sum, xi_sum, a);
		estimate_b(hmm, in_obs, alpha, beta, gamma_sum);
		estimate_pi(alpha, beta, nstates, pi);

		if (threshold > 0 && iter > 0) {
			if (fabs(pow(10,new_log_lik) - pow(10,old_log_lik)) < threshold) {
				break;
			}
		}
		old_log_lik = new_log_lik;
	}
	
	free(alpha);
	free(beta);
	free(gamma_sum);
	free(xi_sum);

	return new_log_lik;
}

float calc_alpha(Hmm *in_hmm, Obs *in_obs, float *alpha, float *scale)
{
	int j;
        int k;
        int t;
        float log_lik;
        float *a = in_hmm->a;
        float *b = in_hmm->b;
        float *pi = in_hmm->pi;
        int nstates = in_hmm->nstates;
        int *obs = in_obs->data;
        int length = in_obs->length;

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

        return log_lik;
}

void calc_beta(Hmm *in_hmm, Obs *in_obs, float *scale, float *beta)
{
	int i;
	int j;
	int t;
	float *a = in_hmm->a;
	float *b = in_hmm->b;
	int nstates = in_hmm->nstates;
	int *obs = in_obs->data;
	int length = in_obs->length;

	for (j = 0; j < nstates; j++) {
		beta[((length - 1) * nstates) +j] = 1 / scale[length - 1];
	}

	for (t = length - 2; t >= 0; t--) {
		for (i = 0; i < nstates; i++) {
			beta[(t * nstates) + i] = 0;
			for (j = 0; j < nstates; j++) {
				beta[(t * nstates) + i] += beta[((t+1) * nstates) + j] *
								a[(i * nstates) + j] *
								b[(obs[t+1] * nstates) + j] /
								scale[t];
			}
		}
	}
}

void calc_gamma_sum(	float *alpha,
			float *beta,
			int nstates,
			int length,
			float *gamma_sum)
{
	int j;
	int t;
	
	for (j = 0; j < nstates; j++) {
		gamma_sum[j] = 0;
		for (t = 0; t < length; t++) {
			gamma_sum[j] += alpha[(t * nstates) + j] * beta[(t * nstates) + j];
		}
	}
}

void calc_xi_sum(	Hmm *in_hmm,
			Obs *in_obs,
			float *alpha,
			float *beta,
			float *xi_sum)
{
	float *a = in_hmm->a;
	float *b = in_hmm->b;
	int nstates = in_hmm->nstates;
	int *obs = in_obs->data;
	int length = in_obs->length;
	float pr_ab;
	float *xi;
	int t;
	int i;
	int j;

	xi = (float *) malloc(sizeof(float) * nstates * nstates);

	for (i = 0; i < nstates; i++) {
		for (j = 0; j < nstates; j++) {
			xi_sum[(i * nstates) + j] = 0;
		}
	}

	for (t = 0; t < length - 1; t++) {
		pr_ab = 0;
		for (i = 0; i < nstates; i++) {
			pr_ab += alpha[(t * nstates) + i] * beta[(t * nstates) + i];
		}

		for (i = 0; i < nstates; i++) {
			for (j = 0; j < nstates; j++) {
				xi[(i * nstates) + j] =	alpha[(t * nstates) + i] *
							a[(i * nstates) + j] *
							b[(obs[t+1] * nstates) + j] *
							beta[((t+1) * nstates) + j] /
							pr_ab;
			}
		}
		
		for (i = 0; i < nstates; i++) {
			for (j = 0; j < nstates; j++) {
				xi_sum[(i * nstates) + j] += xi[(i * nstates) + j];
			}
		}
	}

	free(xi);	
}

void estimate_a(	float *alpha,
			float *beta,
			int nstates,
			int length,
			float *gamma_sum,
			float *xi_sum,
			float *a)
{
	float sum_ab = 0;
	float sum_a;
	int i;
	int j;

	for (i = 0; i < nstates; i++) {
		sum_ab += alpha[((length-1) * nstates) + i] *
			beta[((length-1) * nstates) + i];
	}

	for (i = 0; i < nstates; i++) {
		for (j = 0; j < nstates; j++) {
			a[(i * nstates) + j] = xi_sum[(i * nstates) + j] / (gamma_sum[i] -
						(alpha[((length-1) * nstates) + i] *
						beta[((length-1) * nstates) + i] / sum_ab));
		}
		sum_a =  0;
		for (j = 0; j < nstates; j++) {
			sum_a += a[(i * nstates) + j];
		}
		for (j = 0; j < nstates; j++) {
			a[(i * nstates) + j] = a[(i * nstates) + j] / sum_a;
		}
	}
}

void estimate_b(	Hmm *in_hmm,
			Obs *in_obs,
			float *alpha,
			float *beta,
			float *gamma_sum)
{
	float *a = in_hmm->a;
	float *b = in_hmm->b;
	int nstates = in_hmm->nstates;
	int nsymbols = in_hmm->nsymbols;
	int *obs = in_obs->data;
	int length = in_obs->length;
	float *out_sum;
	float sum_ab;
	float sum_b;
	int i;
	int k;
	int t;

	out_sum = (float *) malloc(sizeof(float) * nstates * nsymbols);

	for (k = 0; k < nsymbols; k++) {
		for (i = 0; i < nstates; i++){
			out_sum[(k * nstates) + i] = 0;
		}
	}
	
	for (k = 0; k < nsymbols; k++) {
		for (t = 0; t < length; t++) {
			if (obs[t] == k) {
				sum_ab = 0;
				for (i = 0; i < nstates; i++) {
					sum_ab += alpha[(t * nstates) + i] *
						beta[(t * nstates) + i];
				}
				for (i = 0; i < nstates; i++) {
					out_sum[(k * nstates) + i] += alpha[(t * nstates) + i] *
									beta[(t * nstates) + i] /
									sum_ab;
				}
			}
		}
		for (i = 0; i < nstates; i++) {
			b[(k * nstates) + i] = out_sum[(k * nstates) + i] / gamma_sum[i];
		}
	}

	for (i = 0; i < nstates; i++) {
		sum_b = 0;
		for (k = 0; k < nsymbols; k++) {
			sum_b += b[(k * nstates) + i];
		}
		for (k = 0; k < nsymbols; k++) {
			if (b[(k * nstates) + i] == 0) {
				b[(k * nstates) + i] = 1e-10;
			} else {
				b[(k * nstates) + i] = b[(k * nstates) + i] / sum_b;
			}
		}
	}

	free(out_sum);
}
void estimate_pi(	float *alpha,
			float *beta,
			int nstates,
			float *pi)
{
	float sum_ab = 0;
	int i;
	
	for (i = 0; i < nstates; i++) {
		sum_ab += alpha[i] * beta[i];
	}

	for (i = 0; i < nstates; i++) {
		pi[i] = (alpha[i] * beta[i]) / sum_ab;
	}
}
