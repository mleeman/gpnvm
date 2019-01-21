# Introduction

This is a small exercise to show your skill and style in embedded C programming.

# Exercise

The exercise exists in the implementation of a non-volatile memory storage
component. This component should be able to backup and restore values
corresponding to a unique attribute identifier. For the simplicity of
the exercise the underlying non-volatile memory (eeprom, flash, ...) can
be modelled as a file.

# Component Specification

Implement the basic implementation of this component based on following
API description:

```c
typedef unsigned char UInt8;
typedef UInt8 gpNvm_AttrId;
typedef UInt8 gpNvm_Result;

gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId, pLength, UInt8* pValue);
gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId, attrId, UInt8* attrId, UInt8 length, UInt8* pValue);
```

Make sure the implementation supports different datatypes for pValue

Some examples:

... Basic datatypes like UInt8 , UInt32 ,...
... An array of UInt8
... Structs like for instance:

```c
#define MAX_LENGTH 20
typedef struct {
UInt8
id;
UInt32 options;
UInt8 length;
UInt8 data[MAX_LENGTH];
} gpTestData_t;
```

... Extend the basic implementation so it discovers corruptions in the underlying storage.
... Extend the basic implementation so it can recover from corruptions in the underlying
storage.
... Extend the your solution with an unit test.


[![Build Status](https://travis-ci.org/cavokz/gpnvm.svg?branch=master)](https://travis-ci.org/cavokz/gpnvm)
