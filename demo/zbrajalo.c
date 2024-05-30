#include <stdio.h>
#include <string.h>
#define STUC_LRELU_FACT 0.1
#include "../src/stuc/stuc.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define GREEN "\x1b[1;32m"
#define WHITE "\x1b[0;37m"

float_t tData[] = {
0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  //  0 +  0 =  0
0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 1,  //  0 +  1 =  1
0, 0, 0, 0,  0, 0, 1, 0,  0, 0, 1, 0,  //  0 +  2 =  2
0, 0, 0, 0,  0, 0, 1, 1,  0, 0, 1, 1,  //  0 +  3 =  3
0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 0, 0,  //  0 +  4 =  4
0, 0, 0, 0,  0, 1, 0, 1,  0, 1, 0, 1,  //  0 +  5 =  5
0, 0, 0, 0,  0, 1, 1, 0,  0, 1, 1, 0,  //  0 +  6 =  6
0, 0, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  //  0 +  8 =  8
0, 0, 0, 0,  1, 0, 0, 1,  1, 0, 0, 1,  //  0 +  9 =  9
0, 0, 0, 0,  1, 0, 1, 0,  1, 0, 1, 0,  //  0 + 10 = 10
0, 0, 0, 0,  1, 0, 1, 1,  1, 0, 1, 1,  //  0 + 11 = 11
0, 0, 0, 0,  1, 1, 0, 0,  1, 1, 0, 0,  //  0 + 12 = 12
0, 0, 0, 0,  1, 1, 0, 1,  1, 1, 0, 1,  //  0 + 13 = 13
0, 0, 0, 0,  1, 1, 1, 0,  1, 1, 1, 0,  //  0 + 14 = 14
0, 0, 0, 1,  0, 0, 0, 1,  0, 0, 1, 0,  //  1 +  1 =  2
0, 0, 0, 1,  0, 0, 1, 1,  0, 1, 0, 0,  //  1 +  3 =  4
0, 0, 0, 1,  0, 1, 0, 0,  0, 1, 0, 1,  //  1 +  4 =  5
0, 0, 0, 1,  0, 1, 0, 1,  0, 1, 1, 0,  //  1 +  5 =  6
0, 0, 0, 1,  0, 1, 1, 0,  0, 1, 1, 1,  //  1 +  6 =  7
0, 0, 0, 1,  1, 0, 0, 0,  1, 0, 0, 1,  //  1 +  8 =  9
0, 0, 0, 1,  1, 0, 0, 1,  1, 0, 1, 0,
0, 0, 0, 1,  1, 0, 1, 0,  1, 0, 1, 1,
0, 0, 0, 1,  1, 0, 1, 1,  1, 1, 0, 0,
0, 0, 0, 1,  1, 1, 1, 0,  1, 1, 1, 1,
0, 0, 1, 0,  0, 0, 0, 1,  0, 0, 1, 1,
0, 0, 1, 0,  0, 0, 1, 0,  0, 1, 0, 0,
0, 0, 1, 0,  0, 0, 1, 1,  0, 1, 0, 1,
0, 0, 1, 0,  0, 1, 0, 0,  0, 1, 1, 0,
0, 0, 1, 0,  0, 1, 0, 1,  0, 1, 1, 1,
0, 0, 1, 0,  0, 1, 1, 0,  1, 0, 0, 0,
0, 0, 1, 0,  1, 0, 0, 0,  1, 0, 1, 0,
0, 0, 1, 0,  1, 0, 0, 1,  1, 0, 1, 1,
0, 0, 1, 0,  1, 0, 1, 0,  1, 1, 0, 0,
0, 0, 1, 0,  1, 0, 1, 1,  1, 1, 0, 1,
0, 0, 1, 0,  1, 1, 0, 1,  1, 1, 1, 1,
0, 0, 1, 1,  0, 0, 0, 0,  0, 0, 1, 1,
0, 0, 1, 1,  0, 0, 1, 0,  0, 1, 0, 1,
0, 0, 1, 1,  0, 0, 1, 1,  0, 1, 1, 0,
0, 0, 1, 1,  0, 1, 0, 0,  0, 1, 1, 1,
0, 0, 1, 1,  0, 1, 0, 1,  1, 0, 0, 0,
0, 0, 1, 1,  0, 1, 1, 0,  1, 0, 0, 1,
0, 0, 1, 1,  0, 1, 1, 1,  1, 0, 1, 0,
0, 0, 1, 1,  1, 0, 0, 0,  1, 0, 1, 1,
0, 0, 1, 1,  1, 0, 1, 0,  1, 1, 0, 1,
0, 0, 1, 1,  1, 0, 1, 1,  1, 1, 1, 0,
0, 0, 1, 1,  1, 1, 0, 0,  1, 1, 1, 1,
0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,
0, 1, 0, 0,  0, 0, 1, 0,  0, 1, 1, 0,
0, 1, 0, 0,  0, 0, 1, 1,  0, 1, 1, 1,
0, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,
0, 1, 0, 0,  0, 1, 0, 1,  1, 0, 0, 1,
0, 1, 0, 0,  0, 1, 1, 1,  1, 0, 1, 1,
0, 1, 0, 0,  1, 0, 0, 0,  1, 1, 0, 0,
0, 1, 0, 0,  1, 0, 0, 1,  1, 1, 0, 1,
0, 1, 0, 0,  1, 0, 1, 0,  1, 1, 1, 0,
0, 1, 0, 1,  0, 0, 0, 0,  0, 1, 0, 1,
0, 1, 0, 1,  0, 0, 0, 1,  0, 1, 1, 0,
0, 1, 0, 1,  0, 0, 1, 0,  0, 1, 1, 1,
0, 1, 0, 1,  0, 0, 1, 1,  1, 0, 0, 0,
0, 1, 0, 1,  0, 1, 0, 0,  1, 0, 0, 1,
0, 1, 0, 1,  0, 1, 0, 1,  1, 0, 1, 0,
0, 1, 0, 1,  0, 1, 1, 1,  1, 1, 0, 0,
0, 1, 0, 1,  1, 0, 0, 1,  1, 1, 1, 0,
0, 1, 0, 1,  1, 0, 1, 0,  1, 1, 1, 1,
0, 1, 1, 0,  0, 0, 0, 0,  0, 1, 1, 0,
0, 1, 1, 0,  0, 0, 1, 0,  1, 0, 0, 0,
0, 1, 1, 0,  0, 0, 1, 1,  1, 0, 0, 1,
0, 1, 1, 0,  0, 1, 0, 1,  1, 0, 1, 1,
0, 1, 1, 0,  1, 0, 0, 0,  1, 1, 1, 0,
0, 1, 1, 0,  1, 0, 0, 1,  1, 1, 1, 1,
0, 1, 1, 1,  0, 0, 0, 0,  0, 1, 1, 1,
0, 1, 1, 1,  0, 0, 0, 1,  1, 0, 0, 0,
0, 1, 1, 1,  0, 0, 1, 0,  1, 0, 0, 1,
0, 1, 1, 1,  0, 0, 1, 1,  1, 0, 1, 0,
0, 1, 1, 1,  0, 1, 0, 0,  1, 0, 1, 1,
0, 1, 1, 1,  0, 1, 0, 1,  1, 1, 0, 0,
0, 1, 1, 1,  0, 1, 1, 0,  1, 1, 0, 1,
0, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 0,
0, 1, 1, 1,  1, 0, 0, 0,  1, 1, 1, 1,
1, 0, 0, 0,  0, 0, 1, 1,  1, 0, 1, 1,
1, 0, 0, 0,  0, 1, 0, 0,  1, 1, 0, 0,
1, 0, 0, 0,  0, 1, 1, 0,  1, 1, 1, 0,
1, 0, 0, 0,  0, 1, 1, 1,  1, 1, 1, 1,
1, 0, 0, 1,  0, 0, 0, 0,  1, 0, 0, 1,
1, 0, 0, 1,  0, 0, 0, 1,  1, 0, 1, 0,
1, 0, 0, 1,  0, 0, 1, 1,  1, 1, 0, 0,
1, 0, 0, 1,  0, 1, 0, 0,  1, 1, 0, 1,
1, 0, 0, 1,  0, 1, 0, 1,  1, 1, 1, 0,
1, 0, 0, 1,  0, 1, 1, 0,  1, 1, 1, 1,
1, 0, 1, 0,  0, 0, 0, 0,  1, 0, 1, 0,
1, 0, 1, 0,  0, 0, 0, 1,  1, 0, 1, 1,
1, 0, 1, 0,  0, 0, 1, 0,  1, 1, 0, 0,
1, 0, 1, 0,  0, 0, 1, 1,  1, 1, 0, 1,
1, 0, 1, 0,  0, 1, 0, 0,  1, 1, 1, 0,
1, 0, 1, 0,  0, 1, 0, 1,  1, 1, 1, 1,
1, 0, 1, 1,  0, 0, 1, 0,  1, 1, 0, 1,
1, 0, 1, 1,  0, 1, 0, 0,  1, 1, 1, 1,
1, 1, 0, 0,  0, 0, 0, 0,  1, 1, 0, 0,  // 12 +  0 = 12
1, 1, 0, 0,  0, 0, 0, 1,  1, 1, 0, 1,  // 12 +  1 = 13
1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 1, 1,  // 12 +  3 = 15
1, 1, 0, 1,  0, 0, 0, 0,  1, 1, 0, 1,  // 13 +  0 = 13
1, 1, 0, 1,  0, 0, 0, 1,  1, 1, 1, 0,  // 13 +  1 = 14
1, 1, 1, 0,  0, 0, 0, 0,  1, 1, 1, 0,  // 14 +  0 = 14
1, 1, 1, 0,  0, 0, 0, 1,  1, 1, 1, 1,  // 14 +  1 = 15
};
// 80% svih mogućih kombinacija
const size_t number_of_samples = 104;
const size_t inputs = 8;
const size_t outputs = 4;


