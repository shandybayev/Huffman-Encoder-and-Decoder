#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node {
    float freq;
    char symbol;
    struct node *left, *right;
} Node;

typedef struct priorityqueue
{
    int size;
    int capacity;
    struct node **array;
} PQ;

// I implemented a basic closed hash table for you to use. I use this for file
// interactions like reading and writing. You may use it however you please
typedef struct huffmancodetable
{
	int	size;
	float *freqs;
	char **codes;
} HCTable;

// These are called function declarations. They are single lines at the top of
// the file to reference functions. It is a common practice in C to list your
// function declarations at the top of your file, then have your full main
// function, then the filled in function definitions at the end of your file.
// You should not need to change any of these top 5 declarations, but feel free
// to
void encode( FILE *plaintext, FILE *codetable, FILE *encodetxt );
void decode( FILE *codetable, FILE *encodetxt, FILE *decodetxt );
HCTable	*initializeHuffmanCodeTable();
void insertToHCTable(HCTable *hct, char c, float f, char *code);
void deleteHuffmanCodeTable( HCTable *hct );
// This is the bare minimum you need for implementing a priority queue and BT
// You will need to write more functions
PQ *initializePriorityQueue(int capacity);
char *printArray(int arr[], int n);
void swapQueueNodes(Node **a, Node **b);
void upQueue(PQ *pq, int index);
int isSizeOne(PQ *pq);
void insert(PQ *pq, Node *node);
void buildQueue(PQ *pq);
int isLeaf(Node *root);
PQ *initializeAndBuildQueue(float freq[], char symbols[], int size);
Node *buildHuffmanTree(float freq[], char symbols[], int size);
void printCodes(Node *root, int arr[], int top, HCTable *hct);
void HuffmanCodes(float freq[], char symbols[], int size, HCTable *hct);
Node *initializeNode(float freq, char symbol);
Node *extractMin(PQ *pq);
void append(char* s, char c);

// My main function only handles the preparation steps of the code. You will not
// need to edit this function. encode and decode is where you need to write code
int main( int argc, char **argv) {
	// I list all my variables at the top of my functions. I do this for many
	// reasons:
	//      1. It makes my code easier to read
	//      2. It makes my code easier to debug
	//      3. I have less segfaults and bugs in my code since I know exactly
	//           what memory and pointers I have available
	//      4. I write smarter and more efficient code since I can see easily
	//           how many variables I'm using
	//      5. I also get a chance to comment and describe each variable and
	//           what they do for the function up here
	char *mode;            // Encode/Decode argument
	char *plaintext_path;  // plainText.txt file path argument
	FILE *plaintext;       // plainText.txt file pointer
	char *codetable_path;  // codeTable.txt file path argument
	FILE *codetable;       // codeTable.txt file pointer
	char *encodetxt_path;  // encodedText.txt file path argument
	FILE *encodetxt;       // encodedText.txt file pointer
	char *decodetxt_path;  // decodedText.txt file path argument
	FILE *decodetxt;       // decodedText.txt file pointer
	// For this assignment, try to avoid declaring any variables below this line
	// Hopefully, this simple challenge will force you to think about your data
	// structures more. You may find you will spend less time debugging

	if( argc < 5 ) {
		printf("Not Enough Arguments Supplied\n");
		return -1;
	}

	mode = argv[1];

	if( strcmp(mode, "encode") == 0 ) {
		plaintext_path = argv[2];
		codetable_path = argv[3];
		encodetxt_path = argv[4];

		plaintext = fopen(plaintext_path, "r"); // Open for reading
		codetable = fopen(codetable_path, "w"); // Open for writing
		encodetxt = fopen(encodetxt_path, "w"); // Open for writing

		if( (plaintext == NULL) ||
		    (codetable == NULL) ||
			(encodetxt == NULL) ) {
			printf("File Reading Error\n");
			return -1;
		}

		encode( plaintext, codetable, encodetxt );

		fclose( plaintext );
		fclose( codetable );
		fclose( encodetxt );

	}
	else if( strcmp(mode, "decode") == 0 ) {
		codetable_path = argv[2];
		encodetxt_path = argv[3];
		decodetxt_path = argv[4];

		codetable = fopen(codetable_path, "r"); // Open for reading
		encodetxt = fopen(encodetxt_path, "r"); // Open for reading
		decodetxt = fopen(decodetxt_path, "w"); // Open for writing

		if( (codetable == NULL) ||
			(encodetxt == NULL) ||
			(decodetxt == NULL) ) {
			printf("File Reading Error\n");
			return -1;
		}

		decode( codetable, encodetxt, decodetxt );

		fclose( codetable );
		fclose( encodetxt );
		fclose( decodetxt );
	}
	else {
		printf("Not A Valid Mode\n");
		return -1;
	}
}

