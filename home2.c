#include <dirent.h> 
#include <stdio.h> 
#include <inttypes.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#define INVALID 0xFFFFFFFF
#define MIN 0x01
#define MAX 0x07
#define MIN_SIZE 0x02
#define FILE_SIZE 0x01
#define GIVEN_PATH 0x08
#define  PATTERN 0x04

void printingItems(char* path, uint32_t args, char** argv, uint8_t depth);
uint32_t parsingArguments(int argc, char** argv) {
  uint32_t args = 0;
  if (argc < MIN && argc > MAX){
    return INVALID;
  }
  uint8_t i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-S") == 0) {
      args |= FILE_SIZE;
    }
    else if (strcmp(argv[i], "-s") == 0) {
      args |= MIN_SIZE;
      i++;
      args |= (i << 24);
      if (i == argc) {
        return INVALID;
      }
    }
    else if (strcmp(argv[i], "-f") == 0) {
      args |= PATTERN;
      i++;
      args |= (i << 16);
      if (i == argc) {
          return INVALID;
      }
    }
        
    else {
      if ((args & GIVEN_PATH) == GIVEN_PATH) {
        return INVALID;
      }
      args |= GIVEN_PATH;
      args |= (i << 8);
      }
  }
  return args;
}

void displayDirectories(char* path, uint32_t args, char** argv, uint8_t depth) {
  DIR* d = opendir(path);
  struct dirent* dir;
  struct stat sBuff;
  char* fPath = (char *) "";

  if (depth == 1) {
      if (strcmp(path, ".") == 0) {
          char words[50];
          getcwd(words, 50);
          uint8_t i;
          for (i = strlen(words) - 1; i > 0; i--) {
              if (words[i] == '/' || words[i] == '\\') {
                  printf("%s\n", words + i + 1);
                  break;
              }
          }
      } else {
          uint8_t i;
          for (i = strlen(path) - 1; i > 0; i--) {
              if (path[i] == '/' || path[i] == '\\') {
                  printf("%s\n", path + i + 1);
                  break;
              }
          }
      }
  }

  dir = readdir(d);
  
  dir = readdir(d);

  while((dir = readdir(d)) != NULL) {
      uint16_t fPathLength = strlen(path) + 2 + strlen(dir->d_name);
      fPath = (char*) malloc(fPathLength);
      memset(fPath, 0, fPathLength);
      strcat(fPath, path);
      strcat(fPath, "/");
      strcat(fPath, dir->d_name);
      if (stat(fPath, &sBuff) != 0){
          printf("Error!");
      }
      if (dir->d_type==DT_DIR){
          uint8_t i;
          for (i = 0; i <depth; i++){
            printf("\t");
          }
          printf("%s\n", dir->d_name);
          printingItems(fPath, args, argv, depth + 1);
      }
      memset(fPath, 0, fPathLength);
      free(fPath);
    }
    closedir(d);
}

void format(uint8_t hasSize, char *fileName, int fileSize) {
    if (hasSize){
        printf("%s (%u bytes)\n", fileName, fileSize);
    } else{
        printf("%s\n", fileName);
    }
}

void printFiles(char* path, uint32_t args, char** argv, uint8_t depth){   
}

void printingItems(char* path, uint32_t args, char** argv, uint8_t depth) {
    displayDirectories(path, args, argv, depth);
    DIR* d = opendir(path);
    struct dirent* dir;
    struct stat sBuff;
    char* fPath = (char *) "";
    uint8_t i;

    while ((dir = readdir(d)) != NULL) {
      uint16_t fPathLength = strlen(path) + 2 + strlen(dir->d_name);
      char *fileName = dir->d_name;

      fPath = (char*) malloc(fPathLength);
      memset(fPath, 0, fPathLength);

      strcat(fPath, path);
      strcat(fPath, "/");
      strcat(fPath, dir->d_name);
      stat(fPath, &sBuff);
      if(dir->d_type==DT_LNK){
        char realPath[261] = {0};
        realpath(fPath, realPath);
        for (i=strlen(realPath); i > 0; i--) {
            if (realPath[i] == '/' || realPath[i] == '\\') {
              fileName = realPath + i + 1;
              break;
            }
          }
      }

      if(dir->d_type==DT_REG || dir->d_type==DT_LNK){
        if(args & MIN_SIZE){
          if (sBuff.st_size>= atoi(argv[(uint8_t)(args >> 24)])) {
            if ((args)){
              if (strstr(fileName, argv[(uint8_t) (args >> 16)])) {
                  for (i =0; i <depth; i++) {
                      printf("\t");
                }
                            format(args & FILE_SIZE, fileName, sBuff.st_size);
                            }
                    } 
                    else{
                       for(i =0; i <depth; i++){
                            printf("\t");
                        }
                        format(args & FILE_SIZE, fileName, sBuff.st_size);
                    }
                }
            } 
            
            else {
                if ((args)) {
                    if (strstr(fileName, argv[(uint8_t) (args >> 16)])) {
                        for (i =0; i <depth; i++) {
                            printf("\t");
                            }
                        format(args & FILE_SIZE, fileName, sBuff.st_size);
                    }
                } else {
                    for (i =0; i< depth; i++) {
                        printf("\t");
                    }
                    format(args & FILE_SIZE, fileName, sBuff.st_size);
                  }
            }
    }
    memset(fPath, 0, fPathLength);
    free(fPath);
  }
  closedir(d);
}

void dictionaryteach(uint32_t args, char** argv){
  char* path = (char *)".";
  if(args & GIVEN_PATH) {
    path = argv[(uint8_t)(args>> 8)];
  }
  printingItems(path,args,argv,1);
}

int main(int argc, char** argv) {
  uint32_t args = parsingArguments(argc, argv);
  if(args == INVALID) {
    printf("invalid\n");
    return 0;
  }
  dictionaryteach(args, argv);
  return(0);
}