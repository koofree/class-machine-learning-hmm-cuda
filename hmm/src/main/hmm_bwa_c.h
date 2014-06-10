#ifndef HMM_BWA_C_H
#define HMM_BWA_C_H

float run_hmm_bwa(	Hmm *hmm,
			Obs *in_obs,
			int iterations,
			float threshold);

float calc_alpha(Hmm *in_hmm, Obs *in_obs, float *alpha, float *scale);

void calc_beta(Hmm *in_hmm, Obs *in_obs, float *scale, float *beta);

void calc_gamma_sum(	float *alpha,
			float *beta,
			int nstates,
			int length,
			float *gamma_sum);

void calc_xi_sum(	Hmm *in_hmm,
			Obs *in_obs,
			float *alpha,
			float *beta,
			float *xi_sum);

void estimate_a(	float *alpha,
			float *beta,
			int nstates,
			int length,
			float *gamma_sum,
			float *xi_sum,
			float *a);

void estimate_b(	Hmm *hmm,
			Obs *in_obs,
			float *alpha,
			float *beta,
			float *gamma_sum);

void estimate_pi(	float *alpha,
			float *beta,
			int nstates,
			float *pi);

#endif	/* HMM_BWA_C_H */

