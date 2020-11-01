/*	LICENSING
	──────────────────────────────────────────────────────────────────────
	Biblio - command-line bibliography creator and editor
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
	along with this program.  If not, see <https://www.gnu.org/licenses/>.

	REFERENCES
	──────────────────────────────────────────────────────────────────────
	Amanda Howell, University of Wisconsin-Whitewater
	(https://www.youtube.com/channel/UCqCSnXwJSpXNJRR0EHIk7RA/)

	University of Illinois at Chicago
	(https://library.uic.edu/help/article/1966/
	what-is-a-doi-and-how-do-i-use-them-in-citations)

	Purdue University
	(https://owl.purdue.edu/owl/research_and_citation/mla_style/
	mla_formatting_and_style_guide/mla_formatting_and_style_guide.html) */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> // POSIX-exclusive

#include "format.h" // Text formatting
#include "line.h" // File operations by the line

#define C_DEF {"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"}

bool pref_created, auto_refresh, editing, oper_success, esc;
typedef struct _citation_struct {
	char title[BUF], first[BUF], last[BUF], others[BUF];
	char city[BUF], publisher[BUF], publication[BUF], issue[BUF], page[BUF];
	char website[BUF], url[BUF], doi[BUF];
	char month[BUF], day[BUF], year[BUF], access[BUF];
} CITATION;

/* Outputs an error with message 'string' */
void biberr(char *string) {
	printf("   └ %sError:%s %s\n", E_BLD, E_CLR, string);
}

/* Prints the line 'ln_echo' from file 'path' onto the screen */
void bibecho(char *path, int ln_echo) {
	char chr_buf, str_buf[BUF];
	bool italics;
	FILE *file = fopen(path, "r");

	fseekl(file, ln_echo);
	for (;;) {
		chr_buf = fgetc(file);
		if (chr_buf == EOF || chr_buf == '\n')
			break;
		else if (chr_buf == '`' && !italics) {
			printf("%s", E_ITL);
			italics = true;
		} else if (chr_buf == '`' && italics) {
			printf("%s", E_CLR);
			italics = false;
		} else
			printf("%c", chr_buf);
	}
	printf("\n");
	fclose(file);
}

/* Creates a citation in 'file'
 * 'esc_sw' is used in case the client wishes to exit the command */
