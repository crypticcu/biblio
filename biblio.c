/*	LICENSING
	──────────────────────────────────────────────────────────────────────
	Biblio v1.4.1 - command-line bibliography creator and editor
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
	Amanda Howell
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

bool pref_created, auto_refresh, editing, oper_success, esc;
struct Citation {
	char title[BUF];
	char first[BUF];
	char last[BUF];
	char others[BUF];
	
	char city[BUF];
	char publisher[BUF];
	char publication[BUF];
	char issue[BUF];
	char page[BUF];

	char website[BUF];
	char url[BUF];
	char doi[BUF];

	char month[BUF];
	char day[BUF];
	char year[BUF];
	char access[BUF];
} Item;

/* Outputs an error with message 'string' */
void biberr(char *string) {
	printf("   └");
	txtb();
	printf(" Error: ");
	txtr();
	printf("%s\n", string);
}

void bibecho(FILE *file, char *path, int ln_echo) {
	char chr_buf, str_buf[BUF];
	bool italics;

	file = fopen(path, "r");
	fseekl(file, ln_echo);
	for (;;) {
		chr_buf = fgetc(file);
		if (chr_buf == EOF || chr_buf == '\n')
			break;
		else if (chr_buf == '_' && !italics) {
			txti();
			italics = true;
		} else if (chr_buf == '_' && italics) {
			txtr();
			italics = false;
		} else
			printf("%c", chr_buf);
	}
	printf("\n");
	rewind(file);
}

/* Creates a citation in 'file'
 * 'editing_sw' is used in case of modification
 * 'esc_sw' is used in case the client wishes to exit the command */
void bibcite(FILE *file, bool *editing_sw, bool *esc_sw) {
	char str_buf[BUF], style[BUF], type[BUF];

	for (;;) {
		printf("   └ Style: ");
		fgetsl(style, BUF, stdin);

		/** Styled citations **/
		if (strcmp(style, "mla") == 0 || strcmp(style, "apa") == 0 || strcmp(style, "chicago") == 0) {
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
					if (strcmp(style, "apa") == 0)
						printf("         │  └ DOI link: ");
					else
						printf("         │  └ DOI num: ");
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
						if (Item.others[0] == 'y')
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
						fputs("_", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("_", file); // End italics
						fputs(", ", file);
					}
					if (Item.publication[0] != 0) {
						fputs(Item.publication, file);
						fputs(" ", file);
						if (strcmp(Item.issue, "") != 0) {
							fputs("#", file);
							fputs(Item.issue, file);
							fputs(", ", file);
							if (strcmp(Item.page, "") != 0) {
								fputs(Item.page, file);
								fputs(", ", file);
							}
						}
					}
					if (Item.year[0] != 0) {
						if (Item.access[0] == 'y')
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
						fputs("_", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("_", file); // End italics
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
						fputs(Item.doi, file);
					}
				
				/*** Chicago-style output ***/
				} else if (strcmp(style, "chicago") == 0) {
					if (Item.first[0] != 0 && Item.last[0] != 0) {
						fputs(Item.last, file);
						fputs(", ", file);
						fputs(Item.first, file);
						if (Item.others[0] != 0)
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
						fputs("_", file); // Begin italics
						fputs(Item.publisher, file);
						fputs("_", file); // End italics
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
					}
					if (Item.doi[0] != 0) {
						fputs("doi:", file);
						fputs(Item.doi, file);
					}
				}

				fprintf(file, "\n");
				fclose(file);
				break;
			} else if (strcmp(type, "esc") == 0) {
				*editing_sw = false;
				*esc_sw = true;
				break;
			} else {
				printf("      ");
				biberr("Unknown citation type\n\t          Use \"website\", \"book\", or \"article\"");
			}
	
		/** Plain citations **/
		} else if (strcmp(style, "manual") == 0) {

				/*** Manual entry ***/
				printf("      └ Manual entry: ");
				fgetsl(str_buf, BUF, stdin);

				/*** Output ***/
				fputs(str_buf, file);
				fprintf(file, "\n");
				fclose(file);
				break;

		} else if (strcmp(style, "esc") == 0) {
			*editing_sw = false;
			*esc_sw = true;
			break;
		} else {
			printf("   ");
			biberr("Unknown citation style\n\t       Use \"mla\", \"apa\", \"chicago\", or \"manual\"");
		}

	}
}

