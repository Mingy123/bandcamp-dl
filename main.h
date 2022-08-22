#pragma once

struct response {
    char* item;
    size_t len;
};

struct response get_site (char* site);

void parse_playlist (char* site);
void parse_mainpage (char* site);
