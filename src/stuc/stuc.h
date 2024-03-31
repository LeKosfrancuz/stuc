#ifndef STUC_H
#define STUC_H

#include <stddef.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define STUC_LRELU_FACT 0.1

#define STUC_MAT_AT(mat, i, j)	(mat).el[(i) * (mat).stride + (j)]
#define STUC_NN_AT(nn, layer)	(nn).layers[(layer)]

#define MAT_PRINT(mat)		stuc_matPrint((mat), (#mat), 0)
#define NN_PRINT(nn)		stuc_nnPrint((nn), (#nn))

#define STUC_NN_OUTPUT(nn)	STUC_NN_AT((nn), (nn).layerCount).a
#define STUC_NN_INPUT(nn)	STUC_NN_AT((nn), 0).a

#define STUC_AT_OUTPUT(nn, i)	(STUC_ASSERT((i) < STUC_NN_OUTPUT((nn)).cols), STUC_NN_OUTPUT((nn)).el[(i)])  // Read only
#define STUC_AT_INPUT(nn, i)	STUC_ASSERT((i) < STUC_NN_INPUT((nn)).cols), STUC_NN_INPUT((nn)).el[(i)]  // Write only

#define STUC_LENP(p)		sizeof (p) / sizeof ((p)[0])

typedef struct {
	size_t rows;
	size_t cols;
	size_t stride;
	float_t *el;
} Stuc_mat;

/* Zbroji matrice (a) + (b) te rezultat spremi u [a] 
 *
 * (a) i (b) moraju biti istih dimenzija
 */
void stuc_matAdd(Stuc_mat a, Stuc_mat b);

/* Oduzme matrice (a) - (b) te rezultat spremi u [a]
 *
 * (a) i (b) moraju biti istih dimenzija
 */
void stuc_matSub(Stuc_mat a, Stuc_mat b);

/* Pomnoži matrice (a) * (b) te rezultat spremi u [dst]
 *
 * (a) i (b) moraju biti ulančanih dimenzija
 *  - tj. a.cols == b.rows
 * [dst] - mora biti odgovarajućih dimenzija
 *  - tj. a.rows x b.cols
 */
void stuc_matDot(Stuc_mat dst, Stuc_mat a, Stuc_mat b);

/* Kopira matricu (src) u matricu [dst]
 * (src) i [dst] moraju biti istih dimenzija
 */
void stuc_matCpy(Stuc_mat dst, Stuc_mat src);

/* Ispituje jesu li matrice (a) i (b) jednake
 * Jednake su ako su istih dimenzija i svi elementi
 * su jednaki
 *
 * Return:
 *	 True: Ako su jednake
 *	False: Ako su različite
 */
bool stuc_matEq(Stuc_mat a, Stuc_mat b);

/* Popunjava matricu (a) sa brojem (number) i sprema u [a]
 */
void stuc_matFill(Stuc_mat a, float_t number);

/* Ispisuje sadržaj matrice (a) na stdout
 * !Funkcija je implementirana ako nije definiran NO_STDIO!
 *
 * (a)      - matrica čiji se sadržaj ispisuje
 * (name)   - ime matrice kako će se pokazati pri ispisu
 * (indent) - indentacija ispisa od lijevog ruba ekrana
 *
 * Napomena: Postoji MAKRO naredba koja pojednostavljuje ovu funkciju
 *	     - MAT_PRINT(mat);
 */
void stuc_matPrint(Stuc_mat a, char *name, int indent);

/* Vraća pregled jednog retka matrice (a)
 * !Funkcija ne kopira podatke već postavlja pokazivač na elemente!
 *
 * (a)   - matrica iz koje će se "kopirati" redak
 * (row) - index redka matrice (a) koji će se "kopirati"
 *
 * Return:
 *	Nova matrica koja sadrži pregled na 1 redak podataka (stare)
 */
Stuc_mat stuc_matRowView(Stuc_mat a, size_t row);

/* Alocira mjesto za spremanje elemenata matrice
 * 
 * (rows) - broj redaka nove matrice
 * (cols) - broj stupaca nove matrice
 *
 * Return:
 *	Nova matrica dimenzija (rows) x (cols)
 */
Stuc_mat stuc_matAlloc(size_t rows, size_t cols);

/* Dealocira mjesto za spremanje elemenata matrice (a). To je: (a).el
 * 
 * Napomena: Neće alocirati mjesto koje zauzima struktura matrice
 *	     to je na programeru. Dealocira samo elemente.
 */
void stuc_matFree(Stuc_mat a);


typedef enum {
	STUC_ACTIVATE_RELU,
	STUC_ACTIVATE_SIGMOID,
	STUC_ACTIVATE_TANH,
	STUC_ACTIVATE_SIN,
	STUC_ACTIVATIONS_COUNT
} Stuc_activationFunction;

