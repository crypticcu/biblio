// Citation information credit Amanda Howell @ https://www.youtube.com/channel/UCqCSnXwJSpXNJRR0EHIk7RA/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> //POSIX-exclusive

#define max_read_size 256
#define str_alloc (char *) malloc(max_read_size)

struct Citation {
	char *title;
	char *first;
	char *last;
	char *others;
	
	char *city;
	char *publisher;
	char *publication;
	char *issue;
	char *page;

	char *website;
	char *url;
	char *doi;

	char *month;
	char *day;
	char *year;
	char *access;
} Item;

/* Removes leading newline from fgets() */
void fgets_nnl(char *buffer, int size, FILE *stream) {
	fgets(buffer, size, stream);
	buffer[strcspn(buffer, "\n")] = 0; // Truncates string to length up to newline character
}

/* Returns boolean if strings are equal */
bool streq(const char *string1, const char *string2) {
	if (strcmp(string1, string2) == 0) // strcmp() returns 0 if string are identical
		return true;
	else
		return false;
}

/* Resets text */
void reset_txt() {
	printf("\e[0m");
}

/* Italicizes text */
void italic_txt() {
	printf("\e[3m");
	}

/* Boldens text */
void bold_txt() {
	printf("\e[1m");
}

/* Outputs the given error message */
void err_out(char *string) {
	printf("   └");
	bold_txt();
	printf(" Error: ");
	reset_txt();
	printf("%s\n", string);
}

