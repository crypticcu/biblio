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
#include <ioplus.h> // Additional I/O functions and macros

/* #define DEBUG */ // Uncomment to prevent screen clearing
#define VER		"1.6.0" // Program version
#define C_DEF	{0,0,X,X,X,X,X,X,X,X,X,X,0,0,0,0} // Initial conditions of CITATION type 

bool pref_created, auto_refresh, editing, oper_success, esc;
typedef struct _citation_struct {
	char others, access, *title, *first, *last, *city, *publisher, *publication, *website, *url, *doi, *month;
	int issue, page, day, year;
} CITATION;

/* Outputs an error with message 'string' */
void biberr(char *string) {
	printf("   └ %sError:%s %s\n", E_BLD, E_CLR, string);
}

/* Prints the line 'ln_echo' from file 'path' onto the screen */
void bibecho(char *path, int ln_echo) {
	int size = fcountl(path);
	char chr;
	bool italics;
	FILE *file = fopen(path, "r");

	fseekl(file, ln_echo);
	for (; (chr = fgetchr(file)) != EOF && chr != '\n'; ) {
		if (chr == '`' && !italics) {
			printf("%s", E_ITL);
			italics = true;
		} else if (chr == '`' && italics) {
			printf("%s", E_CLR);
			italics = false;
		} else
			printf("%c", chr);
	}
	printf("\n");
	fclose(file);
}

/* Creates a citation in 'file'
 * 'escape' is used in case the client wishes to exit the command */
