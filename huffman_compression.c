/*******************************************************************************
*Author: Mary Rizkalla
*Date:
*
* A script to compress and decompress a txt file utilising the Huffman encoding 
* algorithim.
*
*******************************************************************************/

/*******************************************************************************
 * Header files
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Structures
*******************************************************************************/
struct node
{
	int freq;
	int charIndex;
	struct node *largerFreq;
	struct node *smallerFreq;
};
typedef struct node node_t;

struct key_value_pair
{
	char uniqueChar;
	char *hCode;
};
typedef struct key_value_pair key_value_pair_t;

/*******************************************************************************
 * Function prototypes
*******************************************************************************/
int printMenu(void);

int selectChoice(void);

int compressFile(void);

int getInputString(char *fileName, char *charArray);

int uniqueCharsFreqCounter(char *inputString, int inputStringLength, 
	char *uniqueChars, int *charFreq);

void createHuffmanTree(node_t *nodeArray[], const int noOfUniqueChars);

int nextSmallestIndex(node_t *array[], int currentSmallestIndex, 
	const int noOfUniqueChars);

void preorderGenerationOfHuffmanCode(node_t *tree, int huffmanCode[], 
	int codeIndex, const int noOfUniqueChars, key_value_pair_t *codeArray[]);

int encodeString(char *inputString, int inputStringLength, char *compressedOut, 
	const int noOfUniqueChars, key_value_pair_t *codeArray[]);

int decodeString(char *compressedIn, int compressedInLength, 
	char *decompressedOut, const int noOfUniqueChars, 
	key_value_pair_t *codeArray[]);

int outputCompressedString(char *compressedString, int compressedStringLength, 
	char *outputFileName);

int outputCodes(key_value_pair_t *codeArray[], const int noOfUniqueChars, 
	const int stringLength, char *outputFileName);

int decompressFile(void);

int generateCompressedFileName(char *inputFileName, int inputFileNameLength, 
	char *compressedFileName);

int generateCodeFileName(char *compressedFileName, int compressedFileNameLength, 
	char *codeFileName);

int generateDecompressedFileName(char *compressedFileName, 
	int compressedFileNameLength, char *decompressedFileName);

int inputCompressedFileToBinString(char *compressedFileName, 
	char *compressedInputInBin);

char reverseBitsInByte(char byte);

/*******************************************************************************
 * Main
*******************************************************************************/
int main(void)
{
	printf("\n-----------------Huffman Compression-----------------\n");
	
	while(1)
	{
		printMenu();
		int i = selectChoice();
		if(i == 3)
		{
			return 0;
		}
	}
	
	return 0;
}

/*******************************************************************************
 * This function prints user menu.
*******************************************************************************/
int printMenu(void)
{
	printf("\n1. Compress file.\n");
	printf("2. Decompress file.\n");
	printf("3. Exit.\n\n");
	printf("Enter an option between 1-3:\n");

	return 0;
}

/*******************************************************************************
 * This function takes the user selection using switch case.
*******************************************************************************/
int selectChoice(void)
{
	char input[200];
	fgets(input, sizeof(input), stdin);
	char choice = '0';
	if(strlen(input) == 2)
	{
		choice  = input[0];
	}
	switch(choice)
	{
		case '1':	compressFile();
				 	break;
		case '2':	decompressFile();
					break;
		case '3':	return 3;

		default :printf("Invalid option.\n");
	}

	return 0;
}