/* Creates a citation */
void cite(FILE *file, bool *editing_sw, bool *esc_sw) {
	char *str_buf = str_alloc, *style = str_alloc, *type = str_alloc;
	
	Item.title = str_alloc;
	Item.first = str_alloc;
	Item.last = str_alloc;
	Item.others = str_alloc;
	
	Item.city = str_alloc;
	Item.publisher = str_alloc;
	Item.publication = str_alloc;
	Item.issue = str_alloc;
	Item.page = str_alloc;

	Item.website = str_alloc;
	Item.url = str_alloc;
	Item.doi = str_alloc;

	Item.month = str_alloc;
	Item.day = str_alloc;
	Item.year = str_alloc;
	Item.access = str_alloc;

	for (;;) {
		printf("   └ Style: ");
		fgets_nnl(style, max_read_size, stdin);

		/* STYLED CITATIONS */
		if (streq(style, "mla") || streq(style, "apa") || streq(style, "chicago")) {
			printf("      └ Type: ");
			fgets_nnl(type, max_read_size, stdin);
			if (streq(type, "website") || streq(type, "book") || streq(type, "article")) {

				/* ----- General Attributes ----- */
				printf("         ├ Title: ");
				fgets_nnl(Item.title, max_read_size, stdin);
				printf("         ├ Author name: \n");
				printf("         │  ├ First: ");
				fgets_nnl(Item.first, max_read_size, stdin);
				printf("         │  ├ Last: ");
				fgets_nnl(Item.last, max_read_size, stdin);
				printf("         │  └ Others? (y/n): ");
				fgets_nnl(Item.others, max_read_size, stdin);

				/* ----- Website-specific attributes ----- */
				if (streq(type, "website")) {
					if (!streq(style, "apa")) {
						printf("         ├ Website: ");
						fgets_nnl(Item.website, max_read_size,stdin);
						printf("         │  └ URL: ");
					} else
						printf("         ├ URL: ");
					fgets_nnl(Item.url, max_read_size, stdin);
				}

				/* ----- Website/book-specific attributes ----- */
				if (streq(type, "website") && !streq(style, "apa") || streq(type, "book")) {
					printf("         ├ Publisher: ");
					fgets_nnl(Item.publisher, max_read_size,stdin);
				}

				/* ----- Book-specific attributes ----- */
				if (streq(type, "book") && !streq(style, "mla")) {
					printf("         │  └ City: ");
					fgets_nnl(Item.city, max_read_size, stdin);
				}

				/* ----- Article-specific attributes ----- */
				if (streq(type, "article")) {
					printf("         ├ Publication: ");
					fgets_nnl(Item.publication, max_read_size, stdin);
					printf("         │  ├ Issue #: ");
					fgets_nnl(Item.issue, max_read_size, stdin);
					printf("         │  ├ Page(s): ");
					fgets_nnl(Item.page, max_read_size, stdin);
					printf("         │  └ DOI: ");
					fgets_nnl(Item.doi, max_read_size, stdin);
				}

				/* ----- Date attributes ----- */
				printf("         └ Date:\n");
				printf("            ├ Year: ");
				fgets_nnl(Item.year, max_read_size, stdin);
				if (Item.year[0] != 0) {
					printf("            ├ Month: ");
					fgets_nnl(Item.month, max_read_size,stdin);
					if (Item.month[0] != 0) {
						printf("            ├ Day: ");
						fgets_nnl(Item.day, max_read_size,stdin);
					}
				printf("            └ Access date? (y/n): ");
				fgets_nnl(Item.access, max_read_size, stdin);
				}

				/* ----- MLA output ----- */
				if (streq(style, "mla")) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (Item.others[0] == 'y')
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0) {
						fputs("\"", file);
						fputs(Item.title, file);
						fputs(".\" ", file);
					}
					if (Item.website[0] != 0 && streq(type, "website")) {
						fputs(Item.website, file);
						fputs(", ", file);
					}
					if (Item.publisher[0] != 0) {
						fputs(Item.publisher, file);
						fputs(", ", file);
					}
					if (Item.publication[0] != 0) {
						fputs(Item.publication, file);
						fputs(" ", file);
						if (!streq(Item.issue, "")) {
							fputs("#", file);
							fputs(Item.issue, file);
							fputs(", ", file);
							if (!streq(Item.page, "")) {
								fputs(Item.page, file);
								fputs(", ", file);
							}
						}
					}
					if (Item.year[0] != 0) {
						if(Item.access[0] == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fputs(Item.month, file);
							fputs(" ", file);
							if (Item.day[0] != 0) {
								fputs(Item.day, file);
								fputs(", ", file); 
							}
						}
						fputs(Item.year, file);
						if (streq(type, "website")) 
							fputs(", ", file);
						else 
							fputs(". ", file);
					}
					if (Item.url[0] != 0) {
						fputs(Item.url, file);
						fputs(". ", file);
					}
					if (Item.doi[0] != 0) {
						fputs("doi:", file);
						fputs(Item.doi, file);
						fputs(". ", file);
					}

				/* ----- APA output ----- */
				} else if (streq(style, "apa")) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (Item.others[0] != 0)
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.year[0] != 0) {
						fputs("(", file);
						if(Item.access[0] == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fputs(Item.month, file);
							fputs(" ", file);
							if (Item.day[0] != 0) {
								fputs(Item.day, file);
								fputs(", ", file);
							}
						}
						fputs(Item.year, file);
						fputs("). ", file);
					}
					if (Item.title[0] != 0) {
						fputs(Item.title, file);
						fputs(". ", file);
					}
					if (Item.city[0] != 0) {
						fputs(Item.city, file);
						fputs(": ", file);
					}
					if (Item.publisher[0] != 0) {
						fputs(Item.publisher, file);
						fputs(". ", file);
					}
					if (Item.publication[0] != 0) {
						fputs(Item.publication, file);
						fputs(", ", file);
						if (Item.issue[0] != 0) {
							fputs("#", file);
							fputs(Item.issue, file);
							fputs(", ", file);
							if (Item.page[0] != 0) {
								fputs(Item.page, file);
								fputs(". ", file);
							}
						}
					}
					if (Item.url[0] != 0) {
						fputs("Retrieved from ", file);
						fputs(Item.url, file);
					}
					if (Item.doi[0] != 0) {
						fputs("doi:", file);
						fputs(Item.doi, file);
					}
				
				/* ----- Chicago-style output ----- */
				} else if (streq(style, "chicago")) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (Item.others[0] != 0)
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0) {
						if (!streq(type, "book")) {
							fputs("\"", file);
							fputs(Item.title, file);
							fputs(".\" ", file);
						}
						fputs(Item.title, file);
						fputs(". ", file);
					}
					if (Item.city[0] != 0) {
						fputs(Item.city, file);
						fputs(": ", file);
					}
					if (Item.publisher[0] != 0) {
						fputs(Item.publisher, file);
						fputs(", ", file);
					}
					if (Item.publication[0] != 0) {
						fputs(Item.publication, file);
						fputs(", ", file);
						if (Item.issue[0] != 0) {
							fputs(Item.issue, file);
							fputs(" ", file);
						}
					}
					if (Item.year[0] != 0) {
						if (streq(type, "article"))
							fputs("(", file);
						if(Item.access[0] == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fputs(Item.month, file);
							fputs(" ", file);
							if (Item.day[0] != 0) {
								fputs(Item.day, file);
								fputs(", ", file);
							}
						}
						fputs(Item.year, file);
						if (streq(type, "article"))
							fputs("): ", file);
						else
							fputs(". ", file);
					}
					if (Item.page[0] != 0) {
						fputs(Item.page, file);
						fputs(". ", file);
					}
					if (Item.url[0] != 0) {
						fputs(Item.url, file);
					}
					if (Item.doi[0] != 0) {
						fputs("doi:", file);
						fputs(Item.doi, file);
					}
				}

				fprintf(file, "\n");
				fclose(file);
				break;
			} else if (streq(type, "esc")) {
				*editing_sw = false;
				*esc_sw = true;
				break;
			} else {
				printf("      ");
				err_out("Unknown citation type");
			}
	
		/* PLAIN CITATIONS */
		} else if (streq(style, "manual")) {

				/* ----- Manual entry ----- */
				printf("      └ Manual entry: ");
				fgets_nnl(str_buf, max_read_size, stdin);

				/* ----- Output ----- */
				fputs(str_buf, file);
				fprintf(file, "\n");
				fclose(file);
				break;

		} else if (streq(style, "esc")) {
			*editing_sw = false;
			*esc_sw = true;
			break;
		} else {
			printf("   ");
			err_out("Unknown citation style");
		}

	}
	free(str_buf);
	free(style);
	free(type);
}

