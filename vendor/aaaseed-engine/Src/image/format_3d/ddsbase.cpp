// (c) by Stefan Roettger, licensed under GPL 2+

#include "ddsbase.h"
#include "err.h"
#include "aaa_util.h"
#include "system/Native_System.h"
#include "aaa_mem.h"

enum VR_ERROR
{
   VR_ERROR_NONFATAL	=0,
   VR_ERROR_FATAL		=1,
   VR_ERROR_MEM			=2,
   VR_ERROR_IO			=3,
   VR_ERROR_CODE		=4
};

#define TRUE 1
#define FALSE 0

#define ERRORMSG()	ERR_PRINT_STRING( "in %s at line %d, error %d", __FILE__, __LINE__, VR_ERROR_FATAL	)
#define MEMERROR()	ERR_PRINT_STRING( "in %s at line %d, error %d", __FILE__, __LINE__, VR_ERROR_MEM	)
#define IOERROR()	ERR_PRINT_STRING( "in %s at line %d, error %d", __FILE__, __LINE__, VR_ERROR_IO		)
#define CODEERROR() std::cerr << __FILE__ << __LINE__ << VR_ERROR_CODE << std::endl
#define WARNMSG(msg) std::cerr << __FILE__ << __LINE__ << VR_ERROR_NONFATAL << msg << std::endl

//#define MALLOC(size)		malloc(size)
//#define REALLOC(pt,size)	realloc(pt,size)
//#define FREE(pt)			free(pt)

#ifdef HAVE_MINI
#	include <mini/rawbase.h>
#endif

#define DDS_MAXSTR (256)

#define DDS_BLOCKSIZE (1<<20)
#define DDS_INTERLEAVE (1<<24)

#define DDS_RL (7)

#define DDS_ISINTEL (*((unsigned char *)(&DDS_INTEL)+1)==0)

char DDS_ID[]="DDS v3d\n";
char DDS_ID2[]="DDS v3e\n";

unsigned char *DDS_cache;
unsigned int DDS_cachepos,DDS_cachesize;

unsigned int DDS_buffer;
unsigned int DDS_bufsize;

unsigned short int DDS_INTEL=1;

// helper functions for DDS:

inline unsigned int DDS_shiftl(const unsigned int value,const unsigned int bits)
   {return((bits>=32)?0:value<<bits);}

inline unsigned int DDS_shiftr(const unsigned int value,const unsigned int bits)
   {return((bits>=32)?0:value>>bits);}

inline void DDS_swapuint(unsigned int *x)
   {
   unsigned int tmp=*x;

   *x=((tmp&0xff)<<24)|
	  ((tmp&0xff00)<<8)|
	  ((tmp&0xff0000)>>8)|
	  ((tmp&0xff000000)>>24);
   }

void DDS_initbuffer()
   {
   DDS_buffer=0;
   DDS_bufsize=0;
   }

inline void DDS_clearbits()
   {
   DDS_cache=NULL;
   DDS_cachepos=0;
   DDS_cachesize=0;
   }

inline void DDS_writebits(unsigned int value,unsigned int bits)
   {
   value&=DDS_shiftl(1,bits)-1;

   if (DDS_bufsize+bits<32)
	  {
	  DDS_buffer=DDS_shiftl(DDS_buffer,bits)|value;
	  DDS_bufsize+=bits;
	  }
   else
	  {
	  DDS_buffer=DDS_shiftl(DDS_buffer,32-DDS_bufsize);
	  DDS_bufsize-=32-bits;
	  DDS_buffer|=DDS_shiftr(value,DDS_bufsize);

	  if (DDS_cachepos+4>DDS_cachesize)
		 if( DDS_cache==NULL )
         {
			if( (DDS_cache=(unsigned char *)MALLOC(DDS_BLOCKSIZE)) ==NULL)
				MEMERROR();
			DDS_cachesize=DDS_BLOCKSIZE;
		 }
		 else
		 {
			if ((DDS_cache=(unsigned char *)REALLOC(DDS_cache,DDS_cachesize+DDS_BLOCKSIZE))==NULL)
				MEMERROR();
			DDS_cachesize+=DDS_BLOCKSIZE;
		 }

	  if (DDS_ISINTEL) DDS_swapuint(&DDS_buffer);
	  *((unsigned int *)&DDS_cache[DDS_cachepos])=DDS_buffer;
	  DDS_cachepos+=4;

	  DDS_buffer=value&(DDS_shiftl(1,DDS_bufsize)-1);
	  }
   }

inline void DDS_flushbits()
   {
   unsigned int bufsize;

   bufsize=DDS_bufsize;

   if (bufsize>0)
	  {
	  DDS_writebits(0,32-bufsize);
	  DDS_cachepos-=(32-bufsize)/8;
	  }
   }

