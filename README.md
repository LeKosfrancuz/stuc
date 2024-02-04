# stuc (Strojno Učenje u C-u)
Machine Learning library and application

## Operacije s matricama

Definicija matrice sadrži broj *redaka*, *stupaca* i *stride*[^1]

> Napomena: *stride* je najčešće jednak broju stupaca

Npr. Sljedeća matrica ima 2 redka, 3 stupca i stride je 3
|1|2|3|
|-|-|-|
|4|5|6|

Dok ova matrica (ako ne želimo uključiti x-eve) ima 2 redka, 3 stupca i stride je 4
|1|2|3|x|
|-|-|-|-|
|4|5|6|x|

> Matrice će biti jednake jer sadrže iste podatke.
```c
typedef struct {
	size_t rows;
	size_t cols;
	size_t stride;
	float_t *el;
} Stuc_mat;
```

```c
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
 *	Nova matrica koja sadrži 1 redak podataka
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

/* Dealocira mjesto za spremanje elemenata matrice (a)
 * 
 * Napomena: Neće alocirati mjesto koje zauzima struktura matrice
 *	     to je na programeru. Dealocira samo elemente.
 */
void stuc_matFree(Stuc_mat a);

```

## Operacije s neuronskom mrežom
```c
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
	size_t  layerCount;
	Stuc_nnLayer* layers; // count + 1, jer je 0. za input
} Stuc_nn;
```

```c
void    stuc_nnForward(Stuc_nn nn);
void    stuc_nnFill(Stuc_nn nn, float_t number);
void    stuc_nnRand(Stuc_nn nn, float_t low, float_t high);
void    stuc_nnPrint(Stuc_nn nn, char *name);
float_t stuc_nnCost(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput);

Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);

void    stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gdMap, 
                               Stuc_mat tInput, Stuc_mat tOutput, float_t boost);

Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);

void    stuc_nnFiniteDiffNoAlloc(Stuc_nn nn, Stuc_nn fd, float_t eps, 
				 Stuc_mat tInput, Stuc_mat tOutput);

void    stuc_nnApplyDiff(Stuc_nn nn, Stuc_nn fd, float_t learningRate);

Stuc_nn stuc_nnAlloc(Stuc_activationFunction *aktivacije,
		     size_t *arhitektura, size_t arhCount);

void    stuc_setActivation(Stuc_activationFunction *aktivacije, size_t aktCount, 
                           Stuc_activationFunction aktivacija);

void    stuc_nnFree(Stuc_nn nn); 
```


## Spremanje u file

```c
uint8_t stuc_nnSaveToFile(Stuc_nn nn, const char *filePath);
uint8_t stuc_nnLoadFromFile(Stuc_nn *nn, const char *filePath);
void    stuc_printIOFlags(uint8_t flags);

typedef enum {
STUC_IOFLAG_FILE_ERROR      = 0x1 << 0,
STUC_IOFLAG_TYPE_MISMATCH   = 0x1 << 1,
STUC_IOFLAG_UNABLE_TO_READ  = 0x1 << 2,
STUC_IOFLAG_UNABLE_TO_WRITE = 0x1 << 3,
STUC_IOFLAG_WRONG_FILE_TYPE = 0x1 << 4,
STUC_IOFLAG_COUNT = 5,
} fileIOFlags;
```

[^1]: Udaljenost između dva redka (mjerena u broju elemenata)
