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

static int gpNvm_SeekFile(gpNvm_AttrId key, UInt8 *pLength)
{
	UInt8 attrId;
	UInt8 length;

	rewind(fp);

	while (1) {
		if (fread(&attrId, 1, 1, fp) < 1)
			return -1;
		if (fread(&length, 1, 1, fp) < 1)
			return -1;
		if (attrId == key)
			return *pLength = length, ftell(fp);
		if (fseek(fp, length, SEEK_CUR) != 0)
			return -1;
	}

	// uh?
	return -1;
}

gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId attrId, UInt8 *pLength, UInt8 *pValue)
{
	UInt8 len;

	if (!pLength || !*pLength || !pValue || !fp || fileno(fp) < 0)
		return 1;

	if (gpNvm_SeekFile(attrId, &len) < 0)
		return 1;

	if (len != *pLength)
		return 1;
	if (fread(pValue, 1, len, fp) != len)
		return 1;

	return 0;
}

gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId attrId, UInt8 length, UInt8 *pValue)
{
	UInt8 len;

	if (!length || !pValue || !fp || fileno(fp) < 0)
		return 1;

	if (gpNvm_SeekFile(attrId, &len) >= 0 && len != length)
		return 1;

	if (fwrite(&attrId, 1, 1, fp) < 1)
		return 1;
	if (fwrite(&length, 1, 1, fp) < 1)
		return 1;
	if (fwrite(pValue, 1, length, fp) < length)
		return 1;

	return !!fflush(fp);
}
