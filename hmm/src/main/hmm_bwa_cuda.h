#ifndef HMM_BWA_CUDA_H
#define HMM_BWA_CUDA_H

float run_hmm_bwa(	Hmm *hmm,
			Obs *in_obs,
			int iterations,
			float threshold);

float calc_alpha();

int calc_beta();

void calc_gamma_sum();

int calc_xi_sum();

int estimate_a();

int estimate_b();

int estimate_pi();

#endif /* HMM_BWA_CUDA_H */