void bibcite(FILE *file, char *label, bool *escape) {
	char *str = NULL, *style = NULL, *type = NULL;
	bool manual;
	CITATION Item = C_DEF;

	for (;;) {
		if (strcmp(label, "All") == 0) {
			printf("   └ Style: ");
			style = agetstr();
		} else {
			style = (char *) malloc(strlen(label) + 1);
			strcpy(style, slower(label));
			printf("   └ Style: %s\n", style);
		}
		if (strcmp(style, "esc") == 0) {
			*escape = true;
			break;

		/* Styled citations */
		} else if (strcmp(style, "mla") == 0 || strcmp(style, "apa") == 0 || strcmp(style, "chicago") == 0) {
			printf("      └ Type: ");
			type = agetstr();
			if (strcmp(type, "website") == 0 || strcmp(type, "book") == 0 || strcmp(type, "article") == 0) {

				/* General Attributes */
				printf("         ├ Title: ");
				Item.title = agetstr();
				printf("         ├ Author\n");
				printf("         │  ├ First: ");
				Item.first = agetstr();
				printf("         │  ├ Last: ");
				Item.last = agetstr();
				printf("         │  └ Others? (y/n): ");
				Item.others = getchr();

				/* Website-specific attributes */
				if (strcmp(type, "website") == 0) {
					if (strcmp(style, "apa") == 0) {
						printf("         ├ URL: ");
					} else {
						printf("         ├ Website: ");
						Item.website = agetstr();
						printf("         │  └ URL: ");
					}
					Item.url = agetstr();
				}

				/* Website/book-specific attributes */
				if (strcmp(type, "website") == 0 && strcmp(style, "apa") != 0|| strcmp(type, "book") == 0) {
					printf("         ├ Publisher: ");
					Item.publisher = agetstr();
				}

				/* Book-specific attributes */
				if (strcmp(type, "book")  == 0 && strcmp(style, "mla") != 0) {
					printf("         │  └ City: ");
					Item.city = agetstr();
				}

				/* Article-specific attributes */
				if (strcmp(type, "article") == 0) {
					printf("         ├ Publication: ");
					Item.publication = agetstr();
					printf("         │  ├ Issue #: ");
					Item.issue = getint();
					printf("         │  ├ Page(s): ");
					Item.page = getint();
					printf("         │  └ DOI #: ");
					Item.doi = agetstr();
				}

				/* Date attributes */
				printf("         └ Date\n");
				printf("            ├ Year: ");
				Item.year = getint();
				if (Item.year != 0) {
					printf("            ├ Month: ");
					Item.month = agetstr();
					if (Item.month[0] != 0) {
						printf("            ├ Day: ");
						Item.day = getint();
					}
				printf("            └ Access date? (y/n): ");
				Item.access = getchr();
				}

				/* MLA output */
				if (strcmp(style, "mla") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fprintf(file, "%s, %s", Item.last, Item.first);
						if (Item.others == 'y')
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0)
						fprintf(file, "\"%s.\" ", Item.title);
					if (Item.website[0] != 0 && strcmp(type, "website") == 0)
						fprintf(file, "%s, ", Item.website);
					if (Item.publisher[0] != 0)
						fprintf(file, "`%s`, ", Item.publisher);
					if (Item.publication[0] != 0) {
						fprintf(file, "%s ", Item.publication);
						if (Item.issue != 0) {
							fprintf(file, "#%d, ", Item.issue);
							if (Item.page != 0)
								fprintf(file, "%d, ", Item.page);
						}
					}
					if (Item.year != 0) {
						if (Item.access == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fprintf(file, "%s ", Item.month);
							if (Item.day != 0)
								fprintf(file, "%d, ", Item.day);
						}
						fprintf(file, "%d", Item.year);
						if (strcmp(type, "website") == 0)
							fputs(", ", file);
						else
							fputs(". ", file);
					}
					if (Item.url[0] != 0)
						fprintf(file, "%s. ", Item.url);
					if (Item.doi[0] != 0)
						fprintf(file, "doi:%s. ", Item.doi);

				/* APA output */
				} else if (strcmp(style, "apa") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fprintf(file, "%s, %s", Item.last, Item.first);
						if (Item.others == 'y')
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.year != 0) {
						fputs("(", file);
						if(Item.access == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fprintf(file, "%s ", Item.month);
							if (Item.day != 0)
								fprintf(file, "%d, ", Item.day);
						}
						fprintf(file, "%d). ", Item.year);
					}
					if (Item.title[0] != 0)
						fprintf(file, "%s. ", Item.title);
					if (Item.city[0] != 0)
						fprintf(file, "%s: ", Item.city);
					if (Item.publisher[0] != 0)
						fprintf(file, "`%s`. ", Item.publisher);
					if (Item.publication[0] != 0) {
						fprintf(file, "%s, ", Item.publication);
						if (Item.issue != 0) {
							fprintf(file, "#%d, ", Item.issue);
							if (Item.page != 0)
								fprintf(file, "%d. ", Item.page);
						}
					}
					if (Item.url[0] != 0)
						fprintf(file, "Retrieved from %s. ", Item.url);
					if (Item.doi[0] != 0)
						fprintf(file, "https://www.doi.org/%s. ", Item.doi);
				
				/* Chicago-style output */
				} else if (strcmp(style, "chicago") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fprintf(file, "%s, %s", Item.last, Item.first);
						if (Item.others == 'y')
							fputs(" et al", file);
						fputs(". ", file);
					}
					if (Item.title[0] != 0) {
						if (strcmp(type, "book") != 0)
							fprintf(file, "\"%s.\" ", Item.title);
						else
							fprintf(file, "%s. ", Item.title);
					}
					if (Item.city[0] != 0)
						fprintf(file, "%s: ", Item.city);
					if (Item.publisher[0] != 0)
						fprintf(file, "`%s`, ", Item.publisher);
					if (Item.publication[0] != 0) {
						fprintf(file, "%s, ", Item.publication);
						if (Item.issue != 0)
							fprintf(file, "%d ", Item.issue);
					}
					if (Item.year != 0) {
						if (strcmp(type, "article") == 0)
							fputs("(", file);
						if(Item.access == 'y')
							fputs("accessed ", file);
						if (Item.month[0] != 0) {
							fprintf(file, "%s ", Item.month);
							if (Item.day != 0)
								fprintf(file, "%d, ", Item.day);
						}
						fprintf(file, "%d", Item.year);
						if (strcmp(type, "article") == 0)
							fputs("): ", file);
						else
							fputs(". ", file);
					}
					if (Item.page != 0)
						fprintf(file, "%d. ", Item.page);
					if (Item.url[0] != 0)
						fprintf(file, "%s. ", Item.url);
					if (Item.doi[0] != 0)
						fprintf(file, "doi:%s. ", Item.doi);
				}

				fprintf(file, "\n");
				fclose(file);
				break;
			} else if (strcmp(type, "esc") == 0) {
				*escape = true;
				break;
			} else {
				printf("      ");
				biberr("Unknown citation type\n\t          Use \"website\", \"book\", or \"article\"");
			}
	
		/* Plain citations */
		} else {
			printf("      └ Manual entry: ");
			str = agetstr();
			if (str[0] != 0)
				fprintf(file, "%s \n", str);
			else
				*escape = true;
			fclose(file);
			break;
		}

	}
	bin(13, Item.title, Item.first, Item.last, Item.city, Item.publisher, Item.publication, Item.website, Item.url, Item.doi, Item.month, str, style, type);
}

