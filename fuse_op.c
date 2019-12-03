/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  heavily modified by Jann Horn <jannh@google.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall fuse_op.c `pkg-config fuse --cflags --libs` -o fuse_op
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/uio.h>

static const char *fuse_op_path = "/fuse_op";

static char data_state[sizeof(struct iovec)];

static int fuse_op_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, fuse_op_path) == 0) {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = sizeof(data_state);
		stbuf->st_blocks = 0;
	} else
		res = -ENOENT;
	return res;
}

static int fuse_op_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, fuse_op_path + 1, NULL, 0);
	return 0;
}

static int fuse_op_open(const char *path, struct fuse_file_info *fi) {
	return 0;
}

static int fuse_op_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	sleep(10);
	size_t len = sizeof(data_state);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, data_state + offset, size);
	} else
		size = 0;
	return size;
}

static int fuse_op_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	if (offset != 0)
		errx(1, "got write with nonzero offset");
	if (size != sizeof(data_state))
		errx(1, "got write with size %d", (int)size);
	memcpy(data_state + offset, buf, size);
	return size;
}

static struct fuse_operations fuse_op_oper = {
	.getattr	= fuse_op_getattr,
	.readdir	= fuse_op_readdir,
	.open		= fuse_op_open,
	.read		= fuse_op_read,
	.write		= fuse_op_write,
};

int main(int argc, char *argv[]) {
	return fuse_main(argc, argv, &fuse_op_oper, NULL);
}