/* Outputs the contents of 'file' with line numbers */
void bibout(FILE *file) {
	char chr_buf;
	bool italics;
	int chr_count = 0;
	fpos_t pos;

	printf(" ┌───────────────────────────────────────────────────────────────────────────┐\n"); // Beginning of bibliography UI
	for (int ln_num = 1;; ln_num++) {
		printf(" │ %d. ", ln_num);
		for (;;) {
			chr_buf = fgetc(file);
			chr_count++;
			if (chr_buf == EOF || chr_buf == '\n')
				break;
			if (chr_count >= 71) {
				printf(" │▒\n │    ", chr_buf);
				chr_count = 1;
			}
			if (chr_buf == '_' && !italics) {
				txti();
				italics = true;
				chr_count--;
			} else if (chr_buf == '_' && italics) {
				txtr();
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
		fgetpos(file, &pos);
		if (fgetc(file) == EOF)
			break;
		fsetpos(file, &pos);
	}
	printf(" └───────────────────────────────────────────────────────────────────────────┘▒\n");
	printf("  ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒\n"); // End of bibliography UI
}

/* Outputs the header and help page */
void bibhelp(void) {
	system("clear");
	printf("Biblio v1.4.1 » ");
	txtb();
	puts("Help Page\n");
	txtr();

	/** Help Page **/
	txtb();
	puts("Commands");
	txtr();
	puts("cite: Creates a citation.");
	puts("config: Configures preferences.");
	puts("exit: Exits the program.");
	puts("help: Displays the help page.");

	txti();
	puts("While a bibliography is open...");
	txtr();
	puts("clear: Deletes all entries in a bibliography.");
	puts("delete: Deletes an entry.");
	puts("echo: Returns a citation");
	puts("modify: Modifies an entry.");
	puts("move: Moves an entry to a position.");
	puts("refresh: Updates file contents.\n");

	txtb();
	puts("Tips");
	txtr();
	puts(" * Citations should be in alphabetical order");
	puts(" * Use only one citation style per document");
	puts(" * Leave all unknown fields blank\n\n\n");

	printf("Press [ENTER] to continue.");
	fgetc(stdin);
}

/* Outputs the header for an open 'file'
 * 'file' points to file name 'path'
 * 'open_sw' is used in case of read failure */
void bibtitle(FILE *file, char *path) {
	system("clear");
	printf("Biblio v1.4.1 » ");
	txtb();
	printf("%s\n\n", path);
	txtr();
	file = fopen(path, "a");
	if (file == NULL) {
		txtb();
		printf("Error: ");
		txtr();
		puts("Unable to reach file");
		exit(EXIT_FAILURE);
	} else {
		file = fopen(path, "r");
		bibout(file);
		fclose(file);
		printf("\n");
	}
}

/* Main */
int main(int argc, char *argv[]) {
	char cmd_symbol, chr_input, str_input[BUF], path[BUF] = ".bib";
	int num_input;
	FILE *fptr;

	/** For invalid argument # **/
	if (argc > 2) {
		puts("Usage: biblio [FILE]");
		puts("Try 'biblio --help' for more information.");
		exit(EXIT_SUCCESS);
	}

	/** Flags **/
	if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			puts("Usage: biblio [FILE]");
			puts("Open and edit bibliography file with name FILE.");
			puts("Example: biblio 'hello world'");
			puts("If no FILE is specified, default bibliography will be opened.");
			puts("If no bibliography file of name FILE is found, one is made.\n");

			puts("Report bugs to: crypticcu@protonmail.com");
			puts("Biblio git page: <https://github.com/crypticcu/biblio>");
			exit(EXIT_SUCCESS);
		}
	}

	/** Open file **/
	if (argc > 1) {
		for (int i = 0;; i++) {
			path[i] = argv[1][i];
			if (argv[1][i] == 0)
				break;
		}
		strcat(path, ".bib"); // Ensures that a bibliography file is created
	}
	bibtitle(fptr, path);

	/** Check and apply preferences **/
	if (access(".conf", R_OK|W_OK) == -1) {
		fptr = fopen(".conf", "a");
		fprintf(fptr, "Auto-refresh:\noff\n\nCommand symbol:\n>\n");
		cmd_symbol = '>';
	} else {
		fptr = fopen(".conf", "r");
		fseekl(fptr, 2); // Auto-refresh
		fgets(str_input, BUF, fptr);
		if (strcmp(str_input, "on") == 0)
			auto_refresh = true;
		fseekl(fptr, 5); // Command-line symbol
		cmd_symbol = fgetc(fptr);
	}

	/** Check default bibliography **/
	if (access(".bib", R_OK|W_OK) == -1) {
		fptr = fopen(".bib", "a");
	}

	/** Program loop **/
	for (;;) {

		/*** Command line ***/
		printf("%c ", cmd_symbol); // Ensures command line cursor of user's choice
		fgetsl(str_input, BUF, stdin);

		/*** 'modify' command ***/
		if (strcmp(str_input, "modify") == 0) {
			printf("   ├ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			num_input = atoi(str_input);
			if (num_input < 1 || num_input > fcountl(fptr, path)) {
				biberr("Citation does not exist");
			} else if (strcmp(str_input, "esc") != 0) // Breaks if 'esc' is input
				editing = true; // Allows for 'cite' command to commence right afterwards
			oper_success = true;
		}

		/*** 'cite' command ***/
		if (strcmp(str_input, "cite") == 0 || editing) {
			fptr = fopen(".temp", "a");
			bibcite(fptr, &editing, &esc);
			if(!esc) { // Breaks if 'esc' is input
				fptr = fopen(".temp", "r");
				if (editing) { // If came from 'modify' command...
					fgets(str_input, BUF, fptr);
					fclose(fptr);
					fptr = fopen(path, "r");
					fmodl(fptr, path, num_input, str_input);
					editing = false; // Ensures that 'cite' command is not guaranteed to run afterwards
				} else { // Else...
					fgets(str_input, BUF, fptr);
					fclose(fptr);
					fptr = fopen(path, "a");
					fputs(str_input, fptr);
				}
				fclose(fptr);
				if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(fptr, path);
			}
			oper_success = true;
		}

		/*** 'clear' command ***/
		if (strcmp(str_input, "clear") == 0) {
			fptr = fopen(path, "w");
			fclose(fptr);
			if (auto_refresh) // Refreshes automatically if auto-refresh is on
					bibtitle(fptr, path);
			oper_success = true;
		}

		/*** 'config' command ***/
		if (strcmp(str_input, "config") == 0) {
			printf("   ├ Auto-refresh: "); // Configuration for auto-refresh
			fgets(str_input, BUF, stdin);
			if(strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
				if (strcmp(str_input, "on\n") == 0)
					auto_refresh = true;
				else if (strcmp(str_input, "off\n") == 0)
					auto_refresh = false;
				fptr = fopen(".conf", "r");
				fmodl(fptr, ".conf", 2, str_input);
				printf("   └ Command symbol: "); // Configuration for command line symbol
				fgets(str_input, BUF, stdin);
				if(strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
					fptr = fopen(".conf", "r");
					fmodl(fptr, ".conf", 5, str_input);
					cmd_symbol = str_input[0];
				}
			}
			fclose(fptr);
			oper_success = true;
		}

		/*** 'delete' command ***/
		if (strcmp(str_input, "delete") == 0) {
			printf("   └ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			if (atoi(str_input) < 1 || atoi(str_input) > fcountl(fptr, path)) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			}
			else if (strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
				fptr = fopen(path, "r");
				fdell(fptr, path, atoi(str_input));
				fclose(fptr);
			}
			if (auto_refresh) // Refreshes automatically if auto-refresh is on
				bibtitle(fptr, path);
			oper_success = true;
		}

		/*** 'echo' command ***/
		if (strcmp(str_input, "echo") == 0) {
			printf("   └ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			if (atoi(str_input) < 1 || atoi(str_input) > fcountl(fptr, path)) { // Ensures valid input
				printf("   ");
				biberr("Citation does not exist");
			} else
				bibecho(fptr, path, atoi(str_input));
			oper_success = true;
		}

		/*** 'exit' command ***/
		if (strcmp(str_input, "exit") == 0) {
			system("clear"); // Leaves no trace of program
			oper_success = true;
			break; // Breaks out of program loop
		}

		/*** 'help' command ***/
		if (strcmp(str_input, "help") == 0) {
			bibhelp();
			bibtitle(fptr, path);
			oper_success = true;
		}

		/*** 'move' command ***/
		if (strcmp(str_input, "move") == 0) {
			printf("   ├ Citation #: ");
			fgetsl(str_input, BUF, stdin);
			if (atoi(str_input) < 1 || atoi(str_input) > fcountl(fptr, path)) // Ensures valid input
				biberr("Citation does not exist");
			else if (strcmp(str_input, "esc") != 0) { // Breaks if 'esc' is input
				fptr = fopen(path, "r");
				num_input = atoi(str_input);
				printf("   └ Move to: ");
				fgetsl(str_input, BUF, stdin);
				if(strcmp(str_input, "esc") != 0) // Breaks if 'esc' is input
					fmovl(fptr, path, num_input, atoi(str_input));
			}
			if (auto_refresh) // Refreshes automatically if auto-refresh is on
				bibtitle(fptr, path);
			oper_success = true;
		}

		/*** 'refresh' command ***/
		if (strcmp(str_input, "refresh") == 0) {
			bibtitle(fptr, path); // Refreshes the screen
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