/*******************************************************************************
 * This function uses huffam code to compress user selected file.
*******************************************************************************/
int compressFile(void)
{

	printf("Enter the name of the file you wish to compress: \n");
	char inputFileName[100];
	fgets(inputFileName, sizeof(inputFileName), stdin);
	inputFileName[strlen(inputFileName)-1] = '\0'; /*clear \n from input*/


	/****store contents from input file to char array.****/
	char *string = malloc(sizeof(char)*2000000);
	if(string == NULL)
	{
		fprintf(stderr, "Cannot open file. Memory allocation error.");
		return 1;
	}
	int stringLength = getInputString(inputFileName, string);
	if(stringLength == -1)
	{
		return 1;
	}


	/****store the unique chars and corresponding frequencies in arrays****/
	char *uniqueChars = malloc(sizeof(char)*stringLength);
	*uniqueChars = '\0';
	int *charFreq = malloc(sizeof(int)*stringLength);

	const int noOfUniqueChars = uniqueCharsFreqCounter(string, stringLength, 
		uniqueChars, charFreq);

	/*printf(" - %d unique characters\n", noOfUniqueChars);*/


	/**initialise nodeArray to store a node_t variable for each unique char.***/
	node_t *nodeArray[noOfUniqueChars];
	int i;
	for(i=0;i<noOfUniqueChars;i++){
		nodeArray[i] = malloc(sizeof(node_t));
		(*nodeArray[i]).freq = charFreq[i];
		(*nodeArray[i]).charIndex = i;
		(*nodeArray[i]).largerFreq = NULL;
		(*nodeArray[i]).smallerFreq = NULL;
	}


	/****initialise codeArray to store a key_value_pair_t variable storing the 
	index and huffman code for each unique char.****/
	key_value_pair_t *codeArray[noOfUniqueChars];
	int ii;
	for(ii=0;ii<noOfUniqueChars;ii++)
	{
		codeArray[ii] = malloc(sizeof(key_value_pair_t));
		(*codeArray[ii]).uniqueChar = uniqueChars[ii];
		(*codeArray[ii]).hCode = NULL;
	}


	/****create huffman tree****/
	createHuffmanTree(nodeArray, noOfUniqueChars);


	/****create huffman codes from binary tree using preorder algorithim.****/
	/*prepare variables for preorder function.*/
	int codeIndex = 0;
	int huffmanCode[noOfUniqueChars];
	int indexOfTreeTop;
	int k;
	for(k=0;k<noOfUniqueChars;k++)
	{
		if((*nodeArray[k]).freq != -1){
			indexOfTreeTop = k;
		}
	}
	/*generates and stores huffman codes as key-value pairs in pairArray*/
	printf("\n>Generating Huffman Codes\n...\n");
	preorderGenerationOfHuffmanCode(nodeArray[indexOfTreeTop], huffmanCode, 
		codeIndex, noOfUniqueChars, codeArray);
	
	/*print huffman codes*/
	int l;
	for(l=0;l<noOfUniqueChars;l++)
	{
		printf("c: %c, code: %s\n", (*codeArray[l]).uniqueChar, 
			(*codeArray[l]).hCode);
	}
	

	/****compress input string with created values****/
	printf("\n>Compressing %s\n...\n", inputFileName);
	int maxLengthOfHCode = noOfUniqueChars - 1;/*max possible len of code*/
	char *compressedOut = malloc(sizeof(char)*maxLengthOfHCode*stringLength);
	*compressedOut = '\0';
	int compressedOutLength = encodeString(string, stringLength, compressedOut, 
		noOfUniqueChars, codeArray);


	/****output compressed string into txt file****/
	char *compressedFileName = malloc((sizeof(char)*strlen(inputFileName))+11);
	generateCompressedFileName(inputFileName, strlen(inputFileName), 
		compressedFileName);
	if(!outputCompressedString(compressedOut, compressedOutLength, 
		compressedFileName))
	{
		printf("Compressed file written as %s\n", compressedFileName);
	}


	/****output code Array, noOfUniqueChars and original string length.****/
	char *compressedCodeFileName=malloc(sizeof(char)*(strlen(compressedFileName)
										+6));
	generateCodeFileName(compressedFileName, strlen(compressedFileName), 
		compressedCodeFileName);
	if(!outputCodes(codeArray, noOfUniqueChars, stringLength, 
		compressedCodeFileName))
	{
		printf("Dependent huffman codes file written as %s\n", 
			compressedCodeFileName);
	}

	printf("\n>Output complete.\n");

	return 0;
}

/*******************************************************************************
 * This function inputs the characters of the input file into a char array.
*******************************************************************************/
int getInputString(char *fileName, char *charArray)
{
	printf("\n>Searching for %s  \n...\n", fileName);
	FILE *fpIn;
	fpIn = fopen(fileName, "r");
	if(fpIn == NULL)
	{
		fprintf(stderr, "Cannot open %s. File not found.\n", fileName);
		return -1;
	}else
	{
		printf("File Located.\n");
	}
	fseek(fpIn, 0, SEEK_END);
	int sz = ftell(fpIn);

	printf("\n>File Data:\n - file size = %d bytes\n", sz);

	fseek(fpIn,0, SEEK_SET);

	int i;
	for(i=0; i<sz; i++){
		charArray[i] = fgetc(fpIn);
	}

	fclose(fpIn);

	return strlen(charArray);
}

