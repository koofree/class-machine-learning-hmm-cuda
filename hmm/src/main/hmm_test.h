#ifndef HMM_TEST_H
#define HMM_TEST_H

void read_hmm_file(char *configfile, Hmm *hmm, Obs *obs, int transpose);

int timeval_subtract(struct timeval *result, struct timeval *t2,
		struct timeval *t1);

void tic(struct timeval *timer);
void toc(struct timeval *timer);

#endif /* HMM_TEST_H */