typedef union {
    uint8_t val;
    struct {
	// Za Little endian, inače `of` sadrži tražene bitove
        uint8_t b3 : 1;
        uint8_t b2 : 1;
        uint8_t b1 : 1;
        uint8_t b0 : 1;
        uint8_t of : 4; // overflow
    } at;
} num;

typedef enum {
	MODE_NONE     = 0,
	MODE_VALIDATE = 1<<0,
	MODE_TRAIN    = 1<<1,
	MODE_USE      = 1<<2,
	MODE_COUNT    = 3,
} Program_mode;

bool nnRezEq(float a, float b) {
	if (isnan(a) || isnan(b)) return false;
	if ((int)roundf(a) == (int)roundf(b)) return true;
	return false;
}

Stuc_nn loadNetwork(const char* nn_path, bool reset_nn);
float validateNetwork(Stuc_nn nn, size_t *valid_samples);
void forwardOperands(Stuc_nn nn, num i, num j);
void trainNetwork(Stuc_nn nn, const char *nn_path, const size_t batch_size, const size_t epoch_count, float learn_rate, bool save_to_file);
void useNetwork(Stuc_nn nn, bool print_raw_data);
char *shiftArgs(char ***argv, int *argc);

int main(int argc, char **argv) {

#ifdef WIN32
	system("chcp 65001"); // Postavi charset na UTF-8
#endif
	char* nn_path = "./trainedNNs/Zbrajalo.snn";


	char *program = shiftArgs(&argv, &argc);

	Program_mode program_mode = MODE_NONE;
	size_t batch_size  = 5000;
	size_t epoch_count = 10;
	float  learn_rate  = 1;
	bool   save_to_file = true;
	bool   reset_nn = false;
	bool print_raw_output = false;

	while(argc > 0) {
		char *curr_arg = shiftArgs(&argv, &argc);

		if (!strcmp(curr_arg, "use")) {
			program_mode |= MODE_USE;
			continue;
		}
		if (!strcmp(curr_arg, "train")) {
			program_mode |= MODE_TRAIN;
			continue;
		}
		if (!strcmp(curr_arg, "validate")) {
			program_mode |= MODE_VALIDATE;
			continue;
		}
		if (!strcmp(curr_arg, "--batch-size")) {
			if (argc <= 0) {
				printf("Opcija se koristi: --batch-size [broj]\n");
				exit(EXIT_FAILURE);
			}
			char *next_arg = shiftArgs(&argv, &argc);
			errno = 0;
			char *end_ptr = next_arg + strlen(next_arg);
			long ret = strtol(next_arg, &end_ptr, 10);
			if (errno) {
				perror("Greška pri čitanju broja opcije --batch-size");
				exit(EXIT_FAILURE);
			} else if (ret <= 0) {
				printf("--batch-size [broj] ne smije biti negativan ili 0\n");
				exit(EXIT_FAILURE);
			}
			batch_size = ret;
			continue;
		}
		if (!strcmp(curr_arg, "--epoch-count")) {
			if (argc <= 0) {
				printf("Opcija se koristi: --epoch-count [broj]\n");
				exit(EXIT_FAILURE);
			}
			char *next_arg = shiftArgs(&argv, &argc);
			errno = 0;
			char *end_ptr = next_arg + strlen(next_arg);
			long ret = strtol(next_arg, &end_ptr, 10);
			if (errno) {
				perror("Greška pri čitanju broja opcije --epoch-count");
				exit(EXIT_FAILURE);
			} else if (ret <= 0) {
				printf("--epoch-count [broj] ne smije biti negativan ili 0\n");
				exit(EXIT_FAILURE);
			}
			epoch_count = ret;
			continue;
		}
		if (!strcmp(curr_arg, "--no-save")) {
			save_to_file = false;
			continue;
		}
		if (!strcmp(curr_arg, "--nn-path")) {
			if (argc <= 0) {
				printf("Opcija se koristi: --nn-path [path]\n");
				exit(EXIT_FAILURE);
			}
			nn_path = shiftArgs(&argv, &argc);
			continue;
		}
		if (!strcmp(curr_arg, "--reset-nn")) {
			reset_nn = true;
			continue;
		}
		if (!strcmp(curr_arg, "--learn-rate")) {
			if (argc <= 0) {
				printf("Opcija se koristi: --learn-rate [realan_broj]\n");
				exit(EXIT_FAILURE);
			}
			char *l_rate_str = shiftArgs(&argv, &argc);
			if(sscanf(l_rate_str, "%f ", &learn_rate) != 1) {
				if (errno == 0) errno = EINVAL;
				perror("Greška pri čitanju broja opcije --learn-rate");
				exit(EXIT_FAILURE);
			}
			if (learn_rate <= 0) {
				printf("--learn-rate ne smije biti negativan ili 0\n");
				exit(EXIT_FAILURE);
			}
			continue;
		}
		if (!strcmp(curr_arg, "--raw-output")) {
			print_raw_output = true;
			continue;
		}
		if (!strcmp(curr_arg, "--help") || !strcmp(curr_arg, "-h")) {
			printf("\nHelp menu programa za upravljanje neuronskom mrežom Zbrajalo\n\n");

			printf("Korištenje: %s [mod_korištenja] [opcije]\n\n", program);
			printf("Mod korištenja: \n");
			printf("\tvalidate - provjera točnosti neuronske mreže spremljene u file `trainedNNs/Zbrajalo.snn`\n");
			printf("\tuse      - korištenje neuronske mreže spremljene u file `trainedNNs/Zbrajalo.snn`\n");
			printf("\ttrain    - treniranje neuronske mreže i spremljene u file `trainedNNs/Zbrajalo.snn`\n");
			printf("File `trainedNNs/Zbrajalo.snn` može se promjeniti, a ukoliko ne postoji kreira se nova prazna neuronska mreža.\n");
			printf("Ukoliko je navedeno više modova, izvršavaju se redosljedom: validate, train, use.\n\n");

			printf("Opcije: \n");
			printf("\t--batch-size [broj]  - određuje broj ciklusa učenja po epohi. [broj] ne smije biti negativan ili nula\n");
			printf("\t--epoch-count [broj] - određuje broj ciklusa učenja nakon kojeg se ispisuje točnost mreže. [broj] ne smije biti negativan ili nula\n");
			printf("\t--learn-rate [broj]  - određuje brzinu učenja neuronske mreže. Realan [broj] ne smije biti negativan ili nula (zadano: 1.0)\n");
			printf("\t--no-save            - Nakon učenja, neuronska mreža neće se spremiti u file (korisno s modom `use`)\n");
			printf("\t--nn-path [path]     - Promjena zadane putanje za [path] iz kojeg će se učitavati i u koji će se spremati neuronska mreža\n");
			printf("\t--reset-nn           - Resetirati će neuronsku mrežu koju učita (isto kao da ne pronađe file)\n");
			printf("\t--raw-output         - Ispisuje točan podatak koji je dala mreža umjesto da zaokružuje (kod moda use)\n");
			printf("\t--help ili -h        - prikazuje ovaj meni\n\n");

			printf("\n\nCopyright © 2023. Mateo Kos, pod MIT licencom.\n\n");


			exit(EXIT_SUCCESS);
		}

		printf("Nepoznata opcija: `%s`\n Za pomoć: `%s --help`\n", curr_arg, program);
		exit(EXIT_FAILURE);
	}

	if (program_mode == 0) {
		printf("Potrebno je specificirati mod.\n Za pomoć: `%s --help`\n", program);
		exit(EXIT_FAILURE);
	}

	Stuc_nn nn = loadNetwork(nn_path, reset_nn);

	for (size_t i = 0; i < MODE_COUNT; i++) {
		Program_mode mode = program_mode & 1 << i;

		switch(mode) {
		case MODE_NONE:  continue;
		case MODE_COUNT: exit(EXIT_FAILURE);
		case MODE_USE:   useNetwork(nn, print_raw_output); break;
		case MODE_TRAIN: trainNetwork(nn, nn_path, batch_size, epoch_count, learn_rate, save_to_file); break;
		case MODE_VALIDATE: {
				size_t valid_samples;
				float uspjeh = validateNetwork(nn, &valid_samples);
				printf("\nNeuronska mreža prošla %zu validacijskih uzoraka [%.2f%%]\n", valid_samples, uspjeh);
			}
		}
	}


	stuc_nnFree(nn);
	return EXIT_SUCCESS;
}

