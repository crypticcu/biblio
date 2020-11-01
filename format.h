#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifndef BUF
#define BUF 256 // Default buffer size
#endif

#define E_BLD "\e[1m" /*   Text formatting	  */
#define E_ITL "\e[3m" /*     escape codes	  */
#define E_UND "\e[4m" /*					  */
#define E_STR "\e[9m" /* Compatible with most */
#define E_CLR "\e[0m" /* ANSI/VT100 terminals */

/* Converts 'str' to all uppercase */
char *supper(char *str) {
	static char ret[BUF];

	for (int i = 0; i < strlen(str); i++) {
		if (islower(str[i]))
			ret[i] = toupper(str[i]);
		else
			ret[i] = str[i];
	}	
	return ret;
}

/* Converts 'str' to all lowercase */
char *slower(char *str) {
	static char ret[BUF];

	for (int i = 0; i < strlen(str); i++) {
		if (isupper(str[i]))
			ret[i] = tolower(str[i]);
		else
			ret[i] = str[i];
	}	
	return ret;
}