inline void DDS_savebits(unsigned char **data,unsigned int *size)
   {
   *data=DDS_cache;
   *size=DDS_cachepos;
   }

inline void DDS_loadbits(unsigned char *data,unsigned int size)
   {
   DDS_cache=data;
   DDS_cachesize=size;

   if ((DDS_cache=(unsigned char *)REALLOC(DDS_cache,DDS_cachesize+4))==NULL) MEMERROR();
   *((unsigned int *)&DDS_cache[DDS_cachesize])=0;

   DDS_cachesize=4*((DDS_cachesize+3)/4);
   if ((DDS_cache=(unsigned char *)REALLOC(DDS_cache,DDS_cachesize))==NULL) MEMERROR();
   }

inline unsigned int DDS_readbits(unsigned int bits)
   {
   unsigned int value;

   if (bits<DDS_bufsize)
	  {
	  DDS_bufsize-=bits;
	  value=DDS_shiftr(DDS_buffer,DDS_bufsize);
	  }
   else
	  {
	  value=DDS_shiftl(DDS_buffer,bits-DDS_bufsize);

	  if (DDS_cachepos>=DDS_cachesize) DDS_buffer=0;
	  else
		 {
		 DDS_buffer=*((unsigned int *)&DDS_cache[DDS_cachepos]);
		 if (DDS_ISINTEL) DDS_swapuint(&DDS_buffer);
		 DDS_cachepos+=4;
		 }

	  DDS_bufsize+=32-bits;
	  value|=DDS_shiftr(DDS_buffer,DDS_bufsize);
	  }

   DDS_buffer&=DDS_shiftl(1,DDS_bufsize)-1;

   return(value);
   }

inline int DDS_code(int bits)
   {return(bits>1?bits-1:bits);}

inline int DDS_decode(int bits)
   {return(bits>=1?bits+1:bits);}

// deinterleave a byte stream
void DDS_deinterleave(unsigned char *data,unsigned int bytes,unsigned int skip,unsigned int block=0,BOOLINT restore=FALSE)
   {
   unsigned int i,j,k;

   unsigned char *data2,*ptr;

   if (skip<=1) return;

   if (block==0)
	  {
	  if ((data2=(unsigned char *)MALLOC(bytes))==NULL) MEMERROR();

	  if (!restore)
		 for (ptr=data2,i=0; i<skip; i++)
			for (j=i; j<bytes; j+=skip) *ptr++=data[j];
	  else
		 for (ptr=data,i=0; i<skip; i++)
			for (j=i; j<bytes; j+=skip) data2[j]=*ptr++;

	  memcpy(data,data2,bytes);
	  }
   else
	  {
	  if ((data2=(unsigned char *)MALLOC((bytes<skip*block)?bytes:skip*block))==NULL) MEMERROR();

	  if (!restore)
		 {
		 for (k=0; k<bytes/skip/block; k++)
			{
			for (ptr=data2,i=0; i<skip; i++)
			   for (j=i; j<skip*block; j+=skip) *ptr++=data[k*skip*block+j];

			memcpy(data+k*skip*block,data2,skip*block);
			}

		 for (ptr=data2,i=0; i<skip; i++)
			for (j=i; j<bytes-k*skip*block; j+=skip) *ptr++=data[k*skip*block+j];

		 memcpy(data+k*skip*block,data2,bytes-k*skip*block);
		 }
	  else
		 {
		 for (k=0; k<bytes/skip/block; k++)
			{
			for (ptr=data+k*skip*block,i=0; i<skip; i++)
			   for (j=i; j<skip*block; j+=skip) data2[j]=*ptr++;

			memcpy(data+k*skip*block,data2,skip*block);
			}

		 for (ptr=data+k*skip*block,i=0; i<skip; i++)
			for (j=i; j<bytes-k*skip*block; j+=skip) data2[j]=*ptr++;

		 memcpy(data+k*skip*block,data2,bytes-k*skip*block);
		 }
	  }

   FREE(data2);
   }

// interleave a byte stream
void DDS_interleave(unsigned char *data,unsigned int bytes,unsigned int skip,unsigned int block=0)
   {DDS_deinterleave(data,bytes,skip,block,TRUE);}

