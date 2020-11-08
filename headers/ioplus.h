/*	LICENSING
	──────────────────────────────────────────────────────────────────────
	ioplus.h - Addional I/O functions and macros
	Copyright (C) 2020 crypticcu@protonmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#ifndef _IOPLUS_H_
#define _IOPLUS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#define READ_MAX	440440	/* Memory allocation limit */

#define X		""	/* Empty string */

#define A_MOD	1	/* Sorting modes */
#define N_MOD	2	/*  for fsort()  */

#define E_BLD	"\e[1m"	/*   Text formatting	*/
#define E_ITL	"\e[3m"	/*     escape codes	  	*/
#define E_UND	"\e[4m"	/*					  	*/
#define E_STR	"\e[9m"	/* Compatible with most	*/
#define E_CLR	"\e[0m"	/* ANSI/VT100 terminals	*/

#define TYPES	_Bool: "bool",						unsigned char: "unsigned char",						\
				char: "char",						signed char: "signed char",							\
				short int: "short int",				unsigned short int: "unsigned short int",			\
				int: "int",							unsigned int: "unsigned int",						\
				long int: "long int",				unsigned long int: "unsigned long int",				\
				long long int: "long long int",		unsigned long long int: "unsigned long long int",	\
				float: "float",						double: "double",									\
				long double: "long double",			char *: "pointer to char",							\
				void *: "pointer to void",			int *: "pointer to int"
#define typename(var) _Generic((var), TYPES)	/* Allows for retrieval of data type from variables */


//////////////////
// INPUT-OUTPUT //
//////////////////

/* Get char from 'stream'
 *Flushes stream if user input is given */
int fgetchr(FILE *stream) {
	int chr = fgetc(stream);

	if (chr != '\n' && stream == stdin)
		while ((fgetc(stream)) != '\n');
	return chr;
}

/* Get chr from stdin
 * Flushes stream if user input is given */
int getchr(void) {
	return fgetchr(stdin);
}

/* Get string from 'stream' to put into 'buffer'
 * Reads up to 'n' characters
 * Flushes stream if user input is given */
void fngetstr(FILE *stream, char buffer[], int n) {
	char chr;
	int i;

	for (i = 0; (chr = fgetc(stream)) != '\n' && i < n; i++)
		buffer[i] = chr;
	buffer[i] = 0;
	if (chr != '\n')
		while ((fgetc(stream)) != '\n');
}

/* Get string from 'stream' to put into 'buffer'
 * Flushes stream if user input is given */
void fgetstr(FILE *stream, char *buffer) {
	fngetstr(stream, buffer, INT_MAX);
}

/* Get string from stdin  into 'buffer'
 * Reads up to 'n' characters
 * Flushes stream */
void ngetstr(char *buffer, int n) {
	fngetstr(stdin, buffer, n);
}

/* Get string from stdin to put into 'buffer'
 * Flushes stream */
void getstr(char *buffer) {
	fngetstr(stdin, buffer, INT_MAX);
}

/* Get dynamically allocated string from 'stream'
 * Reads up to 'n' characters
 * Flushes stream if user input is given */
char *afngetstr(FILE *stream, int n) {
	char chr, *str = (char*) malloc(0);
	int i;
	
	for (i = 0; (chr = fgetc(stream)) != '\n' && i < n && i < READ_MAX; i++) {
		str = (char *) realloc(str, i + 1);
		str[i] = chr;
	}
	if (chr != '\n' && stream == stdin)
		while ((fgetc(stream)) != '\n');
	if (i > 0) {
		str[i] = 0;
		return str;
	} else
		return X;
}

/* Get dynamically allocated string from 'stream'
 * Flushes stream if user input is given */
char *afgetstr(FILE *stream) {
	return afngetstr(stream, INT_MAX);
}

/* Get dynamically allocated string from stdin 
 * Reads up to 'n' characters
 * Flushes stream */
char *angetstr(int n) {
	return afngetstr(stdin, n);
}

/* Get dynamically allocated string from stdin
 * Flushes stream */
char *agetstr(void) {
	return afngetstr(stdin, INT_MAX);
}

/* Get int from 'stream' to put into 'buffer'
 * Flushes stream if user input is given */
int fgetint(FILE *stream) {
	char chr = 0, *str = (char *) malloc(0);
	static int num = 0;
	int i;

	for (i = 0; (isdigit(chr = fgetc(stream)) || chr == '-' || chr == '+') && i < READ_MAX; i++) {
		str = (char *) realloc(str, i + 1);
		str[i] = chr;
	}
	num = atoi(str);
	free(str);
	return num;
}

/* Get int from stdin
 * Flushes stream */
int getint(void) {
	return fgetint(stdin);
}

/* Get double from 'stream' to put into 'buffer'
 * Flushes stream if user input is given */
