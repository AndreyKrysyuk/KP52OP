#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct post_s* post_t;
typedef struct network_s* network_t;


network_t network_create();
network_t network_getPostsWithRepostsMoreThanValue(network_t self, int value);

void network_remove();
void network_addNewPostToEnd(network_t self, int amountReposts, const char* text);
void network_addNewPostByIndex(network_t self, int amountReposts, const char* text, int index);
void network_setNewAmountRepostsForPost(network_t self, int index, int new_amountReposts);
void network_setNewTextForPost(network_t self, int index, const char * new_text);
void network_printPost(network_t self, int index);
void network_printNetwork(network_t self);
void network_removePostByIndex(network_t self, int index);

int network_getAmountPosts(network_t self);
int network_getAmountRepostsOfPost(network_t self, int index);
int network_getPostWithMaxAmountReposts(network_t self);


#endif // MODULE_H_INCLUDED