// encode a Differential Data Stream
void DDS_encode(unsigned char *data,unsigned int bytes,unsigned int skip,unsigned int strip,
				unsigned char **chunk,unsigned int *size,
				unsigned int block=0)
   {
   int i;

   unsigned char lookup[256];

   unsigned char *ptr1,*ptr2;

   int pre1,pre2,
	   act1,act2,
	   tmp1,tmp2;

   unsigned int cnt,cnt1,cnt2;
   int bits,bits1,bits2;

   if (bytes<1) ERRORMSG();

   if (skip<1 || skip>4) skip=1;
   if (strip<1 || strip>65536) strip=1;

   DDS_deinterleave(data,bytes,skip,block);

   for (i=-128; i<128; i++)
	  {
	  if (i<=0)
		 for (bits=0; (1<<bits)/2<-i; bits++);
	  else
		 for (bits=0; (1<<bits)/2<=i; bits++);

	  lookup[i+128]=bits;
	  }

   DDS_initbuffer();

   DDS_clearbits();

   DDS_writebits(skip-1,2);
   DDS_writebits(strip-1,16);

   ptr1=ptr2=data;
   pre1=pre2=0;

   cnt=cnt1=cnt2=0;
   bits=bits1=bits2=0;

   while (cnt++<bytes)
	  {
	  tmp1=*ptr1;
	  if (strip==1 || ptr1-strip<=data) act1=tmp1-pre1;
	  else act1=tmp1-pre1-*(ptr1-strip)+*(ptr1-strip-1);
	  pre1=tmp1;
	  ptr1++;

	  while (act1<-128) act1+=256;
	  while (act1>127) act1-=256;

	  bits=lookup[act1+128];

	  bits=DDS_decode(DDS_code(bits));

	  if (cnt1==0)
		 {
		 cnt1++;
		 bits1=bits;
		 continue;
		 }

	  if (cnt1<(1<<DDS_RL)-1 && bits==bits1)
		 {
		 cnt1++;
		 continue;
		 }

	  if (cnt1+cnt2<(1<<DDS_RL) && (cnt1+cnt2)*MAX(bits1,bits2)<cnt1*bits1+cnt2*bits2+DDS_RL+3)
		 {
		 cnt2+=cnt1;
		 if (bits1>bits2) bits2=bits1;
		 }
	  else
		 {
		 DDS_writebits(cnt2,DDS_RL);
		 DDS_writebits(DDS_code(bits2),3);

		 while (cnt2-->0)
			{
			tmp2=*ptr2;
			if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
			else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
			pre2=tmp2;
			ptr2++;

			while (act2<-128) act2+=256;
			while (act2>127) act2-=256;

			DDS_writebits(act2+(1<<bits2)/2,bits2);
			}

		 cnt2=cnt1;
		 bits2=bits1;
		 }

	  cnt1=1;
	  bits1=bits;
	  }

   if (cnt1+cnt2<(1<<DDS_RL) && (cnt1+cnt2)*MAX(bits1,bits2)<cnt1*bits1+cnt2*bits2+DDS_RL+3)
	  {
	  cnt2+=cnt1;
	  if (bits1>bits2) bits2=bits1;
	  }
   else
	  {
	  DDS_writebits(cnt2,DDS_RL);
	  DDS_writebits(DDS_code(bits2),3);

	  while (cnt2-->0)
		 {
		 tmp2=*ptr2;
		 if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
		 else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
		 pre2=tmp2;
		 ptr2++;

		 while (act2<-128) act2+=256;
		 while (act2>127) act2-=256;

		 DDS_writebits(act2+(1<<bits2)/2,bits2);
		 }

	  cnt2=cnt1;
	  bits2=bits1;
	  }

   if (cnt2!=0)
	  {
	  DDS_writebits(cnt2,DDS_RL);
	  DDS_writebits(DDS_code(bits2),3);

	  while (cnt2-->0)
		 {
		 tmp2=*ptr2;
		 if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
		 else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
		 pre2=tmp2;
		 ptr2++;

		 while (act2<-128) act2+=256;
		 while (act2>127) act2-=256;

		 DDS_writebits(act2+(1<<bits2)/2,bits2);
		 }
	  }

   DDS_flushbits();
   DDS_savebits(chunk,size);

   DDS_interleave(data,bytes,skip,block);
   }

// decode a Differential Data Stream
void DDS_decode(unsigned char *chunk,unsigned int size,
				unsigned char **data,unsigned int *bytes,
				unsigned int block=0)
   {
   unsigned int skip,strip;

   unsigned char *ptr1,*ptr2;

   unsigned int cnt,cnt1,cnt2;
   int bits,act;

   DDS_initbuffer();

   DDS_clearbits();
   DDS_loadbits(chunk,size);

   skip=DDS_readbits(2)+1;
   strip=DDS_readbits(16)+1;

   ptr1=ptr2=NULL;
   cnt=act=0;

   while ((cnt1=DDS_readbits(DDS_RL))!=0)
	  {
	  bits=DDS_decode(DDS_readbits(3));

	  for (cnt2=0; cnt2<cnt1; cnt2++)
		 {
		 if (strip==1 || cnt<=strip) act+=DDS_readbits(bits)-(1<<bits)/2;
		 else act+=*(ptr2-strip)-*(ptr2-strip-1)+DDS_readbits(bits)-(1<<bits)/2;

		 while (act<0) act+=256;
		 while (act>255) act-=256;

		 if ((cnt&(DDS_BLOCKSIZE-1))==0)
			if (ptr1==NULL)
			   {
			   if ((ptr1=(unsigned char *)MALLOC(DDS_BLOCKSIZE))==NULL) MEMERROR();
			   ptr2=ptr1;
			   }
			else
			   {
			   if ((ptr1=(unsigned char *)REALLOC(ptr1,cnt+DDS_BLOCKSIZE))==NULL) MEMERROR();
			   ptr2=&ptr1[cnt];
			   }

		 *ptr2++=act;
		 cnt++;
		 }
	  }

   if (ptr1!=NULL)
	  if ((ptr1=(unsigned char *)REALLOC(ptr1,cnt))==NULL) MEMERROR();

   DDS_interleave(ptr1,cnt,skip,block);

   *data=ptr1;
   *bytes=cnt;
   }