void bibcite(FILE *file, char *label, bool *esc_sw) {
	char str_buf[BUF], style[BUF], type[BUF];
	bool manual;
	CITATION Item = C_DEF;

	for (;;) {
		if (strcmp(label, "All") == 0) {
			printf("   └ Style: ");
			fgetsl(style, BUF, stdin);
		} else {
			strcpy(style, slower(label));
			printf("   └ Style: %s\n", style);
		}
		if (strcmp(style, "esc") == 0) {
			*esc_sw = true;
			break;

		/** Styled citations **/
		} else if (strcmp(style, "mla") == 0 || strcmp(style, "apa") == 0 || strcmp(style, "chicago") == 0) {
			printf("      └ Type: ");
			fgetsl(type, BUF, stdin);
			if (strcmp(type, "website") == 0 || strcmp(type, "book") == 0 || strcmp(type, "article") == 0) {

				/*** General Attributes ***/
				printf("         ├ Title: ");
				fgetsl(Item.title, BUF, stdin);
				printf("         ├ Author\n");
				printf("         │  ├ First: ");
				fgetsl(Item.first, BUF, stdin);
				printf("         │  ├ Last: ");
				fgetsl(Item.last, BUF, stdin);
				printf("         │  └ Others? (y/n): ");
				fgetsl(Item.others, BUF, stdin);

				/*** Website-specific attributes ***/
				if (strcmp(type, "website") == 0) {
					if (strcmp(style, "apa") == 0) {
						printf("         ├ URL: ");
					} else
						printf("         ├ Website: ");
						fgetsl(Item.website, BUF,stdin);
						printf("         │  └ URL: ");
					fgetsl(Item.url, BUF, stdin);
				}

				/*** Website/book-specific attributes ***/
				if (strcmp(type, "website") == 0 && strcmp(style, "apa") != 0|| strcmp(type, "book") == 0) {
					printf("         ├ Publisher: ");
					fgetsl(Item.publisher, BUF,stdin);
				}

				/*** Book-specific attributes ***/
				if (strcmp(type, "book")  == 0 && strcmp(style, "mla") != 0) {
					printf("         │  └ City: ");
					fgetsl(Item.city, BUF, stdin);
				}

				/*** Article-specific attributes ***/
				if (strcmp(type, "article") == 0) {
					printf("         ├ Publication: ");
					fgetsl(Item.publication, BUF, stdin);
					printf("         │  ├ Issue #: ");
					fgetsl(Item.issue, BUF, stdin);
					printf("         │  ├ Page(s): ");
					fgetsl(Item.page, BUF, stdin);
					printf("         │  └ DOI #: ");
					fgetsl(Item.doi, BUF, stdin);
				}

				/*** Date attributes ***/
				printf("         └ Date\n");
				printf("            ├ Year: ");
				fgetsl(Item.year, BUF, stdin);
				if (Item.year[0] != 0) {
					printf("            ├ Month: ");
					fgetsl(Item.month, BUF,stdin);
					if (Item.month[0] != 0) {
						printf("            ├ Day: ");
						fgetsl(Item.day, BUF,stdin);
					}
				printf("            └ Access date? (y/n): ");
				fgetsl(Item.access, BUF, stdin);
				}

				/*** MLA output ***/
				if (strcmp(style, "mla") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (strcmp(Item.others, "y") == 0)
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0) {
						fputs("\"", file);
						fputs(Item.title, file);
						fputs(".\" ", file);
					}
					if (Item.website[0] != 0 && strcmp(type, "website") == 0) {
						fputs(Item.website, file);
						fputs(", ", file);
					}
					if (Item.publisher[0] != 0) {
						fputs("`", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("`", file); // End italics
						fputs(", ", file);
					}
					if (Item.publication[0] != 0) {
						fputs(Item.publication, file);
						fputs(" ", file);
						if (Item.issue[0] != 0) {
							fputs("#", file);
							fputs(Item.issue, file);
							fputs(", ", file);
							if (Item.page[0] != 0) {
								fputs(Item.page, file);
								fputs(", ", file);
							}
						}
					}
					if (Item.year[0] != 0) {
						if (strcmp(Item.access, "y") == 0)
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
						if (strcmp(type, "website") == 0) 
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

				/*** APA output ***/
				} else if (strcmp(style, "apa") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (strcmp(Item.others, "y") == 0)
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.year[0] != 0) {
						fputs("(", file);
						if(strcmp(Item.access, "y") == 0)
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
						fputs("`", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("`", file); // End italics
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
						fputs(". ", file);
					}
					if (Item.doi[0] != 0) {
						fputs("https://www.doi.org/", file);
						fputs(Item.doi, file);
						fputs("doi:", file);
					}
				
				/*** Chicago-style output ***/
				} else if (strcmp(style, "chicago") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (strcmp(Item.others, "y") == 0)
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0) {
						if (strcmp(type, "book") != 0) {
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
						fputs("`", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("`", file); // End italics
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
						if (strcmp(type, "article") == 0)
							fputs("(", file);
						if(strcmp(Item.access, "y") == 0)
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
						if (strcmp(type, "article") == 0)
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
						fputs(". ", file);
					}
					if (Item.doi[0] != 0) {
						fputs("doi:", file);
						fputs(Item.doi, file);
						fputs(". ", file);
					}
				}

				fprintf(file, "\n");
				fclose(file);
				break;
			} else if (strcmp(type, "esc") == 0) {
				*esc_sw = true;
				break;
			} else {
				printf("      ");
				biberr("Unknown citation type\n\t          Use \"website\", \"book\", or \"article\"");
			}
	
		/** Plain citations **/
		} else {
			printf("      └ Manual entry: ");
			fgetsl(str_buf, BUF, stdin);
			if (str_buf[0] != 0) {
				fputs(str_buf, file);
				fputs(" \n", file);
			} else {;
				*esc_sw = true;
			}
			fclose(file);
			break;
		}

	}
}

/* Outputs the contents of 'file' with line numbers */
void bibout(char *path) {
	int chr_buf, chr_chk;
	bool italics, empty;
	int chr_count = 0;
	FILE *file = fopen(path, "r");
	fpos_t pos;

	for (;;) {
		if (fgetc(file) == '\n')
			break;
	}
	chr_chk = fgetc(file);
	if (chr_chk != EOF) {
		rewind(file);
		for (;;) {
			if (fgetc(file) == '\n')
				break;
		}
		fseek(file, -1, SEEK_CUR);
		printf(" ┌───────────────────────────────────────────────────────────────────────────┐\n"); // Beginning of bibliography UI
		for (int ln_num = 1;; ln_num++) {
			chr_buf = fgetc(file);
			chr_chk = fgetc(file);
			if ((chr_buf == '\n' && chr_chk == EOF) || (chr_buf == EOF && chr_chk == EOF))
				break;
			ungetc(chr_chk, file);
			printf(" │ %d. ", ln_num);
			for (;;) {
				chr_buf = fgetc(file);
				chr_chk = fgetc(file);
				ungetc(chr_chk, file);
				chr_count++;
				if (chr_chk == '\n') {
					break;
				}
				if (chr_buf == EOF || chr_buf == '\n')
					break;
				if (chr_count >= 71) {
					printf(" │▒\n │    ");
					chr_count = 1;
				}
				if (chr_buf == '`' && !italics) {
					printf("%s", E_ITL);
					italics = true;
					chr_count--;
				} else if (chr_buf == '`' && italics) {
					printf("%s", E_CLR);
					italics = false;
					chr_count--;
				} else
					printf("%c", chr_buf);
			}
			for (chr_count; chr_count < 71; chr_count++) {
				printf(" ");
			}
			chr_count = 0;
			printf(" │▒\n │                                                                           │▒\n");
		}
		printf(" └───────────────────────────────────────────────────────────────────────────┘▒\n");
		printf("  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒\n"); // End of bibliography UI
	}
	fclose(file);
}

/* Outputs the header and help page */
void bibhelp(void) {
	system("clear");
	printf(" 1.5.0                                                           Help Page  n/a\n⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺\n");
	/** Help Page **/
	puts(  "   ↑                                                                 ↑       ↑");
	printf(  "%sVersion                                                             File   Style%s\n\n", E_ITL, E_CLR);

	printf("%sCommands%s\n", E_BLD, E_CLR);
	printf("%sc%site: Creates a citation.\n", E_UND, E_CLR);
	printf("%sp%sref: Configures preferences.\n", E_UND, E_CLR);
	printf("%sq%suit: Exits the program.\n", E_UND, E_CLR);
	printf("%sh%selp: Displays the help page.\n", E_UND, E_CLR);
	printf("%sw%sipe: Deletes all entries in a bibliography.\n", E_UND, E_CLR);
	printf("%sd%selete: Deletes an entry.\n", E_UND, E_CLR);
	printf("%se%scho: Returns a citation.\n", E_UND, E_CLR);
	printf("%sm%sodify: Modifies an entry.\n", E_UND, E_CLR);
	printf("%sr%sefresh: Updates file contents.\n\n", E_UND, E_CLR);

	printf("Press [ENTER] to continue.");
	fgetc(stdin);
}

/* Outputs the header for an open 'file'
 * 'file' points to file name 'path'
 * 'open_sw' is used in case of read failure */
void bibtitle(char *path, char *label) {
	FILE *file = fopen(path, "a");

	system("clear");
	printf(" 1.5.0        ");
	for (int i = 0; i < 63 - strlen(path) - strlen(label); i++)
		printf(" ");
	printf("%s  %s\n⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺\n", path, label);
	fclose(file);
	file = fopen(path, "r");
	bibout(path);
	printf("\n");
	fclose(file);
}

/* Main */
int main(int argc, char *argv[]) {
	char cmd_symbol, chr_input, str_input[BUF], label[BUF], path[BUF];
	int num_input;
	FILE *fptr;

	/** Check # of arguments **/
	if (argc > 2) {
		puts("Usage: biblio [FILE]");
		puts("Try 'biblio --help' for more information.");
		exit(EXIT_SUCCESS);
	}
	if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			puts("Usage: biblio [FILE]");
			puts("Example: biblio 'hello world'");
			puts("Open and edit bibliography (.bib) file with name FILE.\n");

			puts("If no FILE is specified, default bibliography will be opened.");
			puts("If no bibliography file of name FILE is found, one is made.\n");

			puts("Optional flags");
			puts("  -h, --h\tShow help\n");

			puts("Report bugs to: crypticcu@protonmail.com");
			puts("Github page:    https://github.com/crypticcu/biblio");
			exit(EXIT_SUCCESS);
		}
		else if (argv[1][0] == '-') {
			puts("Error: Incorrect flag usage");
			puts("Try 'biblio --help' for more information.");
			exit(EXIT_FAILURE);
		}
		strncpy(path, argv[1], 261);
		strcat(path, ".bib"); // Ensures that a bibliography file is created
	} else
		strcpy(path, ".bib");

	/** Check file availability **/
	fptr = fopen(path, "a");
	if (fptr == NULL) {
		printf("Error: File cannot be reached");
		exit(EXIT_FAILURE);
	}
	fclose(fptr);
	
	/** Check styling label **/
	fptr = fopen(path, "r");
	if (fgetc(fptr) == EOF) { // Add default label if file is empty
		fclose(fptr);
		fptr = fopen(path, "w");
		fputs("#All\n", fptr);
	}
	fclose(fptr);
	fptr = fopen(path, "r");
	if (fgetc(fptr) == '#') {
		fgetsl(label, BUF, fptr);
	} else {
		puts("Error: Styling label not found");
		exit(EXIT_FAILURE);
	}
	fclose(fptr);
	

	/** Check and apply preferences **/
	if (access(".conf", R_OK|W_OK) == -1) {
		fptr = fopen(".conf", "a");
		fprintf(fptr, "Auto-refresh:\noff\n\nCommand symbol:\n>\n");
		cmd_symbol = '>';
	} else {
		fptr = fopen(".conf", "r");
		fseekl(fptr, 2); // Auto-refresh
		fgetsl(str_input, BUF, fptr);
		if (strcmp(str_input, "on") == 0)
			auto_refresh = true;
		fseekl(fptr, 5); // Command-line symbol
		cmd_symbol = fgetc(fptr);
	}

	/** Check default bibliography **/
	if (access(".bib", R_OK|W_OK) == -1) {
		fptr = fopen(".bib", "a");
		fclose(fptr);
	}

	fptr = fopen(path, "r");
	bibtitle(path, label);

	/** Program loop **/
	for (;;) {

		/*** Command line ***/
		printf(" %c ", cmd_symbol); // Ensures command line cursor of user's choice
		fgetsl(str_input, BUF, stdin);

		/*** 'modify' command ***/
		if (strcmp(str_input, "modify") == 0 || strcmp(str_input, "m") == 0) {
			printf("   ├ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			num_input = atoi(str_input);
			if (num_input < 1 || num_input > fcountl(path)) {
				biberr("Citation does not exist");
			} else if (strcmp(str_input, "esc") != 0) // Breaks if 'esc' is input
				editing = true; // Allows for 'cite' command to commence right afterwards
			fsort(path, A_MOD|N_MOD);
			oper_success = true;
		}

		/*** 'cite' command ***/
		if (strcmp(str_input, "cite") == 0 || strcmp(str_input, "c") == 0 || editing) {
			fptr = fopen(".temp", "w");
			bibcite(fptr, label, &esc);
			if(!esc) { // Breaks if 'esc' is input
				fptr = fopen(".temp", "r");
				if (editing) { // If came from 'modify' command...
					fgets(str_input, BUF, fptr);
					fmodil(path, num_input, str_input, false);
					editing = false; // Ensures that 'cite' command is not guaranteed to run afterwards
				} else { // Else...
					fgets(str_input, BUF, fptr);
					fclose(fptr);
					fptr = fopen(path, "a");
					fputs(str_input, fptr);
					fclose(fptr);
				}
				if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(path, label);
			}
			fsort(path, A_MOD|N_MOD);
			oper_success = true;
		}

		/*** 'wipe' command ***/
		if (strcmp(str_input, "wipe") == 0 || strcmp(str_input, "w") == 0) {
			printf("   └ Are you sure? (y/n): ");
			fgetsl(str_input, BUF, stdin);
			if(strcmp(str_input, "y") == 0) {
				fptr = fopen(path, "w");
				fprintf(fptr, "#%s\n", label);
				fclose(fptr);
				if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(path, label);
			}
			oper_success = true;
		}

		/*** 'preferences' command ***/
		if (strcmp(str_input, "preferences") == 0 || strcmp(str_input, "p") == 0) {
			printf("   ├ Auto-refresh: "); // Configuration for auto-refresh
			fgetsl(str_input, BUF, stdin);
			if(strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
				if (strcmp(str_input, "on") == 0)
					auto_refresh = true;
				else if (strcmp(str_input, "off") == 0)
					auto_refresh = false;
				fmodil(".conf", 2, str_input, true);
				printf("   └ Command symbol: "); // Configuration for command line symbol
				fgetsl(str_input, BUF, stdin);
				if(strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
					fmodil(".conf", 5, str_input, true);
					cmd_symbol = str_input[0];
				}
			}
			oper_success = true;
		}

		/*** 'delete' command ***/
		if (strcmp(str_input, "delete") == 0 || strcmp(str_input, "d") == 0) {
			printf("   └ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			if (atoi(str_input) < 1 || atoi(str_input) > fcountl(path)) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			}
			else if (strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
				fdell(path, atoi(str_input));
			}
			if (auto_refresh) // Refreshes automatically if auto-refresh is on
				bibtitle(path, label);
			oper_success = true;
		}

		/*** 'echo' command ***/
		if (strcmp(str_input, "echo") == 0 || strcmp(str_input, "e") == 0) {
			printf("   └ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			if (atoi(str_input) < 1 || atoi(str_input) > fcountl(path)) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			} else
				bibecho(path, atoi(str_input));
			oper_success = true;
		}

		/*** 'quit' command ***/
		if (strcmp(str_input, "quit") == 0 || strcmp(str_input, "q") == 0) {
			system("clear"); // Leaves no trace of program
			oper_success = true;
			break; // Breaks out of program loop
		}

		/*** 'help' command ***/
		if (strcmp(str_input, "help") == 0 || strcmp(str_input, "h") == 0) {
			bibhelp();
			bibtitle(path, label);
			oper_success = true;
		}

		/*** 'label' command ***/
		if (strcmp(str_input, "label") == 0 || strcmp(str_input, "l") == 0) {
			printf("   └ New styling label: ");
			fgetsl(label, BUF, stdin);
			strcpy(str_input, "#");
			strcat(str_input, label);
			fmodil(path, 1, str_input, true);
			bibtitle(path, label);
			oper_success = true;
		}

		/*** 'refresh' command ***/
		if (strcmp(str_input, "refresh") == 0 || strcmp(str_input, "r") == 0) {
			bibtitle(path, label); // Refreshes the screen
			oper_success = true;
		}

		/*** Checks for invalid command ***/
		if (!oper_success)
			biberr("Unknown command");

		esc = false;
		oper_success = false;
		
	}
	
	remove(".temp");
	return EXIT_SUCCESS;
}