double fgetdbl(FILE *stream) {
	char chr, *str = (char *) malloc(0);
	double num;
	
	for (int i = 0; (isdigit(chr = fgetc(stream)) || chr == '-' || chr == '+' || chr == '.' || chr == ',' || chr == '\'') && i < READ_MAX; i++) {
		if (chr == ',' || chr == '\'')
			chr == '.';
		str = (char *) realloc(str, i + 1);
		str[i] = chr;
	}
	num = atof(str);
	free(str);
	return num;
}

/* Get double from stdin
 * Flushes stream */
double getdbl(void) {
	return fgetdbl(stdin);
}


/////////////////////
// FILE OPERATIONS //
/////////////////////

/* Checks if newline is final character in file 'path'
 * If not, then one is placed at end */
void fapp(char* path) {
	FILE *file = fopen(path, "r");

	fseek(file, -1, SEEK_END);
	if (fgetchr(file) != '\n') {
		file = fopen(path, "a");
		fputc('\n', file);
	}
	fclose(file);
}

/* Counts the number of characters in file 'path', including escapes */
unsigned int fcountc(char *path) {
	int i;
	FILE *file = fopen(path, "r");

	for (i = 0; fgetchr(file) != EOF; i++);
	fclose(file);
	return i;
}

/* Returns length of longest line in 'path' */
unsigned int flongl(char *path) {
	char chr, *str = (char *) malloc(0);
	int size = 0, num = 0;
	FILE *file = fopen(path, "r");

	fapp(path);
	while ((chr = fgetchr(file)) != EOF) {
		fseek(file, -1, SEEK_CUR);
		while ((chr = fgetchr(file)) != '\n') {
			str = (char *) realloc(str, size + 1);
			str[size] = chr;
			size++;
		}
		if (size > num)
			num = size;
		size = 0;
	}
	free(str);
	return num;
}

/* Returns length of shortest line in 'path'
 * If 'blanks' is true, blank lines will be accounted for */
unsigned int fshortl(char *path, bool blanks) {
	char chr, *str = (char *) malloc(0);
	int size = 0, num = INT_MAX;
	FILE *file = fopen(path, "r");

	while ((chr = fgetchr(file)) != EOF) {
		if (chr == '\n' && blanks)
			continue;
		fseek(file, -1, SEEK_CUR);
		while ((chr = fgetchr(file)) != '\n') {
			str = (char *) realloc(str, size + 1);
			str[size] = chr;
			size++;
		}
		if (size < num)
			num = size;
		size = 0;
	}
	free(str);
	return num;
}

/* Counts the number of lines in file 'path' */
unsigned int fcountl(char *path) {
	int ln_num = 0, size = flongl(path);
	char str[size];
	FILE *file = fopen(path, "r");

	fapp(path);
	while (fgetchr(file) == EOF) {
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, str);
		ln_num++;
	}
	fclose(file);
	return ln_num;
}

/* Deletes the line 'ln_del' in file 'path' */
void fdell(char *path, int ln_del) {
	int size = flongl(path);
	char str[size];
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	/** Goes to line and ignores it **/
	for (int ln_num = 1; ln_num < ln_del; ln_num++) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, str);
		fputs(str, temp);
	}
	fgetstr(file, str);

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, str);
		fprintf(temp, "%s", str);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(file);
	fclose(temp);
}

/* Overwrites the line 'ln_mod' in file 'path'
 * Line is overwritten with 'string'
 * If 'newline' is true, a new line is placed afterwards */
void fmodil(char *path, int ln_mod, char *string, bool newline) {
	int size = flongl(path);
	char str[size];
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	/** Goes to line and replaces it **/
	for (int ln_num = 1; ln_num < ln_mod; ln_num++) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, str);
		fputs(str, temp);
	}
	fgetstr(file, str);
	fputs(str, temp);
	if(newline) {
		fputc('\n', temp);
	}

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, str);
		fputs(str, temp);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(file);
	fclose(temp);
}

/* Moves the line 'ln_from' in file 'path'
 * Line is moved to be before 'ln_to' */