/* Outputs the contents of 'file' with line numbers */
void bibout(char *path) {
	int chrA, chrB, chr_count = 0;
	bool italics, empty;
	FILE *file = fopen(path, "r");

	fseekl(file, 2);
	chrB = fgetchr(file);
	if (chrB != EOF) {
		fseekl(file, 2);
		fseek(file, -1, SEEK_CUR);
		printf(" ┌───────────────────────────────────────────────────────────────────────────┐\n"); // Beginning of bibliography UI
		for (int ln_num = 1;; ln_num++) {
			chrA = fgetchr(file);
			chrB = fgetchr(file);
			if ((chrA == '\n' && chrB == EOF) || (chrA == EOF && chrB == EOF))
				break;
			ungetc(chrB, file);
			printf(" │ %d. ", ln_num);
			for (;;) {
				chrA = fgetchr(file);
				chrB = fgetchr(file);
				ungetc(chrB, file);
				chr_count++;
				if (chrB == '\n') {
					break;
				}
				if (chrA == EOF || chrA == '\n')
					break;
				if (chr_count >= 71) {
					printf(" │▒\n │    ");
					chr_count = 1;
				}
				if (chrA == '`' && !italics) {
					printf("%s", E_ITL);
					italics = true;
					chr_count--;
				} else if (chrA == '`' && italics) {
					printf("%s", E_CLR);
					italics = false;
					chr_count--;
				} else
					printf("%c", chrA);
			}
			for (chr_count; chr_count < 71; chr_count++) {
				printf(" ");
			}
			chr_count = 0;
			printf(" │▒\n │                                                                           │▒\n");
		}
		printf(" └───────────────────────────────────────────────────────────────────────────┘▒\n  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒\n"); // End of bibliography UI
	}
	fclose(file);
}

/* Outputs the header and help page */
void bibhelp(void) {
	#ifndef	DEBUG
	system("clear");
	#endif
	printf(" %s                                                           Help Page  n/a\n⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺\n", VER);
	/* Help Page */
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
	getchr();
}

/* Outputs the header for an open 'file'
 * 'file' points to file name 'path'
 * 'open_sw' is used in case of read failure */
void bibtitle(char *path, char *label) {
	FILE *file = fopen(path, "a");

	#ifndef DEBUG
	system("clear"); // Leaves no trace of previous display
	#endif
	printf(" %s  ", VER);
	for (int i = 0; i < 69 - strlen(path) - strlen(label); i++)
		printf(" ");
	printf("%s  %s\n⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺⎺\n", path, label);
	fclose(file);
	file = fopen(path, "r");
	bibout(path);
	puts("\n");
	fclose(file);
}

