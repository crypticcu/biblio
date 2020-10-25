#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BUF
#define BUF 256 // Default buffer size 
#endif

/* Counts the number of lines in 'file'
 * 'file' points to the file name 'path' */
int fcountl(FILE *file, char *path) {
	char str_buf[BUF];
	int ln_num = 0;
	fpos_t pos;

	file = fopen(path, "r");
	rewind(file);
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

/* Deletes the line 'ln_del' in 'file'
 * 'file' points to the file name 'path' */
void fdell(FILE *file, char *path, int ln_del) {
	char str_buf[BUF];
	FILE *temp_file;

	temp_file = fopen(".temp", "w");

	/** Goes to line and ignores it **/
	for (int ln_num = 1; ln_num < ln_del; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp_file, "%s", str_buf);
	}
	fgets(str_buf, BUF, file);

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp_file, "%s", str_buf);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(temp_file);
	rewind(file);
}

/* Gets line from 'stream' of size 'size' and transfers it to char array 'buffer' */
void fgetsl(char *buffer, int size, FILE *stream) {
	fgets(buffer, size, stream);
	buffer[strcspn(buffer, "\n")] = 0; // Truncates string to length up to newline character
}

/* Overwrites the line 'ln_mod' in 'file'
 * 'file' points to the file name 'path'
 * Line is overwritten with 'string' */
void fmodl(FILE *file, char *path, int ln_mod, char *string) {
	char str_buf[BUF];
	FILE *temp_file;

	temp_file = fopen(".temp", "w");

	/** Goes to line and replaces it **/
	for (int ln_num = 1; ln_num < ln_mod; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp_file, "%s", str_buf);
	}
	fgets(str_buf, BUF, file);
	fprintf(temp_file, "%s", string);

	if(strcmp(path, ".config") == 0) {
		fprintf(temp_file, "\n");
	}

	/** Prints the rest of the file **/
	for (;;) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		fgets(str_buf, BUF, file);
		fprintf(temp_file, "%s", str_buf);
	}

	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(temp_file);
	rewind(file);
}

/* Moves the line 'ln_from' in 'file'
 * 'file' points to the file name 'path'
 * Line is moved to be before 'ln_to' */
void fmovl(FILE *file, char *path, int ln_from, int ln_to) {
	char str_buf[BUF], str_buf_ex[BUF];
	int ln_num = 1;
	FILE *temp_file;

	temp_file = fopen(".temp", "w");
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
			fprintf(temp_file, "%s", str_buf);
		} else
			fgets(str_buf, BUF, file);
	}
	fprintf(temp_file, "%s", str_buf_ex);

	/** Prints the rest of the file **/
	for (ln_num;; ln_num++) {
		if (fgetc(file) == EOF)
			break;
		fseek(file, -1, SEEK_CUR);
		if (ln_num != ln_from) {
			fgets(str_buf, BUF, file);
			fprintf(temp_file, "%s", str_buf);
		} else
			fgets(str_buf, BUF, file);
	}
	remove(path);
	rename(".temp", path); // Replaces old file with new one
	fclose(temp_file);
	rewind(file);
}

/* Moves position in 'file' to the line 'ln_dest' */
void fseekl(FILE *file, int ln_dest) {
	char str_buf[BUF];
	int ln_num = 1;

	rewind(file);
	if (ln_dest > 1) {
		for (;;) {
			fseek(file, -1, SEEK_CUR);
			if (fgetc(file) == EOF || ln_num == ln_dest) // Once the desired line is reached, end loop
				break;
			fgets(str_buf,BUF, file); // Advances the file one line
			ln_num++;
		}
	}
}
