#include "stuc.h"

#ifndef NO_STDIO

#define typeof(x) __typeof__(x)
#define STUC_FILE_PREFIX "MKstucNN"

uint8_t stuc_nnSaveToFile(Stuc_nn nn, const char *file_path) {
	FILE *fp;
	uint8_t return_flags = 0;

	fp = fopen(file_path, "wb");
	if (fp == NULL) {
		fprintf(stderr, "\x1b[1;31mError\x1b[0;37m opening file \"%s\": %s\n", file_path, strerror(errno));
		return return_flags |= STUC_IOFLAG_UNABLE_TO_WRITE;
	}
	
	if (!STUC_SOFT_ASSERT(strlen(STUC_FILE_PREFIX) == 8)) return_flags |= STUC_IOFLAG_TYPE_MISMATCH;
	fwrite(STUC_FILE_PREFIX, strlen(STUC_FILE_PREFIX), 1, fp);

	if (!STUC_SOFT_ASSERT(sizeof(typeof(nn.layer_count)) == 8)) return_flags |= STUC_IOFLAG_TYPE_MISMATCH;
	fwrite(&nn.layer_count, sizeof(typeof(nn.layer_count)), 1, fp);

	STUC_ASSERT(sizeof(typeof(nn.arhitektura[0])) == sizeof(size_t));
	fwrite(nn.arhitektura, sizeof(size_t), nn.layer_count, fp);

	STUC_ASSERT(sizeof(typeof(nn.aktivacije[0])) == sizeof(Stuc_activationFunction));
	fwrite(nn.aktivacije, sizeof(Stuc_activationFunction), nn.layer_count - 1, fp);

	for (size_t i = 1; i < nn.layer_count; i++) {
		STUC_ASSERT(STUC_NN_AT(nn, i).w.cols == STUC_NN_AT(nn, i).b.cols && "Neural network is corrupt!");

		const size_t rows = STUC_NN_AT(nn, i).w.rows;
		const size_t cols = STUC_NN_AT(nn, i).w.cols;

		fwrite(&i, sizeof(i), 1, fp);
		fwrite(&rows, sizeof(rows), 1, fp);
		fwrite(&cols, sizeof(cols), 1, fp);
		fwrite(STUC_NN_AT(nn, i).b.el, sizeof(float_t), cols, fp);
		fwrite(STUC_NN_AT(nn, i).w.el, sizeof(float_t), cols * rows, fp);
	
	}

	fclose(fp);

	return return_flags;
}

void s_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t ret = fread(ptr, size, nmemb, stream);
        if (ret != nmemb) {
            fprintf(stderr, "\x1b[1;31mError\x1b[0;37m reading from file: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

	return;
}

uint8_t stuc_nnLoadFromFile(Stuc_nn *nn, const char *file_path) {
	FILE *fp;
	uint8_t return_flags = 0;

	fp = fopen(file_path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "\x1b[1;31mError\x1b[0;37m opening file \"%s\": %s\n", file_path, strerror(errno));
		return return_flags |= STUC_IOFLAG_UNABLE_TO_READ;
	}
	
	if (!STUC_SOFT_ASSERT(strlen(STUC_FILE_PREFIX) == 8)) {
		return return_flags |= STUC_IOFLAG_TYPE_MISMATCH;
	}

	char buff[9] = {0};
	s_fread(buff, strlen(STUC_FILE_PREFIX), 1, fp);

	if (strcmp(buff, STUC_FILE_PREFIX)) {
		return return_flags |= STUC_IOFLAG_WRONG_FILE_TYPE;
	}

	if (!STUC_SOFT_ASSERT(sizeof(typeof(nn->layer_count)) == 8)) {
		return return_flags |= STUC_IOFLAG_TYPE_MISMATCH;
	}

	s_fread(&nn->layer_count, sizeof(typeof(nn->layer_count)), 1, fp);

	STUC_ASSERT(sizeof(typeof(nn->arhitektura[0])) == sizeof(size_t));
	size_t *temp_arh = (size_t *)STUC_MALLOC((nn->layer_count)*sizeof(size_t));
	s_fread(temp_arh, sizeof(size_t), nn->layer_count, fp);

	STUC_ASSERT(sizeof(typeof(nn->aktivacije[0])) == sizeof(Stuc_activationFunction));
	Stuc_activationFunction *temp_act = (Stuc_activationFunction *)STUC_MALLOC((nn->layer_count - 1) * sizeof(Stuc_activationFunction));
	s_fread(temp_act, sizeof(Stuc_activationFunction), nn->layer_count - 1, fp);

	*nn = stuc_nnAlloc(temp_act, temp_arh, nn->layer_count);
	free(temp_act);
	free(temp_arh);

	for (size_t i = 1; i < nn->layer_count; i++) {
		size_t rows;
		size_t cols;
		size_t temp_id;

		s_fread(&temp_id, sizeof(temp_id), 1, fp);
		(void)STUC_SOFT_ASSERT(temp_id == i);

		s_fread(&rows, sizeof(rows), 1, fp);
		s_fread(&cols, sizeof(cols), 1, fp);
		if (!STUC_SOFT_ASSERT(rows == STUC_NN_AT(*nn, i).w.rows)) return_flags |= STUC_IOFLAG_FILE_ERROR;
		if (!STUC_SOFT_ASSERT(cols == STUC_NN_AT(*nn, i).w.cols)) return_flags |= STUC_IOFLAG_FILE_ERROR;

		s_fread(STUC_NN_AT(*nn, i).b.el, sizeof(float_t), cols , fp);
		s_fread(STUC_NN_AT(*nn, i).w.el, sizeof(float_t), cols * rows, fp);
	
	}

	fclose(fp);

	return return_flags;
}

void stuc_printIOFlags(uint8_t flags) {
	for (size_t i = 0; i < STUC_IOFLAG_COUNT; i++)
		switch (flags & (0x1 << i)) {
			case STUC_IOFLAG_TYPE_MISMATCH: printf("Data type mismatch\n"); break;
			case STUC_IOFLAG_WRONG_FILE_TYPE: printf("Wrong file type\n"); break;
			case STUC_IOFLAG_UNABLE_TO_READ: printf("Unable to Read File\n"); break;
			case STUC_IOFLAG_FILE_ERROR: printf("File is corupted or not right\n"); break;
			case STUC_IOFLAG_UNABLE_TO_WRITE: printf("Unable to write to file\n"); break;
			default: break;
		}

	return;
}

#endif // NO_STDIO
