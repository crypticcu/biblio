# Biblio 📝

## Description
Command-line bibliography creator and editor.

Biblio is a bibliography creator and editor that easily runs in the terminal. The utility of this program over others is that it saves each bibliography as its own files, allowing you to name and keep track of each list of sources accordingly. All style guides have been checked for accuracy. The program currently supports such citations styles as:
 * MLA
 * APA
 * Chicago
 
each available to use in three different formats:
 * Website
 * Book
 * Article

See a citation style not listed? The 'label' command allows you to mark each bibliography with the appropriate styling you need. Manual input is also supported. Terminals running this program require the ability to identify ANSI/VT100 escape codes, as they are used to format text throughout. If you would like request a change in the official release or have found a bug, feel free to contact me. Happy studying!

## Change Log
*v1.0* - Release

*v1.1* - Strings are now dynamically allocated. Citations can now specify if date is access date. Updated help page.

*v1.2* - Strings no longer cause heap overflow. Improved UI. Added the 'echo' command. Updated help page.

*v1.3* - Added licensing information and updated references list. Reworked structure of the program to improve code clarity and user experience. Removed support for dynamic memory allocation.

*v1.4* - Program is now argument-driven. Made it so the publisher's name would be italicized to comform to style guidelines. Removed 'open' and 'close' commands. Updated help page.

*v1.5* - Overhauled UI. Improved bibliography output. Renamed commands and added shortcuts to each. Bibliographies are now automatically sorted alphabetically. Removed 'move' command. Added styling label functionality. Improved error handling. Updated help page.

*v1.6* - Program is now backed by ioplus.h, allowing for precise, dynamic memory allocation. Increased code efficiency and readability. Fixed several major bugs.