/* Deletes a line from a file */
void del_line(FILE *file, char *path, int ln_del) {
	char *str_buf = str_alloc;
	FILE *temp_file;
	fpos_t pos;

	temp_file = fopen(".temp", "w");

	/* ----- Goes to line and ignores it ----- */
	for (int ln_num = 1; ln_num < ln_del; ln_num++) {
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		fgets(str_buf, max_read_size, file);
		fprintf(temp_file, "%s", str_buf);
	}
    fgets(str_buf, max_read_size, file);

	/* ----- Prints the rest of the file ----- */
    for (;;) {
        fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		fgets(str_buf, max_read_size, file);
		fprintf(temp_file, "%s", str_buf);
    }

    remove(path);
    rename(".temp", path); // Replaces old file with new one
    fclose(temp_file);
	rewind(file);
	free(str_buf);
}

/* Modifies a line in a file */
void mod_line(FILE *file, char *path, int ln_mod, char *string) {
	char *str_buf = str_alloc;
	FILE *temp_file;
	fpos_t pos;


	temp_file = fopen(".temp", "w");

	/* ----- Goes to line and replaces it ----- */
	for (int ln_num = 1; ln_num < ln_mod; ln_num++) {
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		fgets(str_buf, max_read_size, file);
		fprintf(temp_file, "%s", str_buf);
	}
    fgets(str_buf, max_read_size, file);
	fprintf(temp_file, "%s", string);

	if(streq(path, ".bibliorc")) { // Biblio-exclusive
		fprintf(temp_file, "\n");
	}

	/* ----- Prints the rest of the file ----- */
    for (;;) {
        fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		fgets(str_buf, max_read_size, file);
		fprintf(temp_file, "%s", str_buf);
    }

    remove(path);
    rename(".temp", path); // Replaces old file with new one
    fclose(temp_file);
	rewind(file);;
	free(str_buf);
}