// write a RAW file
void writeRAWfile(const char *filename,unsigned char *data,unsigned int bytes,BOOLINT nofree)
   {
   FILE *file;

   if (bytes<1) ERRORMSG();

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   if (fwrite(data,1,bytes,file)!=bytes) IOERROR();

   fclose(file);

   if (!nofree) FREE(data);
   }

// read from a RAW file
unsigned char *readRAWfiled(FILE *file,unsigned int *bytes)
   {
   unsigned char *data;
   unsigned int cnt,blkcnt;

   data=NULL;
   cnt=0;

   do
	  {
	  if (data==NULL)
		 {if ((data=(unsigned char *)MALLOC(DDS_BLOCKSIZE))==NULL) MEMERROR();}
	  else
		 if ((data=(unsigned char *)REALLOC(data,cnt+DDS_BLOCKSIZE))==NULL) MEMERROR();

	  blkcnt = (int) fread(&data[cnt],1,DDS_BLOCKSIZE,file);
	  cnt+=blkcnt;
	  }
   while (blkcnt==DDS_BLOCKSIZE);

   if (cnt==0)
	  {
	  FREE(data);
	  return(NULL);
	  }

   if ((data=(unsigned char *)REALLOC(data,cnt))==NULL) MEMERROR();

   *bytes=cnt;

   return(data);
   }

// read a RAW file
unsigned char *readRAWfile(const char *filename,unsigned int *bytes)
   {
   FILE *file;

   unsigned char *data;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   data=readRAWfiled(file,bytes);

   fclose(file);

   return(data);
   }

// write a Differential Data Stream
void writeDDSfile(const char *filename,unsigned char *data,unsigned int bytes,unsigned int skip,unsigned int strip,BOOLINT nofree)
   {
   int version=1;

   FILE *file;

   unsigned char *chunk;
   unsigned int size;

   if (bytes<1) ERRORMSG();

   if (bytes>DDS_INTERLEAVE) version=2;

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   fprintf(file,"%s",(version==1)?DDS_ID:DDS_ID2);

   DDS_encode(data,bytes,skip,strip,&chunk,&size,version==1?0:DDS_INTERLEAVE);

   if (chunk!=NULL)
	  {
	  if (fwrite(chunk,size,1,file)!=1) IOERROR();
	  FREE(chunk);
	  }

   fclose(file);

   if (!nofree) FREE(data);
   }

// read a Differential Data Stream
unsigned char *readDDSfile(const char *filename,unsigned int *bytes)
   {
   int version=1;

   FILE *file;

   int cnt;

   unsigned char *chunk,*data;
   unsigned int size;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   for (cnt=0; DDS_ID[cnt]!='\0'; cnt++)
	  if (fgetc(file)!=DDS_ID[cnt])
		 {
		 fclose(file);
		 version=0;
		 break;
		 }

   if (version==0)
	  {
	  if ((file=fopen(filename,"rb"))==NULL) return(NULL);

	  for (cnt=0; DDS_ID2[cnt]!='\0'; cnt++)
		 if (fgetc(file)!=DDS_ID2[cnt])
			{
			fclose(file);
			return(NULL);
			}

	  version=2;
	  }

   if ((chunk=readRAWfiled(file,&size))==NULL) IOERROR();

   fclose(file);

   DDS_decode(chunk,size,&data,bytes,version==1?0:DDS_INTERLEAVE);

   FREE(chunk);

   return(data);
   }

void swapshort(unsigned char *ptr,unsigned int size)
   {
   unsigned int i;

   unsigned char lo,hi;

   for (i=0; i<size; i++)
	  {
	  lo=ptr[0];
	  hi=ptr[1];
	  *ptr++=hi;
	  *ptr++=lo;
	  }
   }

