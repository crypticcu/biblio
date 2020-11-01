#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef BUF
#define BUF 256 // Default buffer size
#endif

#define A_MOD 1 /* Sorting modes */
#define N_MOD 2 /*	for fsort()	 */

/* Checks if newline is final character in file 'path'
 * If not, then one is created */
void fapp(char* path) {
	FILE *file = fopen(path, "r");

	fseek(file, -1, SEEK_END);
	if (fgetc(file) != '\n') {
		file = fopen(path, "a");
		fputc('\n', file);
	}
	fclose(file);
}

/* Counts the number of lines in file 'path' */
int fcountl(char *path) {
	char str_buf[BUF];
	int ln_num = 0;
	FILE *file = fopen(path, "r");

	fapp(path);
	for (;;) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		ln_num++;
	}
	fclose(file);
	return ln_num;
}

/* Deletes the line 'ln_del' in file 'path' */
void fdell(char *path, int ln_del) {
	char str_buf[BUF];
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	/** Goes to line and ignores it **/
	for (int ln_num = 1; ln_num < ln_del; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp, "%s", str_buf);
	}
	fgets(str_buf, BUF, file);

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp, "%s", str_buf);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(file);
	fclose(temp);
}

/* Gets line from 'stream' of size 'size' and transfers it to char array 'buffer' */
void fgetsl(char *buffer, int size, FILE *stream) {
	fgets(buffer, size, stream);
	buffer[strcspn(buffer, "\n")] = 0; // Truncates string to length up to newline character
}

/* Overwrites the line 'ln_mod' in file 'path'
 * Line is overwritten with 'string'
 * If 'nl_sw' is true, a new line is placed afterwards */
void fmodil(char *path, int ln_mod, char *string, bool nl_sw) {
	char str_buf[BUF];
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	/** Goes to line and replaces it **/
	for (int ln_num = 1; ln_num < ln_mod; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp, "%s", str_buf);
	}
	fgets(str_buf, BUF, file);
	fprintf(temp, "%s", string);
	if(nl_sw) {
		fprintf(temp, "\n");
	}

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp, "%s", str_buf);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(file);
	fclose(temp);
}

/* Moves the line 'ln_from' in file 'path'
 * Line is moved to be before 'ln_to' */
void fmovl(char *path, int ln_from, int ln_to) {
	char str_buf[BUF], str_buf_ex[BUF];
	int ln_num = 1;
	FILE *file = fopen(path, "r"), *temp = fopen(".temp", "w");

	fapp(path);
	rewind(file);
	if (ln_from < ln_to)
		ln_to++;

	/** Scans file for line to move **/
	for (ln_num; ln_num < ln_from; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
	}
	fgets(str_buf_ex, BUF, file);
	rewind(file);

	/** Places line in new position **/
	for (ln_num = 1; ln_num < ln_to; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		if (ln_num != ln_from) {
			fgets(str_buf, BUF, file);
			fprintf(temp, "%s", str_buf);
		} else
			fgets(str_buf, BUF, file);
	}
	fprintf(temp, "%s", str_buf_ex);

	/** Prints the rest of the file **/
	for (ln_num;; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		if (ln_num != ln_from) {
			fgets(str_buf, BUF, file);
			fprintf(temp, "%s", str_buf);
		} else
			fgets(str_buf, BUF, file);
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
			if (fgetc(file) == EOF || ln_num == ln_dest) // Once the desired line is reached, end loop
				break;
			for (;;) { // Advances the file one line
				if (fgetc(file) == '\n')
					break;
			}
			ln_num++;
		}
	}
}

/* Sorts lines in file 'path' according to 'mode'
 * Use macros A_MOD and N_MOD for mode */
void fsort(char* path, int mode) {
	char strA[BUF], strB[BUF];
	float chrA, chrB;
	int err = 0;
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
			fgetsl(strA, BUF, file);
			fgetsl(strB, BUF, file);
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
			fgetsl(strA, BUF, file);
			fgetsl(strB, BUF, file);
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
