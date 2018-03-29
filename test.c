#include "gpnvm.h"
#include "CuTest.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

static const char *gpNvm_file_Test = "test.nvm";

static void gpNvm_preliminary_Test(CuTest* tc)
{
	/* paranoid safety checks */
	CuAssertTrue(tc, sizeof(UInt8) == 1);
	CuAssertTrue(tc, sizeof(UInt16) == 2);
}

static void gpNvm_OpenCloseFile_Test(CuTest* tc)
{
	gpNvm_Result result;

	/* is NULL file name detected? */
	result = gpNvm_OpenFile(NULL);
	CuAssertTrue(tc, result == 1);

	/* is closing without prior opening detected? */
	result = gpNvm_CloseFile();
	CuAssertTrue(tc, result == 1);

	/* is opening succeeding? */
	result = gpNvm_OpenFile(gpNvm_file_Test);
	CuAssertTrue(tc, result == 0);

	/* is opening again detected? */
	result = gpNvm_OpenFile(gpNvm_file_Test);
	CuAssertTrue(tc, result == 1);

	/* is closing with prior opening succeeding? */
	result = gpNvm_CloseFile();
	CuAssertTrue(tc, result == 0);
}

static void gpNvm_GetAttribute_Test(CuTest* tc)
{
	gpNvm_AttrId attrId = 0xda;
	gpNvm_Result result;

	UInt8 value[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, };
	UInt8 length = sizeof(value);
	UInt8 zeroLength = 0;

	/* is NULL length pointer detected? */
	result = gpNvm_GetAttribute(attrId, NULL, value);
	CuAssertTrue(tc, result == 1);

	/* is zero length detected? */
	result = gpNvm_GetAttribute(attrId, &zeroLength, value);
	CuAssertTrue(tc, result == 1);

	/* is NULL value pointer detected? */
	result = gpNvm_GetAttribute(attrId, &length, NULL);
	CuAssertTrue(tc, result == 1);

	/* is valid length/value but not opened file detected? */
	result = gpNvm_GetAttribute(attrId, &length, value);
	CuAssertTrue(tc, result == 1);

	/* delete the persistence file if exists */
	unlink(gpNvm_file_Test);

	/* is opening succeeding? */
	result = gpNvm_OpenFile(gpNvm_file_Test);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value but non-existent entry detected? */
	result = gpNvm_GetAttribute(attrId, &length, value);
	CuAssertTrue(tc, result == 1);

	/* is closing succeeding? */
	result = gpNvm_CloseFile();
	CuAssertTrue(tc, result == 0);
}

static void gpNvm_SetAttribute_Test(CuTest* tc)
{
	gpNvm_AttrId attrId = 0xda;
	gpNvm_Result result;

	UInt8 value[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, };
	UInt8 length = sizeof(value);

	UInt8 sameValue[sizeof(value)] = { 0 };
	UInt8 sameLength = sizeof(sameValue);

	UInt8 shortValue[sizeof(value) - 1];
	UInt8 shortLength = sizeof(shortValue);

	UInt8 longValue[sizeof(value) + 1];
	UInt8 longLength = sizeof(longValue);

	/* is zero length detected? */
	result = gpNvm_SetAttribute(attrId, 0, value);
	CuAssertTrue(tc, result == 1);

	/* is NULL value pointer detected? */
	result = gpNvm_SetAttribute(attrId, length, NULL);
	CuAssertTrue(tc, result == 1);

	/* is valid length/value but not opened file detected? */
	result = gpNvm_SetAttribute(attrId, length, value);
	CuAssertTrue(tc, result == 1);

	/* delete the persistence file if exists */
	unlink(gpNvm_file_Test);

	/* is opening succeeding? */
	result = gpNvm_OpenFile(gpNvm_file_Test);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value accepted? */
	result = gpNvm_SetAttribute(attrId, length, value);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value accepted? */
	result = gpNvm_SetAttribute(attrId + 1, length, value);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value accepted? */
	result = gpNvm_SetAttribute(attrId + 2, length, value);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value accepted? */
	result = gpNvm_SetAttribute(attrId + 3, length, value);
	CuAssertTrue(tc, result == 0);

	/* is valid length/value but non-existent entry detected? */
	result = gpNvm_GetAttribute(attrId, &sameLength, sameValue);
	CuAssertTrue(tc, result == 0);
	CuAssertTrue(tc, memcmp(value, sameValue, length) == 0);

	/* is valid length/value but non-existent entry detected? */
	result = gpNvm_GetAttribute(attrId + 1, &sameLength, sameValue);
	CuAssertTrue(tc, result == 0);
	CuAssertTrue(tc, memcmp(value, sameValue, length) == 0);

	/* is valid length/value but non-existent entry detected? */
	result = gpNvm_GetAttribute(attrId + 2, &sameLength, sameValue);
	CuAssertTrue(tc, result == 0);
	CuAssertTrue(tc, memcmp(value, sameValue, length) == 0);

	/* is valid length/value but non-existent entry detected? */
	result = gpNvm_GetAttribute(attrId + 3, &sameLength, sameValue);
	CuAssertTrue(tc, result == 0);
	CuAssertTrue(tc, memcmp(value, sameValue, length) == 0);

	/* is valid length/value but smaller detected? */
	result = gpNvm_GetAttribute(attrId, &shortLength, shortValue);
	CuAssertTrue(tc, result == 1);

	/* is valid length/value but longer detected? */
	result = gpNvm_GetAttribute(attrId, &longLength, longValue);
	CuAssertTrue(tc, result == 1);

	/* is closing succeeding? */
	result = gpNvm_CloseFile();
	CuAssertTrue(tc, result == 0);
}

static int RunAllTests(void)
{
	int failCount = 0;

	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, gpNvm_preliminary_Test);
	SUITE_ADD_TEST(suite, gpNvm_OpenCloseFile_Test);
	SUITE_ADD_TEST(suite, gpNvm_GetAttribute_Test);
	SUITE_ADD_TEST(suite, gpNvm_SetAttribute_Test);

	CuSuiteRun(suite);
	failCount = suite->failCount;

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
	CuStringDelete(output);
	CuSuiteDelete(suite);

	return failCount;
}

int main(int argc, char *argv[])
{
	return RunAllTests();
}