typedef struct {
	Stuc_activationFunction activation;
	Stuc_mat w; // weights
	Stuc_mat b; // biases
	Stuc_mat a; // aktivacije
} Stuc_nnLayer;

typedef struct {
	Stuc_activationFunction* aktivacije;   // tip aktivacije po sloju (ptr na tablicu)
	size_t* arhitektura;
	size_t  layerCount;   // broj aktiviranih layera (tj. ne broji 0.)
	Stuc_nnLayer* layers; // layerCount + 1 stvarnih elemenata, jer je 0. za input, a on nije "layer"
} Stuc_nn;

/* Prosljeđuje vrijednosti na ulazima neuronske mreže do izlaza
 *
 * (nn) - neuronska mreža kojoj se ulaz prosljeđuje
 *
 * Napomena: Potrebno je ručno postaviti vrijednosti na ulaze
 *	     npr. STUC_AT_INPUT(nn, index_ulaza) = vrijednost_ulaza;
 *	     slično za čitanje: float_t vrijednost_izlaza = STUC_AT_OUTPUT(nn, index_izlaza);
 */
void stuc_nnForward(Stuc_nn nn);

/* Puni neuronsku mrežu s određenom vrijednošću
 *
 * (nn) - neuronska mreža kojoj se pune matrice
 */
void stuc_nnFill(Stuc_nn nn, float_t number);

/* Puni neuronsku mrežu brojevima u rasponu od (low) do (high)
 */
void stuc_nnRand(Stuc_nn nn, float_t low, float_t high);

/* Ispisuje sadržaj neuronske mreže (nn) na stdout
 * !Funkcija je implementirana ako nije definiran NO_STDIO!
 *
 * (nn)     - matrica čiji se sadržaj ispisuje
 * (name)   - ime matrice kako će se pokazati pri ispisu
 *
 * Napomena: Postoji MAKRO naredba koja pojednostavljuje ovu funkciju
 *	     - NN_PRINT(nn);
 */
void stuc_nnPrint(Stuc_nn nn, char *name);

/* Računa trenutnu cijenu neuronske mreže
 * Cijena je broj od 0 do FLT_MAX te predstavlja odstupanje
 * od početnih podataka (tOutput) za svaki ulaz (tInput).
 * 0 je najmanje odstupanje.
 *
 * (nn)      - neuronska mreža čija se cijena računa
 * (tInput)  - ulazni podatci
 * (tOutput) - referentni izlazni podatci
 */
float_t stuc_nnCost(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput);
Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
void stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);
void stuc_nnFiniteDiffNoAlloc(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);
void stuc_nnApplyDiff(Stuc_nn nn, Stuc_nn fd, float_t learningRate);
Stuc_nn stuc_nnAlloc(Stuc_activationFunction *aktivacije, size_t *arhitektura, size_t arhCount);
void stuc_setActivation(Stuc_activationFunction *aktivacije, size_t aktCount, Stuc_activationFunction aktivacija);
void stuc_nnFree(Stuc_nn nn); 
uint8_t stuc_nnSaveToFile(Stuc_nn nn, const char *filePath);
uint8_t stuc_nnLoadFromFile(Stuc_nn *nn, const char *filePath);
void stuc_printIOFlags(uint8_t flags);

typedef enum {
STUC_IOFLAG_FILE_ERROR      = 0x1 << 0,
STUC_IOFLAG_TYPE_MISMATCH   = 0x1 << 1,
STUC_IOFLAG_UNABLE_TO_READ  = 0x1 << 2,
STUC_IOFLAG_UNABLE_TO_WRITE = 0x1 << 3,
STUC_IOFLAG_WRONG_FILE_TYPE = 0x1 << 4,
STUC_IOFLAG_COUNT = 5,
} fileIOFlags;

#ifndef STUC_MALLOC
	#define STUC_MALLOC malloc_set0
	#define malloc_set0(n) calloc((n), 1); 
	#include <stdlib.h>
#endif // STUC_MALLOC

#ifndef STUC_FREE
	#define STUC_FREE free
	#include <stdlib.h>
#endif // STUC_FREE

#ifndef STUC_ASSERT
	#define STUC_ASSERT assert
	#define STUC_SOFT_ASSERT(uvijet) ((uvijet) ? 1 : 0 * printf("\x1b[1;31mAssert failed\x1b[0;37m in file: %s:%d %s: `%s`\n", __FILE__, __LINE__, __func__, #uvijet))
	#include <assert.h>
#else
	#define STUC_SOFT_ASSERT STUC_ASSERT
#endif // STUC_ASSERT

#ifndef NO_STDIO
	#include <stdio.h>	
#else 
	#undef STUC_SOFT_ASSERT
	#define STUC_SOFT_ASSERT STUC_ASSERT
	#undef MAT_PRINT
	#undef NN_PRINT
#endif // NO_STDIO


#endif // STUC_H
