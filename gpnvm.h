#ifndef __GPNVM_H_20180325__
#define __GPNVM_H_20180325__

typedef unsigned char UInt8;
typedef unsigned short UInt16;

typedef UInt8 gpNvm_AttrId;
typedef UInt8 gpNvm_Result;

gpNvm_Result gpNvm_OpenFile(const char *filename);
gpNvm_Result gpNvm_CloseFile(void);

gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId attrId, UInt8 *pLength, UInt8 *pValue);
gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId attrId, UInt8 length, UInt8 *pValue);

#endif /* __GPNVM_H_20180325__ */
