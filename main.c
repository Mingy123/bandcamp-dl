#include <stdio.h>
#include <string.h>
#include "main.h"

// -a --album: download it as an album
// -p --playlist: download as a playlist

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s\n-a --album 'site' : download every playlist from the main thing\n-p --playlist 'site' : download every file from playlist\n", argv[0]);
    }

    else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--album") == 0) {
        parse_mainpage(argv[2]);
    } else if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--playlist") == 0) {
        parse_playlist(argv[2]);
    }

    else {
        printf("Usage: %s\n-a --album 'site' : download every playlist from the main thing\n-p --playlist 'site' : download every file from playlist\n", argv[0]);
    }
}