Stuc_nn loadNetwork(const char* nn_path, bool reset_nn) {

	Stuc_nn nn;
	uint8_t file_ret = 0;

	if (reset_nn || (file_ret = stuc_nnLoadFromFile(&nn, nn_path))) {
		stuc_printIOFlags(file_ret);

		size_t arch[] = {inputs, 14, 10, 7, outputs};
		Stuc_activationFunction funk[STUC_LENP(arch)-1] = {0};
		stuc_setActivation(funk, STUC_LENP(funk), STUC_ACTIVATE_RELU);
		funk[STUC_LENP(funk)-1] = STUC_ACTIVATE_SIGMOID;

		nn = stuc_nnAlloc(funk, arch, STUC_LENP(arch));
		srand(time(0));
#ifdef PERFECT
		srand(1714326700);   //GNU_LINUX_PERFECT_SEED
#ifdef WIN32
		srand(1714328498);   //MS_WINDOWS_PERFECT_SEED
#endif
#endif
		stuc_nnRand(nn, -1, 1);
	} else {
		srand(time(0));
	}

	return nn;
}

void useNetwork(Stuc_nn nn, bool print_raw_data) {

	bool should_exit = false;
	do {
		num num1, num2;
		printf("\nUnesi dva broja: ");
		if(scanf("%hhu %hhu", &num1.val, &num2.val) != 2) {
			if (errno == 0) errno = EINVAL;
			perror("Error reading number");
			exit(EXIT_FAILURE);
		}

		if (num1.at.of || num2.at.of) {
			printf("Broj mora biti 4 bitni [0 - 15]!\n");
			continue;
		}

		if (num1.val == 0) should_exit = 1;

		num temp = {0};
		temp.val = num1.val + num2.val;
		if (temp.at.of != 0) {
			printf("Rezultat mora biti 4 bitni [0 - 15]!\n");
			continue;
		}

		forwardOperands(nn, num1, num2);
		num sum = {0};
		sum.at.b0 = roundf(STUC_AT_OUTPUT(nn, 0));
		sum.at.b1 = roundf(STUC_AT_OUTPUT(nn, 1));
		sum.at.b2 = roundf(STUC_AT_OUTPUT(nn, 2));
		sum.at.b3 = roundf(STUC_AT_OUTPUT(nn, 3));
		printf("%u + %u = %u", num1.val, num2.val, sum.val);

		if (print_raw_data) {
			float val = 0;
			for (size_t i = 0; i < 4; i++) {
				float_t out = STUC_AT_OUTPUT(nn, 3-i);
				val += out*(1 << i);
			}
			printf(" (%f)", val);
		}

		printf("\n");

	} while(!should_exit);
}