/*******************************************************************************
 * This function countes the frequency of unique characters.
*******************************************************************************/
int uniqueCharsFreqCounter(char *inputString, int inputStringLength, 
	char *uniqueChars, int *charFreq)
{
	int countUniqueChars = 0;
	int i;
	for(i=0; i<inputStringLength; i++)
	{
		int isUnique = 1; 
		int j;
		for(j=0; j<countUniqueChars; j++)
		{
			if(inputString[i] == uniqueChars[j])
			{
				isUnique = 0; 
				charFreq[j]++;
				break;
			}
		}
		if(isUnique)
		{
			uniqueChars[countUniqueChars] = inputString[i];
			countUniqueChars++;
			charFreq[countUniqueChars -1 ] = 1;
		}	
	}
	return countUniqueChars;
}

/*******************************************************************************
 * This function creates the huffman tree.
*******************************************************************************/
void createHuffmanTree(node_t *nodeArray[], const int noOfUniqueChars)
{

	int smallest, secondSmall;
	node_t *tmp;
	int j;
	for(j=0; j<(noOfUniqueChars-1); j++){
		smallest = nextSmallestIndex(nodeArray, -1, noOfUniqueChars);
		secondSmall = nextSmallestIndex(nodeArray,smallest,noOfUniqueChars);
		tmp = nodeArray[smallest];
		nodeArray[smallest] = malloc(sizeof(node_t));
		(*nodeArray[smallest]).freq=(*nodeArray[secondSmall]).freq +(*tmp).freq;
		(*nodeArray[smallest]).charIndex = -1; /*added nodes take index of -1 as 
		a huffman code is not to be generated for them.*/
		(*nodeArray[smallest]).largerFreq = nodeArray[secondSmall];
		(*nodeArray[smallest]).smallerFreq = tmp;
		(*nodeArray[secondSmall]).freq = -1;
	}
}

/*******************************************************************************
 * This function finds the index of the next smallest node value.
*******************************************************************************/
int nextSmallestIndex(node_t *nodeArray[], int currentSmallestIndex, 
	const int noOfUniqueChars)
{

	int nextSmallest; /*temporary variable holding the next smallest freq.*/
	int nextSmallestIndex;

	/*if currentSmallest value in array is at index 0, initialise nextSmallest 
	to index 1, else to 0*/
	if(currentSmallestIndex == 0)
	{   
		nextSmallest = (*nodeArray[1]).freq;
		nextSmallestIndex = 1;
	}
	else
	{
		nextSmallest = (*nodeArray[0]).freq;
		nextSmallestIndex = 0;
	}
	
	int i;
	for(i=0;i<noOfUniqueChars;i++)
	{

		/*ensure the currnet smallest index is not included in the comparison 
		for the next smallest index*/
		if(currentSmallestIndex == i)
		{
			i++;
			if(i>(noOfUniqueChars-1))
			{
				return nextSmallestIndex;
			}/*ensure index does not overflow*/
		}

		/*ensure nextSmallest freq is not holding -1 as -1 is assigned to nodes
		which have been parented in the tree. ie. not to be included again in 
		the comparison.*/
		while(nextSmallest == -1)
		{
			i++;
			if(currentSmallestIndex == i)
			{
				i++;
			}
			if(i>(noOfUniqueChars-1))
			{
				return nextSmallestIndex;
			}
			nextSmallest = (*nodeArray[i]).freq;
			nextSmallestIndex = i;
		}

		/*ensure the current index which is to be compared with the temporary 
		valiable is not holding -1*/
		while((*nodeArray[i]).freq == -1)
		{
			i++;
			if(currentSmallestIndex == i){i++;}
			if(i>(noOfUniqueChars-1)){return nextSmallestIndex;}
		}

		/*compare value at current index with temporary variable*/
		if((*nodeArray[i]).freq < nextSmallest)
		{
			nextSmallest = (*nodeArray[i]).freq;
			nextSmallestIndex = i;
		}
	}

	return nextSmallestIndex;
}

