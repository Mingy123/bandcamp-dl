// normal io just doesnt work for binary
// use fwrite(item, len, 1, stream)

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

size_t wfunc (char* new_item, size_t size, size_t n, struct response *prev) {
    size_t got_len = size*n;
    size_t new_len = prev->len + got_len;
    
    // change the size of char* item, then append it to the end
    prev->item = realloc (prev->item, new_len+1);
    memcpy (prev->item+prev->len, new_item, got_len);
    prev->item[new_len] = '\0';
    prev->len = new_len;

    return got_len;
}

struct response get_site (char* site) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Error creating handle\n");
        exit(EXIT_FAILURE);
    }

    struct response item;
    // init the item
    item.len = 0;
    item.item = malloc(1);
    item.item[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, site);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wfunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &item);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        printf("Error getting %s: %s\n", site, curl_easy_strerror(result));
        exit(EXIT_FAILURE);
    }

    curl_easy_cleanup(curl);
    return item;
}