void encode(FILE *plaintext, FILE *codetable, FILE *encodetxt) {
	// As stated in main, I list all my variable declarations at the top of my
	// functions. The list is incomplete. Feel free to add, remove, or change
	// these
	PQ *pq;     // A priority queue for the Huffman Coding algorithm
	HCTable	*hct;    // A closed hash table for storing huffman values
	char c;       // Temporary int/char used for iteration
  int i;
  int len = 0;
  int total_len = 0;
  int compressed_size = 0;
  int index = 0;
  float temp_freq[128];

	hct = initializeHuffmanCodeTable();

  for (i = 0; i < 128; ++i) {
    temp_freq[i] = 0;
  }

	// This loop will read the file character by character
	while ((c = fgetc(plaintext)) != EOF && c != '\n') {
    temp_freq[c]++;
    total_len++;
	}

  for (i = 0; i < 128; ++i) {
    if (temp_freq[i] != 0) {
      len++;
    }
  }

  char arr[len];
  float freq[len];

  for (i = 0; i < 128; ++i) {
    if (temp_freq[i] != 0) {
      freq[index] = temp_freq[i] / total_len;
      arr[index] = i;
      temp_freq[i] = 0;
      index++;
    }
  }

  int size = sizeof(arr) / sizeof(arr[0]);

  HuffmanCodes(freq, arr, size, hct);

  // This encodes the plain text file using the huffman code values and writes
	// it to the encoding file
  fseek(plaintext, 0, SEEK_SET);
	while((c = fgetc(plaintext)) != EOF && c!='\n') {
    fprintf(encodetxt, "%s", hct->codes[c]);
    compressed_size += strlen(hct->codes[c]);
  }

	// This writes the codetable file
	for(i = 0; i < 128; ++i) {
		if(hct->freqs[i] != 0) {
      fprintf(codetable, "%c,%s,%.2f\n", i, hct->codes[i], hct->freqs[i]);
    }
	}

  printf("Original: %d bits\n", total_len * 8);
  printf("Compressed: %d bits\n", compressed_size);
  printf("Compression Ratio: %.2f%%\n", (float)compressed_size / ((float)total_len * 8) * 100);

	deleteHuffmanCodeTable(hct);
}

void decode(FILE *codetable, FILE *encodetxt, FILE *decodetxt) {
	HCTable	*hct;        // A closed hash table used for storing huffman values
	int ce, cd, i;   // Temporary int/char used for iteration
	float f;           // A temporary float variable used for iteration
	char buf[128], temp_code[50];    // A temporary buffer used for iteration
  char c;

	hct = initializeHuffmanCodeTable();

	// This reads the codetable file into a huffman code table. You can use this
	// table for the remainder of the decoding
	while (fscanf(codetable, "%c,%[^,],%f\n", &c, buf, &f) != EOF) {
		if (c < 32)
			continue;

		insertToHCTable(hct, c, f, buf);
	}

	cd = -1;
  temp_code[0] = '\0';

  while ((ce = fgetc(encodetxt)) != EOF) {
    append(temp_code, ce);

    for (i = 0; i < 128; ++i) {
      if (hct->freqs[i] != 0) {
        if (strcmp(temp_code, hct->codes[i]) == 0) {
          cd = i;
          temp_code[0] = '\0';
        }
      }
    }

    // This if statement will print the decoded character to the decode file
    if (cd != -1) {
      fputc(cd, decodetxt);
      cd = -1;
    }
  }

	deleteHuffmanCodeTable(hct);
}

HCTable *initializeHuffmanCodeTable() {
	// This function is self explanatory. You've seen all this before
	HCTable *hct;
	int i;

	hct = malloc( sizeof(HCTable) );
	hct->size = 128;
	hct->codes = malloc( hct->size * sizeof(char *) );
	hct->freqs = malloc( hct->size * sizeof(float) );

	for( i = 0; i < hct->size; i++ )
		hct->freqs[i] = 0;

	return hct;
}