/*******************************************************************************
 * This function uses the preorder tree algorithim (huffman code) to fill the
 * code array.   
*******************************************************************************/
void preorderGenerationOfHuffmanCode(node_t *tree, int huffmanCode[], 
	int codeIndex, const int noOfUniqueChars, key_value_pair_t *codeArray[])
{
	if((*tree).smallerFreq != NULL)
	{
		huffmanCode[codeIndex] = 1;
		preorderGenerationOfHuffmanCode((*tree).smallerFreq, huffmanCode, 
			codeIndex + 1, noOfUniqueChars, codeArray);
	}
	if((*tree).largerFreq != NULL)
	{
		huffmanCode[codeIndex] = 0;
		preorderGenerationOfHuffmanCode((*tree).largerFreq, huffmanCode, 
			codeIndex + 1, noOfUniqueChars, codeArray);
	}

	/*if node is a leaf node*/
	if((*tree).charIndex != -1)
	{

		/*char array to store hcode*/
		char hc[codeIndex + 1];

		if(noOfUniqueChars == 1)
		{
			hc[0] = '0';
		}
		else
		{
			int i;
			for(i=0; i<codeIndex; i++)
			{
				sprintf(&hc[i], "%d", huffmanCode[i]); 
			} 
		}
		/*input null terminator*/
		hc[codeIndex + 1] = '\0'; 

		/*if charIndex of codeArray has not been set, set to huffman code.*/
		if((*codeArray[(*tree).charIndex]).hCode == NULL)
		{
			(*codeArray[(*tree).charIndex]).hCode = malloc(sizeof(char)*
														  (codeIndex + 1));
			strcpy((*codeArray[(*tree).charIndex]).hCode, &hc[0]);
		}
	}

	return;
}

/*******************************************************************************
 * This function encodes the input string.
*******************************************************************************/
int encodeString(char *inputString, int inputStringLength, char *compressedOut, 
	const int noOfUniqueChars, key_value_pair_t *codeArray[])
{ 
	
	int j;
	for(j=0; j<inputStringLength; j++)
	{
		int i;
		for(i=0;i<noOfUniqueChars; i++)
		{
			if(inputString[j] == (*codeArray[i]).uniqueChar)
			{
				strcat(compressedOut, (*codeArray[i]).hCode);
			}
		}
	}
	return strlen(compressedOut);
}

/*******************************************************************************
 * This function decodes the compressed input file.
*******************************************************************************/
int decodeString(char *compressedIn, int compressedInLength, 
	char *decompressedOut, const int noOfUniqueChars, 
	key_value_pair_t *codeArray[])
{

	char *tmpArray = malloc(sizeof(char)*(noOfUniqueChars-1));
	*tmpArray = '\0';
	*decompressedOut = '\0';
	char charToCompare[2];
	char charToAppend[2];
	int j;
	for(j=0; j<compressedInLength; j++)
	{
		charToCompare[0] = compressedIn[j];
		strcat(tmpArray, charToCompare);
		int i;
		for(i=0; i<noOfUniqueChars; i++)
		{ 	
			if(strcmp(tmpArray, (*codeArray[i]).hCode) == 0)
			{
				charToAppend[0] =  (*codeArray[i]).uniqueChar;
				strcat(decompressedOut, charToAppend);
				tmpArray = realloc(tmpArray, sizeof(char)*(noOfUniqueChars-1));
				*tmpArray = '\0';
			}
		}
	}

	return strlen(decompressedOut);
}

/*******************************************************************************
 * This function outputs the compressed string to a file.
*******************************************************************************/
int outputCompressedString(char *compressedString, int compressedStringLength, 
	char *outputFileName)
{
	FILE *fpOut;
	fpOut = fopen(outputFileName, "w");
	if(fpOut == NULL)
	{
		fprintf(stderr, "output file cannot be opened.\n");
		return 1;
	}
	char buffer = '\0';
	int count = 0;
	char currentBit = '\0';
	int e;
	for(e=0; e<compressedStringLength; e++)
	{
		if(compressedString[e] == '0')
		{
			currentBit = '\0';
		}else if(compressedString[e] == '1')
		{
			currentBit = ('1' - '0');
		}
		buffer <<= 1;
		buffer |= currentBit;
		count++;
		if(count == 8)
		{
			fwrite(&buffer, sizeof(char), 1, fpOut);
			count = 0;
			buffer = '\0';
		}
		if(e == (compressedStringLength - 1) && (count != 0))
		{
			buffer <<= (8 - count);
			fwrite(&buffer, sizeof(char), 1, fpOut);
			count = 0;
			buffer = '\0';
		}
	}
	fclose(fpOut);

	return 0;
}