float validateNetwork(Stuc_nn nn, size_t *valid_samples) {

	size_t _valid_samples = 0;
	if (valid_samples == NULL) valid_samples = &_valid_samples;

	size_t all_samples = 0;
	for (num i = {0}; i.val < 16; i.val++) {
		for (num j = {0}; j.val < 16; j.val++) {
		    num sum = { .val = i.val + j.val};
		    if (sum.at.of == 0) {
			bool valid = true;
			forwardOperands(nn, i, j);

			valid &= nnRezEq((float)sum.at.b0, STUC_AT_OUTPUT(nn, 0));
			valid &= nnRezEq((float)sum.at.b1, STUC_AT_OUTPUT(nn, 1));
			valid &= nnRezEq((float)sum.at.b2, STUC_AT_OUTPUT(nn, 2));
			valid &= nnRezEq((float)sum.at.b3, STUC_AT_OUTPUT(nn, 3));

			all_samples++;
			if (valid) _valid_samples++;
		    }
		}
	}

	*valid_samples = _valid_samples;
	return (float)_valid_samples/all_samples * 100;
}

void forwardOperands(Stuc_nn nn, num i, num j) {

	STUC_AT_INPUT(nn, 0) = i.at.b0;
	STUC_AT_INPUT(nn, 1) = i.at.b1;
	STUC_AT_INPUT(nn, 2) = i.at.b2;
	STUC_AT_INPUT(nn, 3) = i.at.b3;
	STUC_AT_INPUT(nn, 4) = j.at.b0;
	STUC_AT_INPUT(nn, 5) = j.at.b1;
	STUC_AT_INPUT(nn, 6) = j.at.b2;
	STUC_AT_INPUT(nn, 7) = j.at.b3;

	stuc_nnForward(nn);
}