/* Moves a line in a file to another position */
void mov_line(FILE *file, char *path, int ln_from, int ln_to) {
	char *str_buf = str_alloc, *str_buf_ex = str_alloc;
	int ln_num = 1;
	FILE *temp_file;
	fpos_t pos;

	temp_file = fopen(".temp", "w");
	if(ln_from < ln_to) // Bug fix
		ln_to++;

	/* ----- Scans file for line to move ----- */
	for (ln_num; ln_num < ln_from; ln_num++) {
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		fgets(str_buf, max_read_size, file);
	}
    fgets(str_buf_ex, max_read_size, file);
	rewind(file);

	/* ----- Places line in new position ----- */
	for (ln_num = 1; ln_num < ln_to; ln_num++) {
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		if (ln_num != ln_from) {
			fgets(str_buf, max_read_size, file);
			fprintf(temp_file, "%s", str_buf);
		} else
			fgets(str_buf, max_read_size, file);
	}
	fprintf(temp_file, "%s", str_buf_ex);

	/* ----- Prints the rest of the file ----- */
	for (ln_num;; ln_num++) {
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
		if (ln_num != ln_from) {
			fgets(str_buf, max_read_size, file);
			fprintf(temp_file, "%s", str_buf);
		} else
			fgets(str_buf, max_read_size, file);
	}

    remove(path);
    rename(".temp", path); // Replaces old file with new one
    fclose(temp_file);
	fclose(file);
	rewind(file);
	free(str_buf);
	free(str_buf_ex);
}

/* Outputs the contents of a file with line numbers */
void fout(FILE *file) {
	bool extra_space = true;
	char chr_buf;
	int chr_count = 0;
	fpos_t pos;

	rewind(file);
	printf(" ┌───────────────────────────────────────────────────────────────────────────┐\n"); // Beginning of bibliography UI
	for (int ln_num = 1;; ln_num++) {
		printf(" │ %d. ", ln_num);
		for (;;) {
			chr_buf = fgetc(file);
			chr_count++;
			if (chr_buf == EOF || chr_buf == '\n')
				break;
			if (chr_count >= 71) {
				printf(" │▒\n │    %c", chr_buf);
				chr_count = 1;
			} else
				printf("%c", chr_buf);
		}
		for (chr_count; chr_count < 71; chr_count++) {
			printf(" ");
		}
		chr_count = 0;
		printf(" │▒\n │                                                                           │▒\n");
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
	}

	printf(" └───────────────────────────────────────────────────────────────────────────┘▒\n");
	printf("  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒\n"); // End of bibliography UI
}

/* Moves position in file to a line */
void fsetpos_toline(FILE *file, int num) {
	char *str_buf = str_alloc;
	int ln_num = 1;

	rewind(file);
	for (;;) {
		if (ln_num == num) // Once the desired line is reached, end loop
			break;
		fgets(str_buf,max_read_size, file); // Advances the file one line
		ln_num++;
	}
	free(str_buf);
}

/* Prints a line in a file to stdout */
void echo(FILE* file, char *path, int num) {
	char *str_buf = str_alloc;

	file = fopen(path, "r");
	free(str_buf);
	fsetpos_toline(file, num);
	fgets(str_buf, max_read_size, file);
	printf("%s", str_buf);
	rewind(file);
}

/* Outputs the title and help page */
void title_help() {
	char *str_buf = str_alloc;

	system("clear");
	printf("Bibliographer v1.2 » ");
	bold_txt();
	puts("Help Page\n");
	reset_txt();

	/* ----- Help Page ----- */
	bold_txt();
	puts("Commands");
	reset_txt();
	puts("cite: Creates a citation.");
	puts("config: Configures preferences.");
	puts("exit: Exits the program.");
	puts("help: Displays the help page.");
	puts("open: Opens a bibliography (.bib) file.\n");

	italic_txt();
	puts("While a bibliography is open...");
	reset_txt();
	puts("clear: Deletes all entries in a bibliography.");
	puts("close: Closes the current bibliography.");
	puts("delete: Deletes an entry.");
	puts("echo: Returns a citation");
	puts("modify: Modifies an entry.");
	puts("move: Moves an entry to a position.");
	puts("refresh: Updates file contents.\n");

	bold_txt();
	puts("Tips");
	reset_txt();
	puts(" * Citations should be in alphabetical order");
	puts(" * Use one citation style per document");
	puts(" * Leave all unknown fields blank\n");

	printf("Press [ENTER] to continue.");
	fgets_nnl(str_buf, max_read_size, stdin);
	free(str_buf);
}