void insertToHCTable(HCTable *hct, char c, float f, char *code) {
	if (hct->freqs[c] != 0)
		free(hct->codes[c]);

	hct->freqs[c] = f;
	hct->codes[c] = malloc( sizeof(char) * strlen(code) + 1 );
	strcpy(hct->codes[c], code);
}

void deleteHuffmanCodeTable( HCTable *hct ) {
	int i;

	for( i = 0; i < hct->size; i++ )
		if( hct->freqs[i] != 0 )
			free( hct->codes[i] );

	free( hct->codes );
	free( hct->freqs );
	free( hct );
}

PQ *initializePriorityQueue(int capacity) {
  PQ *myPriorityQueue = malloc(sizeof(PQ));
  myPriorityQueue->size = 0;
  myPriorityQueue->capacity = capacity;
  myPriorityQueue->array = malloc(sizeof(Node*) * myPriorityQueue->capacity);

  return myPriorityQueue;
}

char *printArray(int arr[], int n) {
  char *code;
  code = malloc(sizeof(char) * (n + 1));

  int i;
  for (i = 0; i < n; ++i) {
    code[i] = arr[i] + '0';
  }

  return code;
}

void swapQueueNodes(Node **a, Node **b) {
  Node *temp = *a;
  *a = *b;
  *b = temp;
}

void upQueue(PQ *pq, int index) {
  int smallest = index;
  int left = 2 * index + 1;
  int right = 2 * index + 2;

  if (left < pq->size && pq->array[left]->freq < pq->array[smallest]->freq)
    smallest = left;

  if (right < pq->size && pq->array[right]->freq < pq->array[smallest]->freq)
    smallest = right;

  if (smallest != index) {
    swapQueueNodes(&pq->array[smallest], &pq->array[index]);
    upQueue(pq, smallest);
  }
}

int isSizeOne(PQ *pq) {
  return (pq->size == 1);
}

void insert(PQ *pq, Node *node) {
  pq->array[pq->size] = node;
  upQueue(pq, pq->size);
  pq->size++;
}

void buildQueue(PQ *pq) {
  int n = pq->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i) {
    upQueue(pq, i);
  }
}

int isLeaf(Node *root) {
  return !(root->left) && !(root->right);
}

PQ *initializeAndBuildQueue(float freq[], char symbols[], int size) {
  PQ *pq = initializePriorityQueue(size);

  for (int i = 0; i < size; ++i) {
    pq->array[i] = initializeNode(freq[i], symbols[i]);
  }

  pq->size = size;
  buildQueue(pq);

  return pq;
}

Node *buildHuffmanTree(float freq[], char symbols[], int size) {
  Node *left, *right, *top;

  PQ *pq = initializeAndBuildQueue(freq, symbols, size);

  while (!isSizeOne(pq)) {
    left = extractMin(pq);
    right = extractMin(pq);

    top = initializeNode(left->freq + right->freq, '$');
    top->left = left;
    top->right = right;

    insert(pq, top);
  }

  return extractMin(pq);
}

void printCodes(Node *root, int arr[], int top, HCTable *hct) {
  if (root->left) {
    arr[top] = 0;
    printCodes(root->left, arr, top + 1, hct);
  }

  if (root->right) {
    arr[top] = 1;
    printCodes(root->right, arr, top + 1, hct);
  }

  if (isLeaf(root)) {
    char *code;
    code = malloc(sizeof(char) * (top + 1));
    code = printArray(arr, top);

    insertToHCTable(hct, root->symbol, root->freq, code);
  }
}

void HuffmanCodes(float freq[], char symbols[], int size, HCTable *hct) {
  Node *root = buildHuffmanTree(freq, symbols, size);

  int arr[128], top = 0;

  printCodes(root, arr, top, hct);
}

Node *initializeNode(float freq, char symbol) {
  Node *myNode = malloc(sizeof(Node));
  myNode->freq = freq;
  myNode->symbol = symbol;
  myNode->left = NULL;
  myNode->right = NULL;

  return myNode;
}

Node *extractMin(PQ *pq) {
  Node *temp = pq->array[0];
  pq->array[0] = pq->array[pq->size - 1];

  --pq->size;
  upQueue(pq, 0);

  return temp;
}

void append(char* s, char c) {
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}