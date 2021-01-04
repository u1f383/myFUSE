#ifndef _FILENOTE_H
#define _FILENOTE_H

typedef struct _FileNode {
    char *filename;
    char *content;
    int isdir;
    struct _FileNode* next;
} FileNode;

FileNode *new_node(const char *filename, int isdir);
FileNode *get_node_by_path(FileNode *hdr, const char *path);
void add_node(FileNode **hdr, FileNode *node);
void delete_node(FileNode **hdr, FileNode *node);
int get_isdir_by_path(FileNode *hdr, const char *path);

#endif