// write an optionally compressed PNM image
void writePNMimage(const char *filename,unsigned char *image,unsigned int width,unsigned int height,unsigned int components,BOOLINT dds)
   {
   char str[DDS_MAXSTR];

   unsigned char *data;

   if (width<1 || height<1) ERRORMSG();

   switch (components)
	  {
	  case 1: snprintf(str,DDS_MAXSTR,"P5\n%d %d\n255\n",width,height); break;
	  case 2: snprintf(str,DDS_MAXSTR,"P5\n%d %d\n32767\n",width,height); break;
	  case 3: snprintf(str,DDS_MAXSTR,"P6\n%d %d\n255\n",width,height); break;
	  default: ERRORMSG();
	  }

   if ((data=(unsigned char *)MALLOC(strlen(str)+width*height*components))==NULL) MEMERROR();

   memcpy(data,str,strlen(str));
   memcpy(data+strlen(str),image,width*height*components);

   if (dds) writeDDSfile(filename,data,(unsigned int)strlen(str)+width*height*components,components,width);
   else writeRAWfile(filename,data,(unsigned int)strlen(str)+width*height*components);
   }

// read a possibly compressed PNM image
unsigned char *readPNMimage(const char *filename,unsigned int *width,unsigned int *height,unsigned int *components)
   {
   const int maxstr=100;

   char str[maxstr];

   unsigned char *data,*ptr1,*ptr2;
   unsigned int bytes;

   int pnmtype,maxval;
   unsigned char *image;

   if ((data=readDDSfile(filename,&bytes))==NULL)
	  if ((data=readRAWfile(filename,&bytes))==NULL) return(NULL);

   if (bytes<4) return(NULL);

   memcpy(str,data,3);
   str[3]='\0';

   if (sscanf(str,"P%1d\n",&pnmtype)!=1) return(NULL);

   ptr1=data+3;
   while (*ptr1=='\n' || *ptr1=='#')
	  {
	  while (*ptr1=='\n')
		 if (++ptr1>=data+bytes) ERRORMSG();
	  while (*ptr1=='#')
		 if (++ptr1>=data+bytes) ERRORMSG();
		 else
			while (*ptr1!='\n')
			   if (++ptr1>=data+bytes) ERRORMSG();
	  }

   ptr2=ptr1;
   while (*ptr2!='\n' && *ptr2!=' ')
	  if (++ptr2>=data+bytes) ERRORMSG();
   if (++ptr2>=data+bytes) ERRORMSG();
   while (*ptr2!='\n' && *ptr2!=' ')
	  if (++ptr2>=data+bytes) ERRORMSG();
   if (++ptr2>=data+bytes) ERRORMSG();
   while (*ptr2!='\n' && *ptr2!=' ')
	  if (++ptr2>=data+bytes) ERRORMSG();
   if (++ptr2>=data+bytes) ERRORMSG();

   if (ptr2-ptr1>=maxstr) ERRORMSG();
   memcpy(str,ptr1,ptr2-ptr1);
   str[ptr2-ptr1]='\0';

   if (sscanf(str,"%d %d\n%d\n",width,height,&maxval)!=3) ERRORMSG();

   if (*width<1 || *height<1) ERRORMSG();

   if (pnmtype==5 && maxval==255) *components=1;
   else if (pnmtype==5 && (maxval==32767 || maxval==65535)) *components=2;
   else if (pnmtype==6 && maxval==255) *components=3;
   else ERRORMSG();

   if ((image=(unsigned char *)MALLOC((*width)*(*height)*(*components)))==NULL) MEMERROR();
   if (data+bytes!=ptr2+(*width)*(*height)*(*components)) ERRORMSG();

   memcpy(image,ptr2,(*width)*(*height)*(*components));
   FREE(data);

   return(image);
   }

