#ifndef HMM_H
#define HMM_H

#ifndef TRUE
#define TRUE	1
#endif /* TRUE */
#ifndef FALSE
#define FALSE	0
#endif /* FALSE */

typedef struct {
	int nstates;
	int nsymbols;
	float *a;
	float *b;
	float *pi;
} Hmm;

typedef struct {
	int length;
	int *data;
} Obs;

void set_hmm_a(float *a);
void set_hmm_b(float *b);
void set_hmm_pi(float *pi);
void print_hmm(Hmm *hmm);
void print_obs(Obs *obs);
void free_vars(Hmm *hmm, Obs *obs);

#endif
