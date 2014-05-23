#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../main/hmm.h"
#include "../main/hmm_test.h"
#include "../main/hmm_fo_c.h"

int main(int argc, char *argv[]){

	int c;
	Hmm *in_hmm;
	Hmm *out_hmm;
	Obs *in_obs;
	char *configfile = NULL;
	float log_lik;
	struct timeval timer;

	in_hmm = (Hmm *)malloc(sizeof(Hmm));
	out_hmm = (Hmm *)malloc(sizeof(Hmm));
	in_obs = (Obs *)malloc(sizeof(Obs));

	while ((c = getopt(argc, argv, "c:n:")) != -1) {
		switch(c) {
			case 'c':
				configfile = optarg;
				break;
			case '?':
				fprintf(stderr, "illegal options\n");
				exit(EXIT_FAILURE);

			default:
				abort();
		}
	}
	read_hmm_file(configfile, in_hmm, in_obs, 0);

	printf("\n");
	printf("HMM parameters read from file: \n");
	print_hmm(in_hmm);
	print_obs(in_obs);

	tic(&timer);
	log_lik = run_hmm_fo(in_hmm, in_obs);
	toc(&timer);
	printf("Log likelihood: %0.4f\n", log_lik);

	free_vars(in_hmm, in_obs);

	return 0;
}