/* Outputs the title and current file */
void title_fopen(FILE *file, char *path) {
	system("clear");
	printf("Bibliographer v1.2 » ");
	bold_txt();
	printf("%s\n\n", path);
	reset_txt();
	file = fopen(path, "r");
	if (file != NULL) {
		fout(file);
		fclose(file);
	}
	printf("\n");
}

/* Outputs the title */
void title_fclosed() {
	system("clear");
	bold_txt();
	puts("Bibliographer v1.2\n");
	reset_txt();
	printf("\n");
}

/* Main */
const int main(void) {
	char cmd_symbol, *str_input = str_alloc, *path = str_alloc;
	bool pref_created, auto_refresh, file_open, editing, oper_success, esc;
	int num_input;
	FILE *fptr;
	struct Citation Item;

	/* POPULATE .bibliorc IF NONEXISTANT */
	if (access(".bibliorc", R_OK|W_OK) == -1) {
		fptr = fopen(".bibliorc", "a");
		fprintf(fptr, "Auto-refresh:\noff\n\nCommand symbol:\n>");
		pref_created = true;
	}

	/* CREATE Default.bib IF NONEXISTANT */
	if (access("Default.bib", R_OK|W_OK) == -1) { // If 'Default.bib' does not exist...
		fptr = fopen("Default.bib", "a");
	}

	/* CHECK PREFERENCES */
	if (access(".bibliorc", R_OK|W_OK) != -1 || pref_created) {
		fptr = fopen(".bibliorc", "r");

		/* ----- 'auto_refresh' preference ----- */
		fsetpos_toline(fptr, 2);
		fgets(str_input, max_read_size, fptr);
		if (streq(str_input, "on"))
			auto_refresh = true;

		/* ----- 'cmd_symbol' preference ----- */
		fsetpos_toline(fptr, 5);
		cmd_symbol = fgetc(fptr);
	}

	title_fclosed();

	/* PROGRAM LOOP */
	for (;;) {

		/* ----- Command line ----- */
		printf("%c ", cmd_symbol); // Ensures command line cursor of user's choice
		fgets_nnl(str_input, max_read_size, stdin);

		/* ----- 'modify' command (file-exclusive) ----- */
		if (streq(str_input, "modify")) {
			if (file_open) {
				printf("   ├ Citation #: ");
				fgets_nnl(str_input, max_read_size, stdin);
				num_input = atoi(str_input);
				if (!streq(str_input, "esc")) // Breaks if 'esc' is input
					editing = true; // Allows for 'cite' command to commence right afterwards
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'cite' command ----- */
		if (streq(str_input, "cite") || editing) {
			fptr = fopen(".temp", "w");
			switch (file_open) {
			case 0:
				file_open = true;
				strcpy(path, "Default.bib"); // If no file is specified, open default bibliography
				title_fopen(fptr, path);
				printf("%c cite\n", cmd_symbol);
			case 1:
				cite(fptr, &editing, &esc);
				if(!esc) { // Breaks if 'esc' is input
					fptr = fopen(".temp", "r");
					if (editing) { // If came from 'modify' command...
						fgets(str_input, max_read_size, fptr);
						fclose(fptr);
						fptr = fopen(path, "r");
						mod_line(fptr, path, num_input, str_input);
						editing = false; // Ensures that 'cite' command is not guaranteed to run afterwards
					} else { // Else...
						fgets(str_input, max_read_size, fptr);
						fclose(fptr);
						fptr = fopen(path, "a");
						fputs(str_input, fptr);
					}
					fclose(fptr);
					if (auto_refresh) // Refreshes automatically if 'auto_refresh' option is on
						title_fopen(fptr, path);
				}
				esc = false;
			}
			oper_success = true;
		}

		/* ----- 'clear' command  (file-exclusive) ----- */
		if (streq(str_input, "clear")) {
			if (file_open) {
				fptr = fopen(path, "w");
				if (fptr == NULL) {
					err_out("Cannot clear file\nInput any key to continue.");
					break;
				}
				fclose(fptr);
				if (auto_refresh) // Refreshes automatically if 'auto_refresh' option is on
						title_fopen(fptr, path);
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'close' command (file-exclusive) ----- */
		if (streq(str_input, "close")) {
			if (file_open) {
				file_open = false;
				title_fclosed();
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'config' command ----- */
		if (streq(str_input, "config")) {
			fptr = fopen(".bibliorc", "r");
			printf("   ├ Auto-refresh: "); // Configuration for 'auto_refresh' preference
			fgets_nnl(str_input, max_read_size, stdin);
			if(!streq(str_input, "esc")) { // Breaks if 'esc' is input
				mod_line(fptr, ".bibliorc", 2, str_input);
				printf("   └ Command symbol: "); // Configuration for 'cmd_symbol' preference
				fgets_nnl(str_input, max_read_size, stdin);
				if(!streq(str_input, "esc")) // Breaks if 'esc' is input
					mod_line(fptr, ".bibliorc", 5, str_input);
			}
			fclose(fptr);
			oper_success = true;
		}

		/* ----- 'delete' command (file-exclusive) ----- */
		if (streq(str_input, "delete")) {
			if (file_open) {
				printf("   └ Citation #: ");
				fgets_nnl(str_input, max_read_size, stdin);			
				if (!streq(str_input, "esc")) { // Breaks if 'esc' is input
					fptr = fopen(path, "r");
					del_line(fptr, path, atoi(str_input));
					fclose(fptr);
				}
				if (auto_refresh) // Refreshes automatically if 'auto_refresh' option is on
					title_fopen(fptr, path);
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'echo' command ----- */
		if (streq(str_input, "echo")) {
			if(file_open) {
				printf("   └ Citation #: ");
				fgets_nnl(str_input, max_read_size, stdin);
				echo(fptr, path, atoi(str_input));
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'exit' command ----- */
		if (streq(str_input, "exit")) {
			system("clear"); // Leaves no trace of program
			oper_success = true;
			break; // Breaks out of program loop
		}

		/* ----- 'help' command ----- */
		if (streq(str_input, "help")) {
			title_help();

			if (file_open)
				title_fopen(fptr, path);
			else
				title_fclosed();
			oper_success = true;
		}

		/* ----- 'move' command ----- */
		if (streq(str_input, "move")) {
			if(file_open) {
				printf("   ├ Citation #: ");
				fgets_nnl(str_input, max_read_size, stdin);
				fptr = fopen(path, "r");
				if (atoi(str_input) < 0) // Ensures input over 0
					err_out("Citation does not exist");
				else if (!streq(str_input, "esc")) { // Breaks if 'esc' is input
					num_input = atoi(str_input);
					printf("   └ Move to: ");
					fgets_nnl(str_input, max_read_size, stdin);
					if(!streq(str_input, "esc")) // Breaks if 'esc' is input
						mov_line(fptr, path, num_input, atoi(str_input));
				}
				if (auto_refresh) // Refreshes automatically if 'auto_refresh' option is on
					title_fopen(fptr, path);
			} else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- 'open' command ----- */
		if (streq(str_input, "open")) {
			printf("   └ Bibliography name: ");
			fgets_nnl(path, max_read_size, stdin);
			strcat(path, ".bib"); // Ensures that a bibliography file is created
			if (!streq(path, "esc")) { // Breaks if 'esc' is input
				file_open = true;
				title_fopen(fptr, path);
			}
			oper_success = true;
		}

		/* ----- 'refresh' command (file-exclusive) ----- */
		if (streq(str_input, "refresh")) {
			if (file_open)
				title_fopen(fptr, path); // Refreshes the screen
			else
				err_out("No file is open");
			oper_success = true;
		}

		/* ----- Checks invalid commands ----- */
		if (!oper_success)
			err_out("Unknown command");

		oper_success = false;
	}

	remove(".temp");
	free(str_input);
	free(path);
	return EXIT_SUCCESS;
}