// write a compressed PVM volume
void writePVMvolume(const char *filename,unsigned char *volume,
					unsigned int width,unsigned int height,unsigned int depth,unsigned int components,
					float scalex,float scaley,float scalez,
					unsigned char *description,
					unsigned char *courtesy,
					unsigned char *parameter,
					unsigned char *comment)
   {
   char str[DDS_MAXSTR];

   unsigned char *data;

   unsigned int len1=1,len2=1,len3=1,len4=1;

   if (width<1 || height<1 || depth<1 || components<1) ERRORMSG();

   if (description==NULL && courtesy==NULL && parameter==NULL && comment==NULL)
	  if (scalex==1.0f && scaley==1.0f && scalez==1.0f)
		 snprintf(str,DDS_MAXSTR,"PVM\n%d %d %d\n%d\n",width,height,depth,components);
	  else
		 snprintf(str,DDS_MAXSTR,"PVM2\n%d %d %d\n%g %g %g\n%d\n",width,height,depth,scalex,scaley,scalez,components);
   else
	  snprintf(str,DDS_MAXSTR,"PVM3\n%d %d %d\n%g %g %g\n%d\n",width,height,depth,scalex,scaley,scalez,components);

   size_t size = width*height*depth*components;
   if (description==NULL && courtesy==NULL && parameter==NULL && comment==NULL)
	  {
	  if ((data=(unsigned char *)MALLOC(strlen(str)+size))==NULL) MEMERROR();

	  memcpy(data,str,strlen(str));
	  memcpy(data+strlen(str),volume,size);

	  writeDDSfile(filename,data,(unsigned int)(strlen(str)+size),(unsigned int)components,width);
	  }
   else
	  {
	  if (description!=NULL)	len1=(int)strlen((char *)description)+1;
	  if (courtesy!=NULL)		len2=(int)strlen((char *)courtesy)+1;
	  if (parameter!=NULL)		len3=(int)strlen((char *)parameter)+1;
	  if (comment!=NULL)		len4=(int)strlen((char *)comment)+1;

	  if ((data=(unsigned char *)MALLOC(strlen(str)+size+len1+len2+len3+len4))==NULL) MEMERROR();

	  memcpy(data,str,strlen(str));
	  memcpy(data+strlen(str),volume,size);

	  if (description==NULL) *(data+strlen(str)+width*height*depth*components)='\0';
	  else memcpy(data+strlen(str)+size,description,len1);

	  if (courtesy==NULL) *(data+strlen(str)+size+len1)='\0';
	  else memcpy(data+strlen(str)+size+len1,courtesy,len2);

	  if (parameter==NULL) *(data+strlen(str)+size+len1+len2)='\0';
	  else memcpy(data+strlen(str)+size+len1+len2,parameter,len3);

	  if (comment==NULL) *(data+strlen(str)+width*height*depth*components+len1+len2+len3)='\0';
	  else memcpy(data+strlen(str)+size+len1+len2+len3,comment,len4);

	  writeDDSfile(filename,data,(unsigned int)(strlen(str)+size+len1+len2+len3+len4),(unsigned int)components,width);
	  }
   }

// read a compressed PVM volume
unsigned char *readPVMvolume(const char *filename,
							 unsigned int *width,unsigned int *height,unsigned int *depth,unsigned int *components,
							 float *scalex,float *scaley,float *scalez,
							 unsigned char **description,
							 unsigned char **courtesy,
							 unsigned char **parameter,
							 unsigned char **comment)
   {
   unsigned char *data,*ptr;
   unsigned int bytes,numc;

   int version=1;

   unsigned char *volume;

   float sx=1.0f,sy=1.0f,sz=1.0f;

   unsigned int len1=0,len2=0,len3=0,len4=0;

   if ((data=readDDSfile(filename,&bytes))==NULL)
	  if ((data=readRAWfile(filename,&bytes))==NULL) return(NULL);

   if (bytes<5) return(NULL);

   if ((data=(unsigned char *)REALLOC(data,bytes+1))==NULL) MEMERROR();
   data[bytes]='\0';

   if (strncmp((char *)data,"PVM\n",4)!=0)
	  {
	  if (strncmp((char *)data,"PVM2\n",5)==0) version=2;
	  else if (strncmp((char *)data,"PVM3\n",5)==0) version=3;
	  else return(NULL);

	  ptr=&data[5];
	  if (sscanf((char *)ptr,"%d %d %d\n%g %g %g\n",width,height,depth,&sx,&sy,&sz)!=6) ERRORMSG();
	  if (*width<1 || *height<1 || *depth<1 || sx<=0.0f || sy<=0.0f || sz<=0.0f) ERRORMSG();
	  ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
	  }
   else
	  {
	  ptr=&data[4];
	  while (*ptr=='#')
		 while (*ptr++!='\n');

	  if (sscanf((char *)ptr,"%d %d %d\n",width,height,depth)!=3) ERRORMSG();
	  if (*width<1 || *height<1 || *depth<1) ERRORMSG();
	  }

   if (scalex!=NULL && scaley!=NULL && scalez!=NULL)
	  {
	  *scalex=sx;
	  *scaley=sy;
	  *scalez=sz;
	  }

   ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
   if (sscanf((char *)ptr,"%d\n",&numc)!=1) ERRORMSG();
   if (numc<1) ERRORMSG();

   if (components!=NULL) *components=numc;
   else if (numc!=1) ERRORMSG();

   size_t size = (*width)*(*height)*(*depth)*numc;

   ptr=(unsigned char *)strchr((char *)ptr,'\n')+1;
   if (version==3) len1=(int)strlen((char *)(ptr+size))+1;
   if (version==3) len2=(int)strlen((char *)(ptr+size+len1))+1;
   if (version==3) len3=(int)strlen((char *)(ptr+size+len1+len2))+1;
   if (version==3) len4=(int)strlen((char *)(ptr+size+len1+len2+len3))+1;
   if ((volume=(unsigned char *)MALLOC(size+len1+len2+len3+len4))==NULL) MEMERROR();
   if (data+bytes!=ptr+size+len1+len2+len3+len4) ERRORMSG();

   memcpy(volume,ptr,size+len1+len2+len3+len4);
   FREE(data);

   if (description!=NULL)
	  if (len1>1) *description=volume+size;
	  else *description=NULL;

   if (courtesy!=NULL)
	  if (len2>1) *courtesy=volume+size+len1;
	  else *courtesy=NULL;

   if (parameter!=NULL)
	  if (len3>1) *parameter=volume+size+len1+len2;
	  else *parameter=NULL;

   if (comment!=NULL)
	  if (len4>1) *comment=volume+size+len1+len2+len3;
	  else *comment=NULL;

   return(volume);
   }

