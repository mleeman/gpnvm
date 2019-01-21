#include "gpnvm.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

/** SECTION: gpnvm
 * @title: Simple Non-Volatile Memory Storage
 *
 * See README.md for full description
 */

/* file pointer */
static FILE *fp;

/**
 * gpNvm_OpenFile:
 * @filename: file to open
 *
 * Open the file, if the file is not present, return a fp to an newly
 * created file.
 *
 * Returns: custom error code
 */
gpNvm_Result gpNvm_OpenFile(const char *filename)
{
	if (fp || !filename)
		return 1;

	fp = fopen(filename, "r+");
	if (!fp)
		fp = fopen(filename, "w+");

	return fp == NULL;
}

/**
 * gpNvm_CloseFile:
 *
 * Close the file global handle
 *
 * Returns: custom error code
 */
gpNvm_Result gpNvm_CloseFile(void)
{
	gpNvm_Result ret = fp ? fclose(fp) : 1;
	fp = NULL;
	return ret;
}

/**
 * gpNvm_Read:
 * @ptr: location to read
 * @len: length to read
 *
 * Simple wrapper around fread, read byte wise
 *
 * Returns: custom error code and mask the bytes read return code
 * from fread. Success if number of bytes asked to read is number of
 * bytes read.
 */
static int gpNvm_Read(void *ptr, int len)
{
	return fread(ptr, 1, len, fp) == len;
}

/**
 * gpNvm_Write:
 * @ptr: location to the byte array to write
 * @len: number of elements to write
 *
 * Simple wrapper around fwrite, write byte wise
 *
 * Returns: custom error code ad mask bytes write. Success if number of
 * bytes pass is number of bytes written.
 */
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

/**
 * gpNvm_checksum:
 * @pvalue: pointer to byte array to compute CRC on
 * @length: number of bytes to CRC
 *
 * Return checksum
 *
 * Returns: 16bit CRC
 */
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