void fmovl(char *path, int ln_from, int ln_to) {
	int size = flongl(path);
	char strA[size], strB[size];
	int ln_num = 1;
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	fapp(path);
	rewind(file);
	if (ln_from < ln_to)
		ln_to++;

	/** Scans file for line to move **/
	for (ln_num; ln_num < ln_from; ln_num++) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgetstr(file, strA);
	}
	fgetstr(file, strB);
	rewind(file);

	/** Places line in new position **/
	for (ln_num = 1; ln_num < ln_to; ln_num++) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		if (ln_num != ln_from) {
			fgetstr(file, strA);
			fputs(strA, temp);
		} else
			fgetstr(file, strA);
	}
	fputs(strB, temp);

	/** Prints the rest of the file **/
	for (ln_num;; ln_num++) {
		if (fgetchr(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		if (ln_num != ln_from) {
			fgetstr(file, strA);
			fputs(strA, temp);
		} else
			fgetstr(file, strA);
	}
	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(file);
	fclose(temp);
}

/* Moves position in 'file' to line 'ln_dest' */
void fseekl(FILE *file, int ln_dest) {
	int ln_num = 1;

	rewind(file);
	if (ln_dest > 1) {
		for (;;) {
			fseek(file, -1, SEEK_CUR);
			if (fgetchr(file) == EOF || ln_num == ln_dest) // Once the desired line is reached, end loop
				break;
			for (;;) { // Advances the file one line
				if (fgetchr(file) == '\n')
					break;
			}
			ln_num++;
		}
	}
}

/* Sorts lines in file 'path' according to 'mode'
 * Use macros A_MOD and N_MOD for mode */
void fsort(char* path, int mode) {
	int err = 0, size = flongl(path);
	char strA[size], strB[size];
	float chrA, chrB;
	FILE *file;
	fpos_t pos;

	/* 'a' = alphabetical mode
	 * 'n' = numerical mode
	 * 'a'|'n' ('o') = alphanumerical mode */

	/*  Alphabetical sorter
	 *
	 *	* Will execute if in alphabetical or alphanumerical mode
	 *	* Sorts file according to leftmost letter */
	if (mode == A_MOD || mode == (A_MOD|N_MOD)) {
		for (int i = 1; i < fcountl(path); i++) {
			chrA = 0; // Reset character variables
			chrB = 0;
			file = fopen(path, "r"); // File is reopened each time to update file pointer
			rewind(file);
			fseekl(file, i);
			fgetstr(file, strA);
			fgetstr(file, strB);
			for (int o = 0; o < strlen(strA); o++) { // Check for leftmost alphabetical character; if none is found, value remains at zero and line is sent to beginning of file
				if (islower(strA[o])) {
					chrA = strA[o];
					break;
				}
				if (isupper(strA[o])) {
					chrA = strA[o] + 32.5; // Ensures capital-letter lines are directly below lowercase-letter lines
					break;
				}
			}
			for (int o = 0; o < strlen(strB); o++) {
				if (islower(strB[o])) {
					chrB = strB[o];
					break;
				}
				if (isupper(strB[o])) {
					chrB = strB[o] + 32.5;
					break;
				}
			}
			if (chrA > chrB) { // Checks for out-of-order lines
				fmovl(path, i + 1, i);
				err++;
			}
			if (fgetc(file) == EOF) { // Stops if end of file is reached
				if (err == 0)
					break;
				else {
					i = 0;
					err = 0;
				}
			}
			fclose(file);
		}
	}

	/*  Numerical sorter
	 *
	 *	* Will execute if in numerical or alphanumerical mode
	 *	* Sorts file according to number or symbol
	 *	* Respects alphabetical sort if in alphanumerical mode */
	if (mode == N_MOD || mode == (A_MOD|N_MOD)) {
		for (int i = 1; i < fcountl(path); i++) {
			file = fopen(path, "r");
			fseekl(file, i);
			fgetstr(file, strA);
			fgetstr(file, strB);
			if (mode == (A_MOD|N_MOD)) { // Ensures alphabetical lines are not touched if in alphanumerical mode
				for (int o = 0; o < strlen(strA); o++) {
					if (islower(strA[o]) || isupper(strA[o])) {
						chrA = 0;
						break;
					}
				}
				for (int o = 0; o < strlen(strB); o++) {
					if (islower(strB[o]) || isupper(strB[o])) {
						chrB = 0;
						break;
					}
				}
				if (chrA == 0 || chrB == 0)
					continue;
			}
			chrA = strA[0];
			chrB = strB[0];
			if (chrA > chrB) { // Checks for out-of-order lines
				fmovl(path, i + 1, i);
				err++;
			}
			if (fgetc(file) == EOF) { // Stops if end of file is reached
				if (err == 0)
					break;
				else {
					i = 0;
					err = 0;
				}
			}
			fclose(file);
		}
	}

}


/////////////////////////
// STRING MANIPULATION //
/////////////////////////

/* Converts 'str' to all uppercase */
char *supper(char *string) {
	static char chr[sizeof(string)];

	for (int i = 0; i < strlen(string); i++) {
		if (islower(string[i]))
			chr[i] = toupper(string[i]);
		else
			chr[i] = string[i];
	}	
	return chr;
}

/* Converts 'str' to all lowercase */
char *slower(char *string) {
	static char chr[sizeof(string)];

	for (int i = 0; i < strlen(string); i++) {
		if (isupper(string[i]))
			chr[i] = tolower(string[i]);
		else
			chr[i] = string[i];
	}	
	return chr;
}

/* Free 'argc' number of blocks 
 * Blocks pointing to NULL will not be freed */
void bin(int argc, ...) {
	void *block;

	va_list valist;
	va_start(valist, argc);
	for (int i = 0; i < argc; i++) {
		block = va_arg(valist, void *);
		if (block != X)
			free(block);
	}
	va_end(valist);
}

#endif
