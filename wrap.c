#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include "main.h"

char* LEGALFILENAME = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM `~?!@#$%^&()-_=+[]{};',.\"";
// return true if valid
int fileChar (char chr) {
    char* good = LEGALFILENAME;
    while (*good != '\0') {
        if (*good == chr) return 1;
        good++;
    } return 0;
}
int intLen (int n) {
    if (n == 0) return 1;
    int ans = 0;
    while (n != 0) {
        ans++;
        n /= 10;
    } return ans;
}

void write_to_file (char* filename, char* site) {
    FILE *outfile = fopen(filename, "wb");
    struct response output = get_site(site);
    fwrite(output.item, output.len, 1, outfile);
    fclose(outfile);
    free(output.item);
}

char* fixName (char* og, int zeros, char* ext) {
    char* ans = malloc(strlen(og) + zeros + strlen(ext) + 1);
    char *read = og, *write = ans;
    for (int i = 0; i < zeros; i++) {
        *(write++) = '0';
    }

    while (*read != '\0') {
        if (*read == '&') {
            read += 5;
            if (*read == '#') *write = atoi(read+1);
            else *write = '&';
            write++;
            read = strstr(read, ";")+1;
        } else if (fileChar(*read)) *(write++) = *(read++);
        else read++;
    }

    while (*ext != '\0') *(write++) = *(ext++);
    *write = '\0';
    ans = realloc(ans, strlen(ans) + 1);
    return ans;
}


void parse_playlist (char* site) {
    struct response output = get_site(site);

    char* title1 = strstr(output.item, "<title>") + 7;
    char* title2 = title1;
    while (*title2 != '<') title2++;
    char *title = malloc(title2-title1); memcpy(title, title1, title2-title1);
    title[title2-title1-1] = '\0';

    char *names = strstr(output.item, "name=\"description\"");
    if ((names = strstr(names, "\n1.")) == NULL) names = title;
    else names++;
    char *url = output.item, *brEnd, *nextLine = names;

    if (mkdir(title, 0755) == -1) {
        printf("Error creating directory: %s. If it previously existed, remove it.\n", title);
        exit(EXIT_FAILURE);
    } if (chdir(title) == -1) {
        printf("Error entering directory: %s\n", title);
        exit(EXIT_FAILURE);
    }

    int fileN = 1, fileC = 0;
    char* getC = names;
    while (atoi(getC) > fileC) {
        fileC = atoi(getC);
        while (*getC != '\n') getC++;
        getC++;
    }

    while ((url = strstr(url, "t4.bcbits.com/stream/")) != NULL ) {
        brEnd = url;
        assert(brEnd == url && names == nextLine);
        while (*brEnd != '}') brEnd++;
        *brEnd = '\0';

        while (*nextLine != '\n' && *nextLine != '\0') nextLine++;
        *nextLine = '\0';
        char* newname = fixName(names, intLen(fileC)-intLen(fileN), ".mp3");
        printf("%s\n", newname);
        write_to_file (newname, url);
        free(newname);

        url = ++brEnd;
        names = ++nextLine;
        fileN++;
    } free(title);

    chdir("..");

    free(output.item);
}



void parse_mainpage (char* site) {
    struct response output = get_site(site);

    char* title1 = strstr(output.item, "<title>") + 7;
    char* title2 = title1;
    while (*title2 != '|' && *title2 != '<') title2++;
    char *title = malloc(title2-title1); memcpy(title, title1, title2-title1);
    title[title2-title1-1] = '\0';

    if (mkdir(title, 0755) == -1) {
        printf("Error creating directory: %s. If it previously existed, remove it.\n", title);
        exit(EXIT_FAILURE);
    } if (chdir(title) == -1) {
        printf("Error entering directory: %s\n", title);
        exit(EXIT_FAILURE);
    }

    char *start = strstr(output.item, "<ol id=\"music-grid\""), *end = strstr(start, "</ol>");
    char *url = start, *urlEnd, *link;
    while ((url = strstr(url, "<a href=\"") + 9) < end) {
        urlEnd = strstr(url, "\"");
        *urlEnd = '\0';

        link = malloc(strlen(site) + strlen(url) + 1);
        strcpy(link, site);
        strcat(link, url);

        parse_playlist(link);
        url = urlEnd + 1;
        free(link);
    }
}