// check a file
int checkfile(const char *filename)
   {
   FILE *file;

   if ((file=fopen(filename,"rb"))==NULL) return(0);
   fclose(file);

   return(1);
   }

// simple checksum algorithm
unsigned int checksum(unsigned char *data,unsigned int bytes)
   {
   const unsigned int prime=271;

   unsigned int i;

   unsigned char *ptr,value;

   unsigned int sum,cipher;

   sum=0;
   cipher=1;

   for (ptr=data,i=0; i<bytes; i++)
	  {
	  value=*ptr++;
	  cipher=prime*cipher+value;
	  sum+=cipher*value;
	  }

   return(sum);
   }

// swap the hi and lo byte of 16 bit data
void swapbytes(unsigned char *data,long long bytes)
   {
   long long i;
   unsigned char *ptr,tmp;

   for (ptr=data,i=0; i<bytes/2; i++,ptr+=2)
	  {
	  tmp=*ptr;
	  *ptr=*(ptr+1);
	  *(ptr+1)=tmp;
	  }
   }

// convert from signed short to unsigned short
void convbytes(unsigned char *data,long long bytes)
   {
   long long i;
   unsigned char *ptr;
   int v,vmin;

   for (vmin=32767,ptr=data,i=0; i<bytes/2; i++,ptr+=2)
	  {
	  v=256*(*ptr)+*(ptr+1);
	  if (v>32767) v=v-65536;
	  if (v<vmin) vmin=v;
	  }

   for (ptr=data,i=0; i<bytes/2; i++,ptr+=2)
	  {
	  v=256*(*ptr)+*(ptr+1);
	  if (v>32767) v=v-65536;

	  *ptr=(v-vmin)/256;
	  *(ptr+1)=(v-vmin)%256;
	  }
   }

// convert from float to unsigned short
void convfloat(unsigned char **data,long long bytes)
   {
   long long i;
   unsigned char *ptr;
   float v,vmax;

   for (vmax=1.0f,ptr=*data,i=0; i<bytes/4; i++,ptr+=4)
	  {
	  if (DDS_ISINTEL) DDS_swapuint((unsigned int *)ptr);

	  v = (float) fabs(*((float *)ptr));
	  if (v>vmax) vmax=v;
	  }

   for (ptr=*data,i=0; i<bytes/4; i++,ptr+=4)
	  {
	  v = (float) fabs(*((float *)ptr))/vmax;

	  (*data)[ 2*i]		= I_FLOOR(65535.0f*v+0.5f) / 256;
	  (*data)[ 2*i+1 ]	= I_FLOOR(65535.0f*v+0.5f) % 256;
	  }

   if ((*data=(unsigned char *)REALLOC(*data,size_t(bytes/4*2)))==NULL) MEMERROR();
   }

// convert from rgb to byte
void convrgb(unsigned char **data,long long bytes)
   {
   long long i;
   unsigned char *ptr1,*ptr2;

   for (ptr1=ptr2=*data,i=0; i<bytes/3; i++,ptr1+=3,ptr2++)
	  *ptr2=((*ptr1)+*(ptr1+1)+*(ptr1+2)+1)/3;

   if ((*data=(unsigned char *)REALLOC(*data,size_t(bytes/3)))==NULL) MEMERROR();
   }

// helper to get a short value from a volume
inline int getshort(unsigned short int *data,
					long long width,long long height,long long depth,
					long long i,long long j,long long k)
   {return(data[i+(j+k*height)*width]);}

