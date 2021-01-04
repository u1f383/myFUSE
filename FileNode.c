#include "FileNode.h"
#include <string.h>
#include <stdlib.h>

FileNode *new_node(const char *filename, int isdir)
{
    FileNode *n = malloc(sizeof(FileNode));
    n->filename = malloc(strlen(filename) + 1);
    n->content = malloc(0x10);
    n->next = NULL;
    n->isdir = isdir;

    strcpy(n->filename, filename);
    n->content[0] = '\x00';

    return n;
}

FileNode *get_node_by_path(FileNode *hdr, const char *path)
{
    FileNode *n = hdr; 
    path++;

    while (n) {
        if (strcmp(n->filename, path) == 0) {
            return n;
        }
        n = n->next;
    }

    return NULL;
}

// change the variable point to target
void delete_node(FileNode **hdr, FileNode *target)
{
    FileNode** indirect = hdr;
    
    while (*indirect != target)
        indirect = &(*indirect)->next;

    *indirect = target->next;
    
    if (target->filename) {
        free(target->filename);
    }
    if (target->content) {
        free(target->content);
    }
    free(target);
}

void add_node(FileNode **hdr, FileNode *target)
{
    FileNode** indirect = hdr;

    if(*indirect == NULL) {
        *indirect = target;
    } else {
        while ((*indirect)->next)
            indirect = &(*indirect)->next;

        (*indirect)->next = target;
    }
}

int get_isdir_by_path(FileNode *hdr, const char *path)
{
    FileNode *n = get_node_by_path(hdr, path);

    if (n) {
        return n->isdir;
    }

    return -1;
}