/*******************************************************************************
 * This function outputs the codes file to be used for decompression.
*******************************************************************************/
int outputCodes(key_value_pair_t *codeArray[], int noOfUniqueChars, 
	int stringLength, char *outputFileName)
{
	FILE *fcodes;
	fcodes = fopen(outputFileName,"w");
	if(fcodes == NULL){
		fprintf(stderr, "Cannot open file.");
		return 1;
	}
	fwrite(&noOfUniqueChars, sizeof(int), 1, fcodes);
	fwrite(&stringLength, sizeof(int), 1, fcodes);

	int codeLengths[noOfUniqueChars];
	int i;
	for(i=0; i<noOfUniqueChars; i++){
		codeLengths[i] = strlen((*codeArray[i]).hCode);
		fwrite(&codeLengths[i], sizeof(int), 1, fcodes);
	}

	int j;
	for(j=0; j<noOfUniqueChars; j++){
		fwrite(&((*codeArray[j]).uniqueChar), sizeof(char), 1, fcodes);
	}

	int k;
	for(k=0; k<noOfUniqueChars; k++){
		fwrite((*codeArray[k]).hCode, codeLengths[k]*sizeof(char), 1, fcodes);
	}

	fclose(fcodes);
	return 0;
}

/*******************************************************************************
 * This function decompresses the user selected file. Outputs decompressed file.
*******************************************************************************/
int decompressFile(void)
{
	/*User enters the name of the file they wish to decompress*/
	printf("Enter the name of the file you wish to decompress: \n");
	char compressedFileName[100];
	fgets(compressedFileName, sizeof(compressedFileName), stdin);
	compressedFileName[strlen(compressedFileName)-1] = '\0'; /*clear \n*/
	printf("\n>Searching for %s  \n...\n", compressedFileName);

	/*open selected file*/
	FILE *fpt;
	fpt = fopen(compressedFileName, "r");
	if(fpt == NULL)
	{
		fprintf(stderr,"Cannot open %s. File not found.\n", compressedFileName);
		return 1;
	}else{
		printf("File located.\n");
	}

	/****input noOfUniqueChars, original txt Length and codes.****/
	int noOfUniqueCharsIn = 0;
	int stringLengthIn = 0;
	/*generate code file name*/
	char *codeFileName = malloc((sizeof(char)*strlen(compressedFileName))+6);
	generateCodeFileName(compressedFileName, strlen(compressedFileName), 
						codeFileName);
	printf("\n>Searching for %s  \n...\n", codeFileName);

	/*open code file*/
	FILE *fcodesIn;
	fcodesIn = fopen(codeFileName, "r");
	if(fcodesIn == NULL)
	{
		fprintf(stderr, "Cannot open %s. File not found.\n", codeFileName);
		return 1;
	}else
	{
		printf("File located.\n");
	}
	printf("\n>Fetching Huffman Codes Data\n...\n");
	fread(&noOfUniqueCharsIn, sizeof(int), 1, fcodesIn);
	fread(&stringLengthIn, sizeof(int), 1, fcodesIn);

	key_value_pair_t *codeArrayIn[noOfUniqueCharsIn];
	int il;
	for(il=0;il<noOfUniqueCharsIn;il++){
		codeArrayIn[il] = malloc(sizeof(key_value_pair_t));
		(*codeArrayIn[il]).uniqueChar = ' ';
		(*codeArrayIn[il]).hCode = NULL;
	}

	int codeLengths[noOfUniqueCharsIn];
	int i;
	for(i=0; i<noOfUniqueCharsIn; i++){
		fread(&codeLengths[i], sizeof(int), 1, fcodesIn);
	}

	int j;
	for(j=0; j<noOfUniqueCharsIn; j++){
		fread(&((*codeArrayIn[j]).uniqueChar), sizeof(char), 1, fcodesIn);
	}

	int k;
	for(k=0; k<noOfUniqueCharsIn; k++){
		(*codeArrayIn[k]).hCode = malloc(codeLengths[k]*sizeof(char));
		fread((*codeArrayIn[k]).hCode, codeLengths[k]*sizeof(char),1,fcodesIn);
	}

	fclose(fcodesIn);

	int f;
	for(f=0;f<noOfUniqueCharsIn;f++)
	{
		printf("c: %c, code: %s\n", (*codeArrayIn[f]).uniqueChar, 
			  (*codeArrayIn[f]).hCode);
	}

	printf("\n>Decompressing %s\n...\n", compressedFileName);
	
	/****input compressed txt file into a char array of binary values.****/
	int maxLengthOfHCodeIn = noOfUniqueCharsIn-1;
	char *compressedInputInBin = malloc(sizeof(char)*maxLengthOfHCodeIn*
									   stringLengthIn);
	if(compressedInputInBin == NULL)
	{
		fprintf(stderr, "Cannot input file. Memory allocation error.\n");
		return 1;
	}
	int compressedInputInBinLen = inputCompressedFileToBinString(
								  compressedFileName, compressedInputInBin);

	/****decompress the binary array input****/
	char *decompressedOutString = malloc(sizeof(char)*compressedInputInBinLen);
	*decompressedOutString = '\0';

	decodeString(compressedInputInBin, compressedInputInBinLen, 
				decompressedOutString, noOfUniqueCharsIn, codeArrayIn);

	decompressedOutString[stringLengthIn] = '\0';

	/****output decompressed string into txt file****/
	char *decompressedFileName = malloc(sizeof(char)*(strlen(compressedFileName)
										+3));	
	generateDecompressedFileName(compressedFileName, strlen(compressedFileName), 
								decompressedFileName);
	FILE *fpOutDecomp;
	fpOutDecomp = fopen(decompressedFileName,"w");
	if(fpOutDecomp == NULL)
	{
		fprintf(stderr, "Cannot open output file.\n");
		return 1;
	}
	fwrite(decompressedOutString, sizeof(char)*strlen(decompressedOutString), 1, 
		  fpOutDecomp);
	fclose(fpOutDecomp);
	
	printf("Decompressed file written as %s\n", decompressedFileName);
	printf("\n>Output complete.\n");
	return 0;
}

