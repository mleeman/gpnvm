#include "gpnvm.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

static FILE *fp;

gpNvm_Result gpNvm_OpenFile(const char *filename)
{
	if (fp || !filename)
		return 1;

	fp = fopen(filename, "r+");
	if (!fp)
		fp = fopen(filename, "w+");

	return fp == NULL;
}

gpNvm_Result gpNvm_CloseFile(void)
{
	gpNvm_Result ret = fp ? fclose(fp) : 1;
	fp = NULL;
	return ret;
}

static int gpNvm_Read(void *ptr, int len)
{
	return fread(ptr, 1, len, fp) == len;
}

static int gpNvm_Write(void *ptr, int len)
{
	return fwrite(ptr, 1, len, fp) == len;
}

static int gpNvm_SeekFile(gpNvm_AttrId key, UInt8 *pLength)
{
	gpNvm_AttrId attrId;
	UInt16 sum;
	UInt8 len;

	rewind(fp);

	while (1) {
		if (!gpNvm_Read(&attrId, sizeof attrId))
			return -1;
		if (!gpNvm_Read(&len, sizeof len))
			return -1;
		if (!gpNvm_Read(&sum, sizeof sum))
			return -1;
		if (sum != attrId + len)
			return -1;
		if (attrId == key)
			return *pLength = len, ftell(fp);
		if (fseek(fp, len, SEEK_CUR) != 0)
			return -1;
	}

	// uh?
	return -1;
}

static UInt16 gpNvm_checksum(UInt8 *pValue, UInt8 length)
{
	int i, sum = 0;

	for (i = 0; i != length; i++)
		sum += pValue[i];

	return sum & 0xffff;
}

gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId attrId, UInt8 *pLength, UInt8 *pValue)
{
	UInt16 sum;
	UInt8 len;

	if (!pLength || !*pLength || !pValue || !fp || fileno(fp) < 0)
		return 1;

	if (gpNvm_SeekFile(attrId, &len) < 0 || len != *pLength + sizeof sum)
		return 1;
	if (!gpNvm_Read(pValue, *pLength))
		return 1;
	if (!gpNvm_Read(&sum, sizeof sum))
		return 1;

	return sum != gpNvm_checksum(pValue, *pLength);
}

gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId attrId, UInt8 length, UInt8 *pValue)
{
	UInt16 sum = attrId + length + sizeof sum;
	UInt8 len = length + sizeof sum;

	if (!length || !pValue || !fp || fileno(fp) < 0)
		return 1;

	if (gpNvm_SeekFile(attrId, &len) > 0 && len != length + sizeof sum)
		return 1;
	if (!gpNvm_Write(&attrId, sizeof attrId))
		return 1;
	if (!gpNvm_Write(&len, sizeof len))
		return 1;
	if (!gpNvm_Write(&sum, sizeof sum))
		return 1;
	if (!gpNvm_Write(pValue, length))
		return 1;

	sum = gpNvm_checksum(pValue, length);
	if (!gpNvm_Write(&sum, sizeof sum))
		return 1;

	return !!fflush(fp);
}
