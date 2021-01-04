/**
 * Reference by https://github.com/MaaSTaaR/LSYSFS
 */
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "FileNode.h"

static int do_getattr(const char *path, struct stat *st, struct fuse_file_info *fi);
static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags);
static int do_mkdir(const char *path, mode_t mode);
static int do_rmfile(const char *path);
static int do_mknod(const char *path, mode_t mode, dev_t rdev);
static int do_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);

FileNode *file_node_hdr = NULL;

/* call everytime when FUSE asks for attributes of specified file */
static int do_getattr(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    st->st_uid = getuid(); // owner
    st->st_gid = getgid(); // group
    st->st_atime = time(NULL); // last access time
    st->st_mtime = time(NULL); // last modify time
	
	if (strcmp(path, "/") == 0 || get_isdir_by_path(file_node_hdr, path) == 1)
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    }
    else if (get_isdir_by_path(file_node_hdr, path) == 0)
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
    }
    else
    {
		// fusemount will auto access some files like autorun.inf, .Trash-1000...
        return -ENOENT;
    }
    return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    /* add entries */
    filler(buffer, ".", NULL, 0, 0);
    filler(buffer, "..", NULL, 0, 0);
  
    if (strcmp(path, "/") == 0) // show all entries
    {    
        FileNode *n = file_node_hdr;

        while (n) {
            filler(buffer, n->filename, NULL, 0, 0);
            n = n->next;
        }
    }

    return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    
    FileNode *n = get_node_by_path(file_node_hdr, path);
    
    if (n) {
        memcpy(buffer, n->content + offset, size);
		size_t len = strlen(n->content);
        
        if (offset >= len) {
            
            return 0;
        }
        
        if (offset+size > len) {
             memcpy(buffer, n->content+offset, len-offset);

             return len-offset;
        }
        
        memcpy(buffer, n->content+offset, size);

        return strlen(n->content) - offset;
    }
    
    return -1; // not found
}

static int do_mkdir(const char *path, mode_t mode)
{
	path++;
	add_node(&file_node_hdr, new_node(path, 1));
	
	return 0;
}

static int do_mknod(const char *path, mode_t mode, dev_t rdev)
{
	path++;
	add_node(&file_node_hdr, new_node(path, 0));
	
	return 0;
}

static int do_rmfile(const char *path)
{
    FileNode *n = get_node_by_path(file_node_hdr, path);

    if (n)
        delete_node(&file_node_hdr, n);

    return 0;
}

static int do_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    FileNode *n = get_node_by_path(file_node_hdr, path);

    if (n) {
        size_t len = strlen(buffer);

        n->content = realloc(n->content, len+1);
        strcpy(n->content, buffer);

       return len;
    }

    return -1;
}

// those functions will be triggered when user does the corresponding operation
static struct fuse_operations oper = {
    // struct initializer: .var = value
    .getattr = do_getattr,
    .readdir = do_readdir, // cd
    .read = do_read, // cat
    .mkdir = do_mkdir, // mkdir
    .mknod = do_mknod, // touch
	.rmdir = do_rmfile, // rmdir
    .unlink = do_rmfile, // rm
    .write = do_write, // echo "XX" >> file
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &oper, NULL);
}