/* Main */
int main(int argc, char *argv[]) {
	char chr, cmd_symbol, *str = NULL, *label = NULL, *path = NULL;
	int num;
	FILE *fptr;

	/* Check # of arguments */
	if (argc > 2) { // Two or more arguments
		puts("Usage: biblio [FILE]");
		puts("Try 'biblio --help' for more information.");
		exit(EXIT_SUCCESS);
	}
	if (argc == 2) { // One argument
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			puts("Usage: biblio [FILE]");
			puts("Example: biblio 'hello world'");
			puts("Open and edit bibliography (.bib) file with name FILE.\n");

			puts("If no FILE is specified, default bibliography will be opened.");
			puts("If no bibliography file of name FILE is found, one is made.\n");

			puts("Optional flags");
			puts("  -h, --help\tShow help\n");

			puts("Report bugs to: crypticcu@protonmail.com");
			puts("Github page:    https://github.com/crypticcu/biblio");
			exit(EXIT_SUCCESS);
		}
		else if (argv[1][0] == '-') {
			puts("Error: Incorrect flag usage");
			puts("Try 'biblio --help' for more information.");
			exit(EXIT_FAILURE);
		}
		path = (char *) malloc(61);
		strncpy(path, argv[1], 60); // Any file name over 50 characters would overflow status bar
		path[strlen(path) + 1] = 0;
		path = strcat(path, ".bib"); // Ensures that a bibliography file is created
	} else { // No arguments
		path = (char *) malloc(5);
		strcpy(path, ".bib");
	}

	/* Check file availability */
	fptr = fopen(path, "a");
	if (fptr == NULL) {
		puts("Error: File cannot be reached");
		exit(EXIT_FAILURE);
	}
	fclose(fptr);

	/* Check styling label */
	fptr = fopen(path, "r");
	if (fgetchr(fptr) == EOF) { // Add default label if file is empty
		fclose(fptr);
		fptr = fopen(path, "w");
		fputs("#All\n", fptr); // Default styling label
	}
	fclose(fptr);
	fptr = fopen(path, "r");
	if (fgetchr(fptr) == '#') {
		label = afngetstr(fptr, 9);
	} else {
		puts("Error: Styling label not found");
		exit(EXIT_FAILURE);
	}
	fclose(fptr);

	/* Check and apply preferences */
	if (access(".conf", R_OK|W_OK) == -1) {
		fptr = fopen(".conf", "a");
		fputs("Auto-refresh:\noff\n\nCommand symbol:\n>\n", fptr);
		cmd_symbol = '>';
	} else {
		fptr = fopen(".conf", "r");
		fseekl(fptr, 2); // Auto-refresh
		str = afgetstr(fptr);
		if (strcmp(str, "on") == 0)
			auto_refresh = true;
		fseekl(fptr, 5); // Command-line symbol
		cmd_symbol = fgetchr(fptr);
	}

	/* Check default bibliography */
	if (access(".bib", R_OK|W_OK) == -1) {
		fptr = fopen(".bib", "a");
		fclose(fptr);
	}

	fptr = fopen(path, "r");
	bibtitle(path, label);

	/* Program loop */
	for (;;) {

		/* Command line */
		printf(" %c ", cmd_symbol); // Ensures command line cursor of user's choice
		str = agetstr();

		/* 'modify' command */
		if (strcmp(str, "modify") == 0 || strcmp(str, "m") == 0) {
			printf("   ├ Citation #: ");
			str = agetstr();
			num = atoi(str) + 1;
			if (num < 2 || num > fcountl(path) - 1) {
				biberr("Citation does not exist");
			} else if (strcmp(str, "esc") != 0) // Breaks if 'esc' is input
				editing = true; // Allows for 'cite' command to commence right afterwards
			fsort(path, A_MOD|N_MOD);
			oper_success = true;
		}

		/* 'cite' command */
		if (strcmp(str, "cite") == 0 || strcmp(str, "c") == 0 || editing) {
			fptr = fopen(".temp", "w");
			bibcite(fptr, label, &esc);
			if(!esc) { // Breaks if 'esc' is input
				fptr = fopen(".temp", "r");
				if (editing) { // If came from 'modify' command...
					str = afgetstr(fptr);
					fmodil(path, num, str, false);
					editing = false; // Ensures that 'cite' command is not guaranteed to run afterwards
				} else { // Else...
					str = afgetstr(fptr);
					fptr = fopen(path, "a");
					fputs(str, fptr);
					fclose(fptr);
				}
				if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(path, label);
			}
			fsort(path, A_MOD|N_MOD);
			oper_success = true;
		}

		/* 'wipe' command */
		if (strcmp(str, "wipe") == 0 || strcmp(str, "w") == 0) {
			printf("   └ Are you sure? (y/n): ");
			chr = getchr();
			if(chr == 'y') {
				fptr = fopen(path, "w");
				fprintf(fptr, "#%s\n", label);
				fclose(fptr);
				if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(path, label);
			}
			oper_success = true;
		}

		/* 'preferences' command */
		if (strcmp(str, "preferences") == 0 || strcmp(str, "p") == 0) {
			printf("   ├ Auto-refresh: "); // Configuration for auto-refresh
			str = agetstr();
			if(strcmp(str, "esc") != 0) { // Breaks if 'esc' is input
				if (strcmp(str, "on") == 0)
					auto_refresh = true;
				else if (strcmp(str, "off") == 0)
					auto_refresh = false;
				fmodil(".conf", 2, str, true);
				printf("   └ Command symbol: "); // Configuration for command line symbol
				str = agetstr();
				if(strcmp(str, "esc") != 0) { // Breaks if 'esc' is input
					fmodil(".conf", 5, str, true);
					cmd_symbol = str[0];
				}
			}
			oper_success = true;
		}

		/* 'delete' command */
		if (strcmp(str, "delete") == 0 || strcmp(str, "d") == 0) {
			printf("   └ Citation #: ");
			str = agetstr();
			if (atoi(str) < 1 || atoi(str) > fcountl(path) - 1) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			}
			else if (strcmp(str, "esc") != 0) { // Breaks if 'esc' is input
				fdell(path, atoi(str) + 1);
			}
			if (auto_refresh) // Refreshes automatically if auto-refresh is on
				bibtitle(path, label);
			oper_success = true;
		}

		/* 'echo' command */
		if (strcmp(str, "echo") == 0 || strcmp(str, "e") == 0) {
			printf("   └ Citation #: ");
			str = agetstr();
			if (atoi(str) < 1 || atoi(str) > fcountl(path) - 1) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			} else
				bibecho(path, atoi(str));
			oper_success = true;
		}

		/* 'quit' command */
		if (strcmp(str, "quit") == 0 || strcmp(str, "q") == 0) {
			#ifndef DEBUG
			system("clear"); // Leaves no trace of program
			#endif
			oper_success = true;
			break; // Breaks out of program loop
		}

		/* 'help' command */
		if (strcmp(str, "help") == 0 || strcmp(str, "h") == 0) {
			bibhelp();
			bibtitle(path, label);
			oper_success = true;
		}

		/* 'label' command */
		if (strcmp(str, "label") == 0 || strcmp(str, "l") == 0) {
			printf("   └ New styling label: ");
			str = agetstr();
			if (label[0] != 0 && strcmp(label, "esc") == 0) {
				label = strcpy(str, "#");
				label = strcat(label, str);
				fmodil(path, 1, str, true);
				bibtitle(path, label);
			}
			oper_success = true;
		}

		/* 'refresh' command */
		if (strcmp(str, "refresh") == 0 || strcmp(str, "r") == 0) {
			bibtitle(path, label); // Refreshes the screen
			oper_success = true;
		}

		/* Checks for invalid command */
		if (!oper_success)
			biberr("Unknown command");

		esc = false;
		oper_success = false;
		
	}
	bin(3, str, label, path);
	remove(".temp");
	return EXIT_SUCCESS;
}
