#ifndef FS_SLIM_H
#define FS_SLIM_H

// Init function called by fs/filesystems.c on startup
fs_t * slim_init(gbd_t *disk);

// Structure for slim_t, the fs_t internal pointer for slim-specific
// information.
// TODO
struct {
	uint16_t BPB_BytsPerSec; // Set to 512
	uint8_t BPB_SecPerClus; // 1,2,4,8,16,32,64 or 128
	uint16_t BPB_RsvdSecCnt; // Usually 0x20
	uint8_t BPB_NumFATs; // Always 2
	uint32_t BPB_FATSz32; // Depends on disk size
	uint32_t BPB_RootClus; // Usualy 0x00000002
	uint16_t BPB_Signature; // Always 0xAA55
	uint32_t BPB_VolumeID; // Not covered
} BPB_t;

// Functions that fs_t requires be present for a full implementation
// of a filesystem
int slim_unmount(fs_t *fs);
int slim_open(fs_t *fs, char *filename);
int slim_close(fs_t *fs, int fileid);
int slim_create(fs_t *fs, char *filename, int size);
int slim_remove(fs_t *fs, char *filename);
int slim_read(fs_t *fs, int fileid, void *buffer, int bufsize, int offset);
int slim_write(fs_t *fs, int fileid, void *buffer, int datasize, int offset);
int slim_getfree(fs_t *fs);

// Utility stuff
uint16_t read_16(uint8_t *);
uint32_t read_32(uint8_t *);
#endif