// helper to get a gradient value from a volume
inline double getgrad(unsigned short int *data,
					  long long width,long long height,long long depth,
					  long long i,long long j,long long k)
   {
   double gx,gy,gz;

   if (i>0)
	  if (i<width-1) gx=(getshort(data,width,height,depth,i+1,j,k)-getshort(data,width,height,depth,i-1,j,k))/2.0;
	  else gx=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i-1,j,k);
   else
	  if (i<width-1) gx=getshort(data,width,height,depth,i+1,j,k)-getshort(data,width,height,depth,i,j,k);
	  else gx=0.0;

   if (j>0)
	  if (j<height-1) gy=(getshort(data,width,height,depth,i,j+1,k)-getshort(data,width,height,depth,i,j-1,k))/2.0;
	  else gy=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i,j-1,k);
   else
	  if (j<height-1) gy=getshort(data,width,height,depth,i,j+1,k)-getshort(data,width,height,depth,i,j,k);
	  else gy=0.0;

   if (k>0)
	  if (k<depth-1) gz=(getshort(data,width,height,depth,i,j,k+1)-getshort(data,width,height,depth,i,j,k-1))/2.0;
	  else gz=getshort(data,width,height,depth,i,j,k)-getshort(data,width,height,depth,i,j,k-1);
   else
	  if (k<depth-1) gz=getshort(data,width,height,depth,i,j,k+1)-getshort(data,width,height,depth,i,j,k);
	  else gz=0.0;

   return(sqrt(gx*gx+gy*gy+gz*gz));
   }

// quantize 16 bit data to 8 bit using a non-linear mapping
unsigned char *quantize(unsigned char *data,
						long long width,long long height,long long depth,
						BOOLINT msb,
						BOOLINT linear,BOOLINT nofree)
   {
   long long i,j,k;

   unsigned char *data2;
   unsigned short int *data3;
   long long idx;

   int v,vmin,vmax;

   double *err,eint;

   BOOLINT done;

   if ((data3=(unsigned short int*)MALLOC(size_t(width*height*depth*sizeof(unsigned short int))))==NULL) MEMERROR();

   vmin=65535;
   vmax=0;

   for (k=0; k<depth; k++)
	  for (j=0; j<height; j++)
		 for (i=0; i<width; i++)
			{
			idx=i+(j+k*height)*width;

			if (msb)
			   v=256*data[2*idx]+data[2*idx+1];
			else
			   v=data[2*idx]+256*data[2*idx+1];
			data3[idx]=v;

			if (v<vmin) vmin=v;
			if (v>vmax) vmax=v;
			}

   if (!nofree) FREE(data);

   if (vmin==vmax) vmax=vmin+1;

   if (vmax-vmin<256)
	   linear=TRUE;

   err=new double[65536];

   if (linear)
	  for (i=0; i<65536; i++) err[i]=255*(double)(i-vmin)/(vmax-vmin);
   else
	  {
	  for (i=0; i<65536; i++) err[i]=0.0;

	  for (k=0; k<depth; k++)
		 for (j=0; j<height; j++)
			for (i=0; i<width; i++)
			   err[getshort(data3,width,height,depth,i,j,k)]+=sqrt(getgrad(data3,width,height,depth,i,j,k));

	  for (i=0; i<65536; i++) err[i]=pow(err[i],1.0/3);

	  err[vmin]=err[vmax]=0.0;

	  for (k=0; k<256; k++)
		 {
		 for (eint=0.0,i=0; i<65536; i++) eint+=err[i];

		 done=TRUE;

		 for (i=0; i<65536; i++)
			if (err[i]>eint/256)
			   {
			   err[i]=eint/256;
			   done=FALSE;
			   }

		 if (done) break;
		 }

	  for (i=1; i<65536; i++) err[i]+=err[i-1];

	  if (err[65535]>0.0f)
		 for (i=0; i<65536; i++) err[i]*=255.0/err[65535];
	  }

   if ((data2=(unsigned char *)MALLOC(size_t(width*height*depth)))==NULL) MEMERROR();

   for (k=0; k<depth; k++)
	  for (j=0; j<height; j++)
		 for (i=0; i<width; i++)
			{
			idx=i+(j+k*height)*width;
			data2[idx]=(int)(err[data3[idx]]+0.5);
			}

   delete[] err;
   FREE(data3);

   return(data2);
   }

//unused
/*
// copy a PVM volume to a RAW volume
char *processPVMvolume(const char *filename)
   {
   unsigned char *volume;

   unsigned int width,height,depth,
				components;

   float scalex,scaley,scalez;

   char *output,*dot;
   char *outname;

   // read and uncompress PVM volume
   if ((volume=readPVMvolume(filename,
							 &width,&height,&depth,&components,
							 &scalex,&scaley,&scalez))==NULL) return(NULL);

   // use input file name as output prefix
   output=strdup(filename);
   dot=strrchr(output,'.');

   // remove suffix from output
   if( dot!=NULL )
	  if( strcasecmp(dot,".pvm")==0 )
		  *dot='\0';

   outname=NULL;

#ifdef HAVE_MINI

   // copy PVM data to RAW file
   outname=writeRAWvolume(output,volume,
						  width,height,depth,1,
						  components,8,FALSE,TRUE,
						  scalex,scaley,scalez);

#endif

   FREE(volume);
   FREE(output);

   return(outname);
   }
*/