void trainNetwork(Stuc_nn nn, const char *nn_path, const size_t batch_size, const size_t epoch_count, float learn_rate, bool save_to_file) {

	Stuc_mat tInput  = {number_of_samples, inputs,  inputs + outputs, tData};
	Stuc_mat tOutput = {number_of_samples, outputs, inputs + outputs, tData + tInput.cols};

	Stuc_nn gdMap = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layer_count);

	printf("\n");

	for (size_t epoch = 1; epoch < epoch_count + 1; epoch++) {
		for (size_t i = 0; i < batch_size; i++) {
#ifdef FINITE_DIFF
			float_t eps = 1e-1;
			stuc_nnFiniteDiffNoAlloc(nn, gdMap, eps, tInput, tOutput);
#else
#ifdef BACKPROP
			float_t boostMultiplier = 1;
			stuc_nnBackpropNoAlloc(nn, gdMap, tInput, tOutput, boostMultiplier);
#else
#error "Nije definirana metoda ucenja, definirati sa -D[ime_metode]"
#endif
#endif
			stuc_nnApplyDiff(nn, gdMap, learn_rate);


			if (i % batch_size == 0) {
				printf("cost = %.32f                          \n",
					stuc_nnCost(nn, tInput, tOutput));
			}

			if (batch_size < 4 || i % (batch_size / 4) == 1) {
				printf("Traning Neural Network Model: %.2f%%\r",
					(epoch / (float_t)epoch_count) * 100);
				fflush(stdout);
			}

		}
	}

	printf("\n\nVerifikacija: ");
	size_t valid_samples;
	float uspjeh = validateNetwork(nn, &valid_samples);

	if (save_to_file) {
		stuc_nnSaveToFile(nn, nn_path);
		printf(GREEN"Network saved to file:"WHITE" %s\n", nn_path);
	}

	printf("\nNeuronska mreža prošla %zu validacijskih uzoraka [%.2f%%] \n\n", valid_samples, uspjeh);

	stuc_nnFree(gdMap);
}


char *shiftArgs(char ***argv, int *argc) {
	char **curr = argv[0];
	*argv = &((*argv)[1]);
	*argc = *argc - 1;
	return *curr;
}
