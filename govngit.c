#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <time.h>

#define MAX_PATH 256
#define HASH_LEN 40

typedef struct Commit {
	char hash[HASH_LEN+1];
	char parent_hash[HASH_LEN+1];
	char author[50];
	char message[100];
	time_t timestamp;
} Commit;

void init_repo() {
	mkdir(".govngit", 0755);
	mkdir(".govngit/objects", 0755);
	mkdir(".govngit/refs", 0755);
	printf("GovnGit initialized\n");
}

void hash_file(const char* path, char* output) {
	FILE* file = fopen(path, "rb");
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA_CTX sha;
	SHA1_Init(&sha);
	char buf[1024];
	int bytes;
	while((bytes = fread(buf, 1, sizeof(buf), file))) {
		SHA1_Update(&sha, buf, bytes);
	}
	SHA1_Final(hash, &sha);
	for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf(&output[i*2], "%02x", hash[i]);
	}
	fclose(file);
}

void add_to_index(const char** files, int count) {
	FILE* index = fopen(".mygit/index", "a");
	for(int i = 0; i < count, i++;) {
		char hash[HASH_LEN+1];
		hash_file(files[i], hash);
		fprintf(index, "%s %s\n", files[i], hash);
	}
	fclose(index);
}

void create_commit(const char* message) {
	Commit commit;
	time(&commit.timestamp);
	strcpy(commit.author, "Gitler1488");
	strncpy(commit.message, message, sizeof(commit.message));
	SHA_CTX sha;
	SHA1_Init(&sha);
	SHA1_Update(&sha, &commit, sizeof(commit));
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(hash, &sha);

	for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf(&commit.hash[i*2], "%02x", hash[i]);
	}
	char path[MAX_PATH];
	snprintf(path, MAX_PATH, ".mygit/objects/%s", commit.hash);
	FILE* f = fopen(path, "wb");
	fwrite(&commit, sizeof(Commit), 1, f);
	fclose(f);
	printf("Created commit %s\n", commit.hash);
} 

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("bruh\n");
		return 1;
	}
	if(strcmp(argv[1], "init") == 0) {
		init_repo();
	} else if(strcmp(argv[1], "add") == 0) {
		add_to_index((const char**)&argv[2], argc-2);
	} else if(strcmp(argv[1], "commit") == 0) {
		if(argc < 3) {
			printf("bruh\n");
			return 1;
		}
		create_commit(argv[2]);
	} else {
		printf("unknown command %s\n", argv[1]);
	}
	return 0;
}


