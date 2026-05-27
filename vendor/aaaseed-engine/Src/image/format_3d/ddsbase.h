// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef DDSBASE_H
#define DDSBASE_H

//#include "codebase.h" // universal code base
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#define BOOLINT INT32

void writeDDSfile(const char *filename,unsigned char *data,unsigned int bytes,unsigned int skip=0,unsigned int strip=0,BOOLINT nofree=0);
unsigned char *readDDSfile(const char *filename,unsigned int *bytes);

void writeRAWfile(const char *filename,unsigned char *data,unsigned int bytes,BOOLINT nofree=0);
unsigned char *readRAWfile(const char *filename,unsigned int *bytes);

void writePNMimage(const char *filename,unsigned char *image,unsigned int width,unsigned int height,unsigned int components,BOOLINT dds=0);
unsigned char *readPNMimage(const char *filename,unsigned int *width,unsigned int *height,unsigned int *components);

void writePVMvolume(const char *filename,unsigned char *volume,
                    unsigned int width,unsigned int height,unsigned int depth,unsigned int components=1,
                    float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                    unsigned char *description=NULL,
                    unsigned char *courtesy=NULL,
                    unsigned char *parameter=NULL,
                    unsigned char *comment=NULL);

unsigned char *readPVMvolume(const char *filename,
                             unsigned int *width,unsigned int *height,unsigned int *depth,unsigned int *components=NULL,
                             float *scalex=NULL,float *scaley=NULL,float *scalez=NULL,
                             unsigned char **description=NULL,
                             unsigned char **courtesy=NULL,
                             unsigned char **parameter=NULL,
                             unsigned char **comment=NULL);

int checkfile(const char *filename);
unsigned int checksum(unsigned char *data,unsigned int bytes);

void swapbytes(unsigned char *data,long long bytes);
void convbytes(unsigned char *data,long long bytes);
void convfloat(unsigned char **data,long long bytes);
void convrgb(unsigned char **data,long long bytes);

unsigned char *quantize(unsigned char *volume,
                        long long width,long long height,long long depth,
                        BOOLINT msb=1,
                        BOOLINT linear=0,BOOLINT nofree=0);

char *processPVMvolume(const char *filename);

#endif
