#include <stdio.h>

/* Resets text formatting in terminal */
void txtr(void) {
	printf("\e[0m");
}

/* Italicizes text in terminal */
void txti(void) {
	printf("\e[3m");
}

/* Boldens text in terminal */
void txtb(void) {
	printf("\e[1m");
}