/*******************************************************************************
 * This function generates the file name of the compressed text.
*******************************************************************************/
int generateCompressedFileName(char *inputFileName, int inputFileNameLength, 
	char *compressedFileName)
{
	strcpy(compressedFileName, inputFileName);
	compressedFileName[inputFileNameLength - 4] = '\0';

	char *appendName = "Compressed.txt";
	strcat(compressedFileName, appendName);

	return 0;
}

/*******************************************************************************
 * This function generates the file name of the codes.
*******************************************************************************/
int generateCodeFileName(char *compressedFileName, int compressedFileNameLength, 
	char *codeFileName)
{
	strcpy(codeFileName, compressedFileName);
	codeFileName[compressedFileNameLength - 4] = '\0';

	char *appendName = "Codes.txt";
	strcat(codeFileName, appendName);

	return 0;
}

/*******************************************************************************
 * This function generates the decompressed file name.
*******************************************************************************/
int generateDecompressedFileName(char *compressedFileName, 
	int compressedFileNameLength, char *decompressedFileName)
{
	strcpy(decompressedFileName, compressedFileName);
	decompressedFileName[compressedFileNameLength - 14] = '\0';

	char *appendName = "Decompressed.txt";
	strcat(decompressedFileName, appendName);

	return 0;
}

/*******************************************************************************
 * This function inouts the compressd file into a binary string.
*******************************************************************************/
int inputCompressedFileToBinString(char *compressedFileName, 
	char *compressedInputInBin)
{
	FILE *fpOutIn;
	fpOutIn = fopen(compressedFileName, "r");
	if(fpOutIn == NULL)
	{
		fprintf(stderr, "Cannot open input file.\n");
		return 1;
	}

	fseek(fpOutIn, 0, SEEK_END);
	fseek(fpOutIn, 0, SEEK_SET);

	int c;
	int g;
	*compressedInputInBin = '\0';
	char compressedInput[] = "\0";
	char buff[] = "\0";
	c = fgetc(fpOutIn);
	while(c != EOF)
	{
		compressedInput[0] = reverseBitsInByte(c);
		for(g=0; g<8; g++)
		{
			buff[0] = compressedInput[0] & 1;
			if(buff[0] == '\0')
			{
				buff[0] = '0';
			}else if(buff[0] == ('1' - '0'))
			{
				buff[0] = '1';
			}
			strcat(compressedInputInBin, buff);
			compressedInput[0] >>= 1;
		}
		c = fgetc(fpOutIn);
	}
	fclose(fpOutIn);

	return strlen(compressedInputInBin);
}

/*******************************************************************************
 * This function reverses the bits in the input byte.
*******************************************************************************/
char reverseBitsInByte(char byte)
{
	char reversedByte = '\0';
	int i;
	for(i=0; i<7; i++)
	{
		reversedByte += (1 & byte);
		reversedByte <<= 1;
		byte >>= 1;
	}
	reversedByte += (1 & byte);
	return reversedByte;
}
