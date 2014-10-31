/* File: csvfunctions2.c
   by Jason Sanche
   UVic SENG265 Fall 2014 Assignment02 - Neigel Horsepool
   provides functions to manipulate csv files input via command line:
   ex: load "filename.csv", findrow B "abc" (row containing "abc" in col B), 
   printrow 3, evalsum C (sum column C), evalavg C (average column C), 
   save, print, stats, help, merge "otherfile.csv", 
   sort A (specify column: ex "A"), sortnumeric B (column B)
   deleterow 4, quit
   requires driver csvdriver2.c and csvfunctions2.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvfunctions2.h"

#define MAXINPUTLINELEN     256
#define MAXITEMSPERROW		128

#define CHECKMALLOC(p)	if ((p)==NULL) { fprintf(stderr,"out of memory!"); exit(1); } else { }

static int debug = 0;


// forward declarations
static int extractItems(char *line, char *row[]);
char *mystrdup(char *s);

void SS_SetDebug(int dbg) {
    debug = dbg;
}

SPREADSHEET *SS_ReadCSV(char *fileName) {
    char line[MAXINPUTLINELEN];
    char *tempRow[MAXITEMSPERROW];
    SPREADSHEET *result;
    struct OneRow *lastRow = NULL;
    int i;
	result = malloc(sizeof(SPREADSHEET));
	CHECKMALLOC(result);
    result->fileName = mystrdup(fileName);
    result->firstRow = NULL;
    result->numRows = result->numCols = 0;

    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read from file %s\n", fileName);
        perror(fileName);
        return NULL;
    }
    for( i = 0; ; i++) {
        if (fgets(line, MAXINPUTLINELEN, f) == NULL)
            break;
        int k = extractItems(line, tempRow);
        if (result->numCols == 0) {
            result->numCols = k;
        } 
        else
        if (result->numCols != k) {
            fprintf(stderr, "Row %d has different number of columns from first row\n", i);
            continue;	// ignore this row
        }

        // instantiate the storage for the new row and copy k cells into it
        char **rc = calloc(k, sizeof(char *));
        CHECKMALLOC(rc);
        struct OneRow *newrow = malloc(sizeof(struct OneRow));
        CHECKMALLOC(newrow);
        newrow->row = rc;
        newrow->nextRow = NULL;
        int ix;
        for( ix = 0; ix < k; ix++ ) {
            rc[ix] = tempRow[ix];
        }
        
        // add the new row as the last row in the spreadsheet
        if (lastRow == NULL) {
            result->firstRow = newrow;
        } 
        else {
            lastRow->nextRow = newrow;
        }
        lastRow = newrow;

    }
    result->numRows = i;
    fclose(f);
    return result;
}

void putCell(char *cell, FILE *fp) {
    
    char c = *cell++;
    fputc('\"',fp);
    while(c != '\0'){
    	if(c == '\"'){
    		fputc('\"',fp);
    		fputc('\"',fp);
    	}
    	else
    		fputc(c,fp);	
    	c = *cell++;
    }	
    fputc('\"',fp);
    
return;
}

// Write the spreadsheet in CSV format to the specified file 
void SS_SaveCSV(SPREADSHEET *ss, char *fileName) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_SaveCSV(--)\n");
    FILE *fp;
    fp = fopen(fileName, "w");
    if(fp==NULL)printf("Error");
    struct OneRow *cp = ss->firstRow;
    if(cp == NULL)printf("ERROR: nothing to save\n");
	while(cp != NULL){
		int col = 0;
		while(col < ss->numCols){
			char *cell = cp->row[col];
			putCell(cell, fp);
       		if(col == ss->numCols-1)
        		fprintf(fp,"\n");
        	else
    	    	fprintf(fp,",");
        	col++;
        }	
	    cp = cp->nextRow;
	}
	fclose(fp);
	return;
}



// Free all storage being use by the spreadsheet instance.
extern void SS_Unload(SPREADSHEET *ss) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_Unload(--)\n");
	int i;
	int nrows = ss->numRows;
	for(i=nrows;i>0;i--){
		SS_DeleteRow(ss,i-1);
	}
}


// Reads one item from the csv row.
// line is a string where reading should begin; tok reference a char array into
// which the characters for the item are copied.
// On return, the result references the remainder of the original string which has
// not been read, or is NULL if no item could be read before the end of the line.
static char *getOneItem(char *line, char *tok) {
    char *tokSaved = tok;
    char c;
    c = *line++;
S1: if (c == '\"') {
        c = *line++;
        goto S2;
    }
    if (c == ',' || c == '\0' || c == '\n' || c == '\r') {
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S1;
S2: if (c == '\"') {
        c = *line++;
        goto S3;
    }
    if (c == '\0' || c == '\n' || c == '\r') {
        // unexpected end of input line
        fprintf(stderr, "mismatched doublequote found");
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S2;
S3: if (c == '\"') {
        *tok++ = '\"';
        c = *line++;
        goto S2;
    }
    if (c == ',' || c == '\0' || c == '\n' || c == '\r') {
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S1;
S4: if (c == '\0' || c == '\n' || c == '\r') {
        if (tokSaved == tok)
            return NULL;  // nothing was read
        line--;
    }
    *tok = '\0';
    return line;
}

// Extracts items one by one from line, copies them into heap storage,
// and stores references to them in the row array.
// The function result is the number of items copied.
static int extractItems(char *line, char *row[]) {
    char t[MAXINPUTLINELEN];
    int col = 0;
    for( ; ; ) {
        line = getOneItem(line,t);
        if (line == NULL) break;
        char *s = mystrdup(t);
        row[col++] = s;
        
    }
    return col;
}

// prints filename, number of rows and number of columns for
// this spreadsheet
void SS_PrintStats(SPREADSHEET *ss) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_PrintStats(--)\n");
	printf("File: %s\nRows: %d\nColumns: %d\n", 
				ss->fileName,ss->numRows,ss->numCols);
}


// Transfers rows from spreadsheet ss2 to the end of spreadsheet ss1
// then releases any storage for ss2 which is no longer needed
void SS_MergeCSV(SPREADSHEET *ss1, SPREADSHEET *ss2) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_MergeCSV(--, --)\n");
	if (ss1->numCols != ss2->numCols){
		printf("Error: column number mismatch");
		return;
	}
	struct OneRow *rp;
	struct OneRow *rp2;
	int colcount = 0;
	for(rp=ss1->firstRow; rp!=NULL; rp=rp->nextRow){ 
		if (colcount++ == ss1->numCols){ 	//traverse to end of ss1
			for(rp2=ss2->firstRow; rp2!=NULL; rp2=rp2->nextRow){  //for each ss2 row
				char **rc = calloc(ss2->numCols, sizeof(char*)); //allocate storage
				CHECKMALLOC(rc);
				struct OneRow *newrow = malloc(sizeof(struct OneRow));//instantiate row
				CHECKMALLOC(newrow);
				newrow->row = rc;
				newrow->nextRow = NULL;
				int ix;
				for(ix=0; ix<ss2->numCols; ix++){		//
					rc[ix] = rp2->row[ix];
				}
				if(rp->nextRow != NULL){
					printf("oops");
					return;
					}
				else{
					rp->nextRow = newrow;
					ss1->numRows++;
					free(rp2->row);
				}
				rp = rp->nextRow;
			}
			free(ss2);
		}
	}
	
	return;
}

// Deletes the specified row from the spreadsheet.
// Any storage used by the row is freed.
void SS_DeleteRow(SPREADSHEET *ss, int rowNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_DeleteRow(--,%d)\n", rowNum);
	if(ss->firstRow == NULL)printf("ERROR: nothing to delete\n");
	struct OneRow *rp;
	int i = 0;
	for(rp=ss->firstRow; rp!=NULL; rp=rp->nextRow){
		if(rowNum >= ss->numRows){
			printf("Row number (%d) is out of range\n",rowNum);
			return;
		}
		if(rowNum == 0){
			if(rp->nextRow == NULL){
				free(rp->row[0]);
				ss->numRows--;
				ss->numCols = 0;
				ss->firstRow = NULL;
				free(ss);
				break;
				}
			else{
			ss->firstRow = rp->nextRow;
			free(rp->row[0]);
			ss->numRows--;
			break;
			}
		}
		else if(i == rowNum-1){
			if(rowNum == ss->numRows-1){
				rp->nextRow = NULL;
				free(rp->row[i+1]);
				ss->numRows--;
				break;
			}
			else{
			rp->nextRow = rp->nextRow->nextRow;
			free(rp->row[i+1]);
			ss->numRows--;
			break;
			}
		}
		i++;
	}
}

static int colNm = 0;

int strcompare(const void *a, const void *b) {
	struct OneRow *aptr = *(struct OneRow **)a;
	struct OneRow *bptr = *(struct OneRow **)b;
	return(strcmp(aptr->row[colNm],bptr->row[colNm]));
}

// Sorts the rows of the spreadsheet into ascending order, based on
// the strings in the specified column
extern void SS_Sort(SPREADSHEET *ss, int colNum) {
    colNm = colNum;
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_Sort(--,%d)\n", colNum);
    if(ss == NULL) {printf("invalid file"); return;}
    int arraySize = 4;
	struct OneRow **colArray;
	colArray = calloc(arraySize, sizeof(struct OneRow *));
	CHECKMALLOC(colArray);
	int numItems = 0;
    struct OneRow *cp;
	for (cp = ss->firstRow; cp!=NULL; cp = cp->nextRow){
		if(numItems >= arraySize){
			arraySize *= 2;
			colArray = realloc(colArray, arraySize*sizeof(struct Item*));
		}
		colArray[numItems++] = cp;
	}
	qsort(colArray,ss->numCols,sizeof(struct OneRow *),strcompare);
	struct OneRow *next = NULL;
	while(--numItems >= 0){
		colArray[numItems]->nextRow = next;
		next = colArray[numItems];
	}
	ss->firstRow = next;
	free(colArray);
	return ;
}

int numcompare(const void *a, const void *b) {
	struct OneRow *aptr = *(struct OneRow **)a;
	struct OneRow *bptr = *(struct OneRow **)b;
	return(int)(strtof(aptr->row[colNm],NULL) - (strtof(bptr->row[colNm],NULL)));
	
}

// Sorts the rows of the spreadsheet into ascending order, based on
// the values of the floating point numbers in the specified column
extern void SS_SortNumeric(SPREADSHEET *ss, int colNum) {
	colNm = colNum;
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_Sort(--,%d)\n", colNum);
    if(ss == NULL) {printf("invalid file"); return;}
    int arraySize = 4;
	struct OneRow **colArray;
	colArray = calloc(arraySize, sizeof(struct OneRow *));
	CHECKMALLOC(colArray);
	int numItems = 0;
    struct OneRow *cp;
	for (cp = ss->firstRow; cp!=NULL; cp = cp->nextRow){
		if(numItems >= arraySize){
			arraySize *= 2;
			colArray = realloc(colArray, arraySize*sizeof(struct Item*));
		}
		colArray[numItems++] = cp;
	}
	qsort(colArray,ss->numCols,sizeof(struct OneRow *),numcompare);
	struct OneRow *next = NULL;
	while(--numItems >= 0){
		colArray[numItems]->nextRow = next;
		next = colArray[numItems];
	}
	ss->firstRow = next;
	free(colArray);
	return ;
}


// Searches down the specified column for a row which contains text.
// The search starts at row number startNum;
// The result is the row number (where the first row is numbered 0).
// If the text is not found, the result is -1.
int SS_FindRow(SPREADSHEET *ss, int colNum, char *text, int startNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_FindRow(--,%d,%s,%d)\n",
            colNum, text, startNum);
    if (colNum > ss->numCols-1){
    	printf("--input out of range ");
    	return -1;
    	}
    int curr = 0;
    struct OneRow *cp = ss->firstRow;
	while(cp != NULL){
		if(strcmp(cp->row[colNum], text) == 0)
			return curr;
		cp = cp->nextRow;
		curr++;
		}
    return -1;
}

// Outputs the specified row of the spreadsheet.
// It is printed as one line on standard output.
void SS_PrintRow(SPREADSHEET *ss, int rowNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_PrintRow(--,%d)\n", rowNum);
    if (rowNum >= ss->numRows || rowNum < 0) {
        printf("Row number (%d) is out of range\n", rowNum);
        return;
    }
    struct OneRow *rp = ss->firstRow;
    while(rowNum > 0 && rp != NULL) {
        rp = rp->nextRow;
        rowNum--;
    }
    if (rp == NULL) {
        printf("Row number (%d) is out of range??\n", rowNum);
        return;        
    }
    int k;
    for( k = 0 ; k<ss->numCols; k++ ) {
        if (k>0)
            printf(", ");
        printf("%s", rp->row[k]);
    }
    putchar('\n');
    return;
}

// The specified column must contain the textual representations of numbers
// (either integer or floating point). The sum of the numbers in the column
// is returned as a floating point number.
double SS_ColumnSum(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnSum(--,%d)\n", colNum);
    if (colNum < 0 || colNum > ss->numCols){
    	printf("--input out of range");
    	return -1.0;
    	}
    double sum = 0.0;
    struct OneRow *cp = ss->firstRow;
	while(cp != NULL){	
		sum += atof(cp->row[colNum]);		
		cp = cp->nextRow;
	}
    return sum;
}

double SS_ColumnAvg(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnAvg(--,%d)\n", colNum);
    if (colNum < 0 || colNum > ss->numCols){
    	printf("--input out of range");
    	return -1.0;
    	}
    double sum = SS_ColumnSum(ss,colNum);
    return sum/ss->numRows;
}


// The strdup function is provided in many but not all variants of the
// C library. Here it is, renamed as mystrdup, just in case.
char *mystrdup(char *s) {
	int len = strlen(s);
	char *result = malloc(len+1);
	CHECKMALLOC(result);
	return strcpy(result, s);
}
