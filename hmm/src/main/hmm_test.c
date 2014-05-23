#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include "hmm.h"
#include "hmm_test.h"

#define IDX(i,j,d) (((i)*(d))+(j))
#define HANDLE_ERROR(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int timeval_subtract(struct timeval *result, struct timeval *t2,
					struct timeval *t1)
{
	long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) -
			(t1->tv_usec + 1000000 * t1->tv_sec);
	result->tv_sec = diff / 1000000;
	result->tv_usec = diff % 1000000;

	return (diff<0);
}

void tic(struct timeval *timer)
{
	gettimeofday(timer, NULL);
}

void toc(struct timeval *timer)
{
	struct timeval tv_end, tv_diff;
	
	gettimeofday(&tv_end, NULL);
	timeval_subtract(&tv_diff, &tv_end, timer);
	printf("Running time: %ld.%06ld\n", tv_diff.tv_sec, tv_diff.tv_usec);
}

void read_hmm_file(char *configfile, Hmm *hmm, Obs *obs, int transpose)
{
	FILE *fin, *bin;
	
	char *linebuf = NULL;
	size_t buflen = 0;

	int c, k;
	float d;
	opterr = 0;

	int nStates = 0;
	int nSymbols = 0;
	int nSeq = 0;
	int length = 0;

	int i, j;

	if (configfile == NULL) {
		fin = stdin;
	} else {
		fin = fopen(configfile, "r");
		if (fin == NULL) {
			HANDLE_ERROR("fopen");
		}
	}

	i = 0;
	while ((c = getline(&linebuf, &buflen, fin)) != -1) {
		// If you want to watch which line precessing, use next line.
		// printf("%d: %s", c, linebuf);
		if (c <= 1 || linebuf[0] == '#' || linebuf[0] == '\r' || linebuf[0] == '\n') {
			continue;
		}

		if (i == 0) {
			if (sscanf(linebuf, "%d", &nStates) != 1) {
				fprintf(stderr, "Config file format error: %d\n", i);
				free_vars(hmm, obs);
				exit(EXIT_FAILURE);
			}

			hmm->nstates = nStates;
			hmm->pi = (float *) malloc(sizeof(float) * nStates);
			if (hmm->pi == NULL) {
				HANDLE_ERROR("malloc");
			}

			hmm->a = (float *) malloc(sizeof(float) * nStates * nStates);
			if (hmm->a == NULL) {
				HANDLE_ERROR("malloc");
			}
		} else if (i == 1) {
			if (sscanf(linebuf, "%d", &nSymbols) != 1) {
				fprintf(stderr, "Config file format error: %d\n", i);
				free_vars(hmm, obs);
				exit(EXIT_FAILURE);
			}

			hmm->nsymbols = nSymbols;
			hmm->b = (float *) malloc(sizeof(float) * nStates * nSymbols);
			if (hmm->b == NULL) {
				HANDLE_ERROR("malloc");
			}
		} else if (i == 2) {
			bin = fmemopen(linebuf, buflen, "r");
			if (bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}
			
			for (j = 0; j < nStates; j++) {
				if (fscanf(bin, "%f", &d) != 1) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				hmm->pi[j] = d;
			}
			fclose(bin);
		} else if (i <= 2 + nStates) {
			bin = fmemopen(linebuf, buflen, "r");
			if(bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}

			for (j = 0; j < nStates; j++) {
				if (fscanf(bin, "%f", &d) != 1) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				hmm->a[IDX((i-3), j, nStates)] = d;
			}
			fclose(bin);
		} else if ((i <= 2 + nStates * 2) && (transpose == 0)) {
			bin = fmemopen(linebuf, buflen, "r");
			if (bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}

			for (j = 0; j < nSymbols; j++) {
				if (fscanf(bin, "%f", &d) != 1) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				hmm->b[j * nStates + (i - 3 - nStates)] = d;
			}
			fclose(bin);
		} else if ((i <= 2 + nStates + nSymbols) && (transpose)) { 
			bin = fmemopen(linebuf, buflen, "r");
			if (bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}

			for (j = 0; j < nStates; j++) {
				if (fscanf(bin, "%f", &d) != 1) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				hmm->b[(i - 3 - nStates) * nStates + j] = d;
			}	
			fclose(bin);
		} else if ((i == 3 + nStates * 2) && (transpose == 0)) {
			if (sscanf(linebuf, "%d %d", &nSeq, &length) != 2) {
				fprintf(stderr, "Config file format error: %d\n", i);
				free_vars(hmm, obs);
				exit(EXIT_FAILURE);
			}
			obs->length = nSeq * length;

			int memory_size = nSeq * length;
			obs->data = (int *) malloc(sizeof(int) * memory_size);
			if (obs->data == NULL) {
				HANDLE_ERROR("malloc");
			}
		} else if ((i == 3 + nStates + nSymbols) && (transpose)) {
			if (sscanf(linebuf, "%d %d", &nSeq, &length) != 2) {
				fprintf(stderr, "Config file format error: %d\n", i);
				free_vars(hmm, obs);
				exit(EXIT_FAILURE);
			}
			obs->length = nSeq * length;

			obs->data = (int *) malloc(sizeof(int) * nSeq * length);
			if (obs->data == NULL) {
				HANDLE_ERROR("malloc");
			}
		} else if ((i <= 3 + nStates * 2 + nSeq) && (transpose == 0)){
			bin = fmemopen(linebuf, buflen, "r");
			if (bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}
			
			for (j = 0; j < length; j++) {
				if (fscanf(bin, "%d", &k) != 1 || k < 0 || k >= nSymbols) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				obs->data[j * nSeq + (i - 4 - nStates * 2)] = k;
			}
			fclose(bin);
		} else if ((i <= 3 + nStates + nSymbols + nSeq) && (transpose)){
			bin = fmemopen(linebuf, buflen, "r");
			if (bin == NULL) {
				HANDLE_ERROR("fmemopen");
			}

			for (j = 0; j < length; j++) {
				if (fscanf(bin, "%d", &k) != 1 || k < 0 || k >= nSymbols) {
					fprintf(stderr, "Config file format error: %d\n", i);
					free_vars(hmm, obs);
					exit(EXIT_FAILURE);
				}
				obs->data[j * nSeq + (i - 4 - (nStates + nSymbols))] = k;
			}
			fclose(bin);
		}
	
		i++;
	}
	fclose(fin);
	if (linebuf) {
		free(linebuf);
	}

	if (i < 4 + nStates * 2 + nSeq) {
		fprintf(stderr, "Configuration incomplete\n");
		free_vars(hmm, obs);
		exit(EXIT_FAILURE);
	}
}
