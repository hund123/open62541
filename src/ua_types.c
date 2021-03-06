#include "ua_util.h"
#include "ua_types.h"
#include "ua_statuscodes.h"
#include "ua_types_generated.h"

/*****************/
/* Helper Macros */
/*****************/

#define UA_TYPE_DEFAULT(TYPE)            \
    UA_TYPE_NEW_DEFAULT(TYPE)            \
    UA_TYPE_INIT_DEFAULT(TYPE)           \
    UA_TYPE_DELETEMEMBERS_NOACTION(TYPE) \
    UA_TYPE_DELETE_DEFAULT(TYPE)         \
    UA_TYPE_COPY_DEFAULT(TYPE)           \

#define UA_TYPE_NEW_DEFAULT(TYPE)                              \
    TYPE * TYPE##_new() {                                      \
        TYPE *p = UA_malloc(sizeof(TYPE));                     \
        if(p) TYPE##_init(p);                                  \
        return p;                                              \
    }

#define UA_TYPE_INIT_DEFAULT(TYPE) \
    void TYPE##_init(TYPE * p) {   \
        *p = (TYPE)0;              \
    }

#define UA_TYPE_DELETEMEMBERS_NOACTION(TYPE) \
    void TYPE##_deleteMembers(TYPE *p) {    \
    }

#define UA_TYPE_DELETE_DEFAULT(TYPE) \
    void TYPE##_delete(TYPE *p) {    \
        TYPE##_deleteMembers(p);     \
        UA_free(p);                  \
    }

#define UA_TYPE_COPY_DEFAULT(TYPE)                             \
    UA_StatusCode TYPE##_copy(TYPE const *src, TYPE *dst) {    \
        *dst = *src;                                           \
        return UA_STATUSCODE_GOOD;                             \
    }

/*****************/
/* Builtin Types */
/*****************/

/* Boolean */
UA_TYPE_DEFAULT(UA_Boolean)

/* SByte */
UA_TYPE_DEFAULT(UA_SByte)

/* Byte */
UA_TYPE_DEFAULT(UA_Byte)

/* Int16 */
UA_TYPE_DEFAULT(UA_Int16)

/* UInt16 */
UA_TYPE_DEFAULT(UA_UInt16)

/* Int32 */
UA_TYPE_DEFAULT(UA_Int32)

/* UInt32 */
UA_TYPE_DEFAULT(UA_UInt32)

/* Int64 */
UA_TYPE_DEFAULT(UA_Int64)

/* UInt64 */
UA_TYPE_DEFAULT(UA_UInt64)

/* Float */
UA_TYPE_DEFAULT(UA_Float)

/* Double */
UA_TYPE_DEFAULT(UA_Double)

/* String */
UA_TYPE_NEW_DEFAULT(UA_String)
void UA_String_init(UA_String *p) {
    p->length = -1;
    p->data   = UA_NULL;
}

UA_TYPE_DELETE_DEFAULT(UA_String)
void UA_String_deleteMembers(UA_String *p) {
	if(p->data) {
		UA_free(p->data);
        p->data = UA_NULL;
    }
}

UA_StatusCode UA_String_copy(UA_String const *src, UA_String *dst) {
    UA_String_init(dst);
    if(src->length > 0) {
        if(!(dst->data = UA_malloc((UA_UInt32)src->length)))
            return UA_STATUSCODE_BADOUTOFMEMORY;
        UA_memcpy((void *)dst->data, src->data, (UA_UInt32)src->length);
    }
    dst->length = src->length;
    return UA_STATUSCODE_GOOD;
}

/* The c-string needs to be null-terminated. the string cannot be smaller than zero. */
UA_StatusCode UA_String_copycstring(char const *src, UA_String *dst) {
    UA_String_init(dst);
    size_t length = (UA_UInt32) strlen(src);
    if(length == 0) {
        dst->length = 0;
        dst->data = UA_NULL;
        return UA_STATUSCODE_GOOD;
    }
    dst->data = UA_malloc(length);
    if(!dst->data)
        return UA_STATUSCODE_BADOUTOFMEMORY;
    UA_memcpy(dst->data, src, length);
    dst->length = length;
    return UA_STATUSCODE_GOOD;
}

#define UA_STRING_COPYPRINTF_BUFSIZE 1024
UA_StatusCode UA_String_copyprintf(char const *fmt, UA_String *dst, ...) {
    char src[UA_STRING_COPYPRINTF_BUFSIZE];
    va_list ap;
    va_start(ap, dst);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    // vsnprintf should only take a literal and no variable to be secure
    UA_Int32 len = vsnprintf(src, UA_STRING_COPYPRINTF_BUFSIZE, fmt, ap);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
    va_end(ap);
    if(len < 0)  // FIXME: old glibc 2.0 would return -1 when truncated
        return UA_STATUSCODE_BADINTERNALERROR;
    // since glibc 2.1 vsnprintf returns the len that would have resulted if buf were large enough
    len = ( len > UA_STRING_COPYPRINTF_BUFSIZE ? UA_STRING_COPYPRINTF_BUFSIZE : len );
    if(!(dst->data = UA_malloc((UA_UInt32)len)))
        return UA_STATUSCODE_BADOUTOFMEMORY;
    UA_memcpy((void *)dst->data, src, (UA_UInt32)len);
    dst->length = len;
    return UA_STATUSCODE_GOOD;
}

UA_Boolean UA_String_equal(const UA_String *string1, const UA_String *string2) {
    if(string1->length <= 0 && string2->length <= 0)
        return UA_TRUE;
    if(string1->length != string2->length)
        return UA_FALSE;

    // casts are needed to overcome signed warnings
    UA_Int32 is = strncmp((char const *)string1->data, (char const *)string2->data, (size_t)string1->length);
    return (is == 0) ? UA_TRUE : UA_FALSE;
}

/* DateTime */
#define UNIX_EPOCH_BIAS_SEC 11644473600LL // Number of seconds from 1 Jan. 1601 00:00 to 1 Jan 1970 00:00 UTC
#define HUNDRED_NANOSEC_PER_USEC 10LL
#define HUNDRED_NANOSEC_PER_SEC (HUNDRED_NANOSEC_PER_USEC * 1000000LL)

#ifdef __MINGW32__
#ifndef _TIMEZONE_DEFINED
#define _TIMEZONE_DEFINED
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};
#endif
#endif
#ifdef _WIN32
static const unsigned __int64 epoch = 116444736000000000;
int gettimeofday(struct timeval *tp, struct timezone *tzp);
int gettimeofday(struct timeval *tp, struct timezone *tzp) {
    FILETIME       ft;
    SYSTEMTIME     st;
    ULARGE_INTEGER ul;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);
    ul.LowPart  = ft.dwLowDateTime;
    ul.HighPart = ft.dwHighDateTime;
    tp->tv_sec  = (ul.QuadPart - epoch) / 10000000L;
    tp->tv_usec = st.wMilliseconds * 1000;
    return 0;
}
#endif

UA_DateTime UA_DateTime_now() {
    UA_DateTime    dateTime;
    struct timeval tv;
    gettimeofday(&tv, UA_NULL);
    dateTime = (tv.tv_sec + UNIX_EPOCH_BIAS_SEC)
               * HUNDRED_NANOSEC_PER_SEC + tv.tv_usec * HUNDRED_NANOSEC_PER_USEC;
    return dateTime;
}

UA_DateTimeStruct UA_DateTime_toStruct(UA_DateTime atime) {
    UA_DateTimeStruct dateTimeStruct;
    //calcualting the the milli-, micro- and nanoseconds
    dateTimeStruct.nanoSec  = (UA_Int16)((atime % 10) * 100);
    dateTimeStruct.microSec = (UA_Int16)((atime % 10000) / 10);
    dateTimeStruct.milliSec = (UA_Int16)((atime % 10000000) / 10000);

    //calculating the unix time with #include <time.h>
    time_t secSinceUnixEpoch = (atime/10000000) - UNIX_EPOCH_BIAS_SEC;
    struct tm ts = *gmtime(&secSinceUnixEpoch);
    dateTimeStruct.sec    = (UA_Int16)ts.tm_sec;
    dateTimeStruct.min    = (UA_Int16)ts.tm_min;
    dateTimeStruct.hour   = (UA_Int16)ts.tm_hour;
    dateTimeStruct.day    = (UA_Int16)ts.tm_mday;
    dateTimeStruct.month  = (UA_Int16)(ts.tm_mon + 1);
    dateTimeStruct.year   = (UA_Int16)(ts.tm_year + 1900);
    return dateTimeStruct;
}

UA_StatusCode UA_DateTime_toString(UA_DateTime atime, UA_String *timeString) {
    // length of the string is 31 (incl. \0 at the end)
    if(!(timeString->data = UA_malloc(32)))
        return UA_STATUSCODE_BADOUTOFMEMORY;
    timeString->length = 31;

    UA_DateTimeStruct tSt = UA_DateTime_toStruct(atime);
    sprintf((char*)timeString->data, "%02d/%02d/%04d %02d:%02d:%02d.%03d.%03d.%03d", tSt.month, tSt.day, tSt.year,
            tSt.hour, tSt.min, tSt.sec, tSt.milliSec, tSt.microSec, tSt.nanoSec);
    return UA_STATUSCODE_GOOD;
}

/* Guid */
UA_TYPE_DELETEMEMBERS_NOACTION(UA_Guid)
UA_TYPE_DELETE_DEFAULT(UA_Guid)

UA_Boolean UA_Guid_equal(const UA_Guid *g1, const UA_Guid *g2) {
    if(memcmp(g1, g2, sizeof(UA_Guid)) == 0)
        return UA_TRUE;
    return UA_FALSE;
}

UA_Guid UA_Guid_random(UA_UInt32 *seed) {
    UA_Guid result;
    result.data1 = RAND(seed);
    UA_UInt32 r = RAND(seed);
    result.data2 = (UA_UInt16) r;
    result.data3 = (UA_UInt16) (r >> 16);
    r = RAND(seed);
    result.data4[0] = (UA_Byte)r;
    result.data4[1] = (UA_Byte)(r >> 4);
    result.data4[2] = (UA_Byte)(r >> 8);
    result.data4[3] = (UA_Byte)(r >> 12);
    r = RAND(seed);
    result.data4[4] = (UA_Byte)r;
    result.data4[5] = (UA_Byte)(r >> 4);
    result.data4[6] = (UA_Byte)(r >> 8);
    result.data4[7] = (UA_Byte)(r >> 12);
    return result;
}

void UA_Guid_init(UA_Guid *p) {
    p->data1 = 0;
    p->data2 = 0;
    p->data3 = 0;
    memset(p->data4, 0, sizeof(UA_Byte)*8);
}

UA_TYPE_NEW_DEFAULT(UA_Guid)
UA_StatusCode UA_Guid_copy(UA_Guid const *src, UA_Guid *dst) {
    UA_memcpy((void *)dst, (const void *)src, sizeof(UA_Guid));
    return UA_STATUSCODE_GOOD;
}

/* ByteString */
UA_StatusCode UA_ByteString_newMembers(UA_ByteString *p, UA_Int32 length) {
    if(length > 0) {
        if(!(p->data = UA_malloc((UA_UInt32)length)))
            return UA_STATUSCODE_BADOUTOFMEMORY;
        p->length = length;
    } else {
        p->data = UA_NULL;
        if(length == 0)
            p->length = 0;
        else
            p->length = -1;
    }
    return UA_STATUSCODE_GOOD;
}

/* XmlElement */

/* NodeId */
void UA_NodeId_init(UA_NodeId *p) {
    p->identifierType = UA_NODEIDTYPE_NUMERIC;
    p->namespaceIndex = 0;
    memset(&p->identifier, 0, sizeof(p->identifier));
}

UA_TYPE_NEW_DEFAULT(UA_NodeId)
UA_StatusCode UA_NodeId_copy(UA_NodeId const *src, UA_NodeId *dst) {
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    switch(src->identifierType) {
    case UA_NODEIDTYPE_NUMERIC:
        *dst = *src;
        return UA_STATUSCODE_GOOD;
    case UA_NODEIDTYPE_STRING: // Table 6, second entry
        retval |= UA_String_copy(&src->identifier.string, &dst->identifier.string);
        break;
    case UA_NODEIDTYPE_GUID: // Table 6, third entry
        retval |= UA_Guid_copy(&src->identifier.guid, &dst->identifier.guid);
        break;
    case UA_NODEIDTYPE_BYTESTRING: // Table 6, "OPAQUE"
        retval |= UA_ByteString_copy(&src->identifier.byteString, &dst->identifier.byteString);
        break;
    default:
        UA_NodeId_init(dst);
        return UA_STATUSCODE_BADINTERNALERROR;
    }
    dst->namespaceIndex = src->namespaceIndex;
    dst->identifierType = src->identifierType;
    if(retval) {
        UA_NodeId_deleteMembers(dst);
        UA_NodeId_init(dst);
    }
    return retval;
}

static UA_Boolean UA_NodeId_isBasicType(UA_NodeId const *id) {
    return id ->namespaceIndex == 0 && 1 <= id ->identifier.numeric &&
        id ->identifier.numeric <= 25;
}

UA_TYPE_DELETE_DEFAULT(UA_NodeId)
void UA_NodeId_deleteMembers(UA_NodeId *p) {
    switch(p->identifierType) {
    case UA_NODEIDTYPE_STRING:
    case UA_NODEIDTYPE_BYTESTRING:
        UA_ByteString_deleteMembers(&p->identifier.byteString);
        break;
    default:
        break;
    }
}

UA_Boolean UA_NodeId_equal(const UA_NodeId *n1, const UA_NodeId *n2) {
	if(n1->namespaceIndex != n2->namespaceIndex || n1->identifierType!=n2->identifierType)
        return UA_FALSE;

    switch(n1->identifierType) {
    case UA_NODEIDTYPE_NUMERIC:
        if(n1->identifier.numeric == n2->identifier.numeric)
            return UA_TRUE;
        else
            return UA_FALSE;

    case UA_NODEIDTYPE_STRING:
        return UA_String_equal(&n1->identifier.string, &n2->identifier.string);

    case UA_NODEIDTYPE_GUID:
        return UA_Guid_equal(&n1->identifier.guid, &n2->identifier.guid);

    case UA_NODEIDTYPE_BYTESTRING:
        return UA_ByteString_equal(&n1->identifier.byteString, &n2->identifier.byteString);
    }
    return UA_FALSE;
}

UA_Boolean UA_NodeId_isNull(const UA_NodeId *p) {
    switch(p->identifierType) {
    case UA_NODEIDTYPE_NUMERIC:
        if(p->namespaceIndex != 0 || p->identifier.numeric != 0)
            return UA_FALSE;
        break;

    case UA_NODEIDTYPE_STRING:
        if(p->namespaceIndex != 0 || p->identifier.string.length > 0)
            return UA_FALSE;
        break;

    case UA_NODEIDTYPE_GUID:
        if(p->namespaceIndex != 0 ||
           memcmp(&p->identifier.guid, (char[sizeof(UA_Guid)]) { 0 }, sizeof(UA_Guid)) != 0)
            return UA_FALSE;
        break;

    case UA_NODEIDTYPE_BYTESTRING:
        if(p->namespaceIndex != 0 || p->identifier.byteString.length > 0)
            return UA_FALSE;
        break;

    default:
        return UA_FALSE;
    }
    return UA_TRUE;
}

/* ExpandedNodeId */
UA_TYPE_DELETE_DEFAULT(UA_ExpandedNodeId)
void UA_ExpandedNodeId_deleteMembers(UA_ExpandedNodeId *p) {
    UA_NodeId_deleteMembers(&p->nodeId);
    UA_String_deleteMembers(&p->namespaceUri);
}

void UA_ExpandedNodeId_init(UA_ExpandedNodeId *p) {
    UA_NodeId_init(&p->nodeId);
    UA_String_init(&p->namespaceUri);
    p->serverIndex = 0;
}

UA_TYPE_NEW_DEFAULT(UA_ExpandedNodeId)
UA_StatusCode UA_ExpandedNodeId_copy(UA_ExpandedNodeId const *src, UA_ExpandedNodeId *dst) {
    UA_StatusCode retval = UA_NodeId_copy(&src->nodeId, &dst->nodeId);
    retval |= UA_String_copy(&src->namespaceUri, &dst->namespaceUri);
    dst->serverIndex = src->serverIndex;
    if(retval) {
        UA_ExpandedNodeId_deleteMembers(dst);
        UA_ExpandedNodeId_init(dst);
    }
    return retval;
}

UA_Boolean UA_ExpandedNodeId_isNull(const UA_ExpandedNodeId *p) {
    return UA_NodeId_isNull(&p->nodeId);
}

/* StatusCode */

/* QualifiedName */
UA_TYPE_DELETE_DEFAULT(UA_QualifiedName)
void UA_QualifiedName_deleteMembers(UA_QualifiedName *p) {
    UA_String_deleteMembers(&p->name);
}

void UA_QualifiedName_init(UA_QualifiedName *p) {
    UA_String_init(&p->name);
    p->namespaceIndex = 0;
}

UA_TYPE_NEW_DEFAULT(UA_QualifiedName)
UA_StatusCode UA_QualifiedName_copy(UA_QualifiedName const *src, UA_QualifiedName *dst) {
    UA_StatusCode retval = UA_String_copy(&src->name, &dst->name);
    dst->namespaceIndex = src->namespaceIndex;
    if(retval) {
        UA_QualifiedName_deleteMembers(dst);
        UA_QualifiedName_init(dst);
    }
    return retval;

}

UA_StatusCode UA_QualifiedName_copycstring(char const *src, UA_QualifiedName *dst) {
    dst->namespaceIndex = 0;
    return UA_String_copycstring(src, &dst->name);
}

/* LocalizedText */
UA_TYPE_DELETE_DEFAULT(UA_LocalizedText)
void UA_LocalizedText_deleteMembers(UA_LocalizedText *p) {
    UA_String_deleteMembers(&p->locale);
    UA_String_deleteMembers(&p->text);
}

void UA_LocalizedText_init(UA_LocalizedText *p) {
    UA_String_init(&p->locale);
    UA_String_init(&p->text);
}

UA_TYPE_NEW_DEFAULT(UA_LocalizedText)
UA_StatusCode UA_LocalizedText_copycstring(char const *src, UA_LocalizedText *dst) {
    UA_StatusCode retval = UA_String_copycstring("", &dst->locale);
    retval |= UA_String_copycstring(src, &dst->text);
    if(retval) {
        UA_LocalizedText_deleteMembers(dst);
        UA_LocalizedText_init(dst);
    }
    return retval;
}

UA_StatusCode UA_LocalizedText_copy(UA_LocalizedText const *src, UA_LocalizedText *dst) {
    UA_StatusCode retval = UA_String_copy(&src->locale, &dst->locale);
    retval |= UA_String_copy(&src->text, &dst->text);
    if(retval) {
        UA_LocalizedText_deleteMembers(dst);
        UA_LocalizedText_init(dst);
    }
    return retval;
}

/* ExtensionObject */
UA_TYPE_DELETE_DEFAULT(UA_ExtensionObject)
void UA_ExtensionObject_deleteMembers(UA_ExtensionObject *p) {
    UA_NodeId_deleteMembers(&p->typeId);
    UA_ByteString_deleteMembers(&p->body);
}

void UA_ExtensionObject_init(UA_ExtensionObject *p) {
    UA_NodeId_init(&p->typeId);
    p->encoding = UA_EXTENSIONOBJECT_ENCODINGMASK_NOBODYISENCODED;
    UA_ByteString_init(&p->body);
}

UA_TYPE_NEW_DEFAULT(UA_ExtensionObject)
UA_StatusCode UA_ExtensionObject_copy(UA_ExtensionObject const *src, UA_ExtensionObject *dst) {
    UA_StatusCode retval = UA_ByteString_copy(&src->body, &dst->body);
    retval |= UA_NodeId_copy(&src->typeId, &dst->typeId);
    dst->encoding = src->encoding;
    if(retval) {
        UA_ExtensionObject_deleteMembers(dst);
        UA_ExtensionObject_init(dst);
    }
    return retval;
}

/* DataValue */
UA_TYPE_DELETE_DEFAULT(UA_DataValue)
void UA_DataValue_deleteMembers(UA_DataValue *p) {
    UA_Variant_deleteMembers(&p->value);
}

void UA_DataValue_init(UA_DataValue *p) {
    *((UA_Byte*)p) = 0; // zero out the bitfield
    p->serverPicoseconds = 0;
    UA_DateTime_init(&p->serverTimestamp);
    p->sourcePicoseconds = 0;
    UA_DateTime_init(&p->sourceTimestamp);
    UA_StatusCode_init(&p->status);
    UA_Variant_init(&p->value);
}

UA_TYPE_NEW_DEFAULT(UA_DataValue)
UA_StatusCode UA_DataValue_copy(UA_DataValue const *src, UA_DataValue *dst) {
    UA_DataValue_init(dst);
    *((UA_Byte*)dst) = *((const UA_Byte*)src); // the bitfield
    UA_StatusCode retval = UA_DateTime_copy(&src->serverTimestamp, &dst->serverTimestamp);
    retval |= UA_DateTime_copy(&src->sourceTimestamp, &dst->sourceTimestamp);
    retval |= UA_Variant_copy(&src->value, &dst->value);
    dst->serverPicoseconds = src->serverPicoseconds;
    dst->sourcePicoseconds = src->sourcePicoseconds;
    dst->status = src->status;
    if(retval) {
        UA_DataValue_deleteMembers(dst);
        UA_DataValue_init(dst);
    }
    return retval;
}

/* Variant */
UA_TYPE_NEW_DEFAULT(UA_Variant)
void UA_Variant_init(UA_Variant *p) {
    p->storageType = UA_VARIANT_DATA;
    p->arrayLength = -1;
    p->dataPtr = UA_NULL;
    p->arrayDimensions = UA_NULL;
    p->arrayDimensionsSize = -1;
    p->type = &UA_TYPES[UA_TYPES_BOOLEAN];
}

UA_TYPE_DELETE_DEFAULT(UA_Variant)
void UA_Variant_deleteMembers(UA_Variant *p) {
    if(p->storageType == UA_VARIANT_DATA) {
        if(p->dataPtr) {
            if(p->arrayLength == -1)
                p->arrayLength = 1;
            UA_Array_delete(p->dataPtr, p->type, p->arrayLength);
            p->dataPtr = UA_NULL;
            p->arrayLength = -1;
        }
        if(p->arrayDimensions) {
            UA_free(p->arrayDimensions);
            p->arrayDimensions = UA_NULL;
        }
    }
}

UA_StatusCode UA_Variant_copy(UA_Variant const *src, UA_Variant *dst) {
    UA_Variant_init(dst);
    UA_Int32 tmp = src->arrayLength;
    if(src->arrayLength == -1 && src->dataPtr)
        tmp = 1;
    UA_StatusCode retval = UA_Array_copy(src->dataPtr, &dst->dataPtr, src->type, tmp);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Variant_deleteMembers(dst);
        UA_Variant_init(dst);
        return retval;
    }
    dst->arrayLength = src->arrayLength;
    dst->type = src->type;
    dst->storageType = UA_VARIANT_DATA;

    if(src->arrayDimensions) {
        retval |= UA_Array_copy(src->arrayDimensions, (void **)&dst->arrayDimensions,
                                &UA_TYPES[UA_TYPES_INT32], src->arrayDimensionsSize);
        if(retval != UA_STATUSCODE_GOOD) {
            UA_Variant_deleteMembers(dst);
            UA_Variant_init(dst);
            return retval;
        }
        dst->arrayDimensionsSize = src->arrayDimensionsSize;
    }

    return retval;
}

UA_StatusCode UA_Variant_copyRange(const UA_Variant *src, UA_Variant *dst, const UA_NumericRange range) {
    UA_Variant_init(dst);
    // test the integrity of the variant dimensions
    UA_Int32 dims_count = 1;
    const UA_Int32 *dims = &src->arrayLength; // default: the array has only one dimension
    if(src->arrayDimensionsSize > 0) {
        dims_count = src->arrayDimensionsSize;
        dims = src->arrayDimensions;
        UA_Int32 elements = 1;
        for(UA_Int32 i = 0; i < dims_count; i++)
            elements *= dims[i];
        if(elements != src->arrayLength)
            return UA_STATUSCODE_BADINTERNALERROR;
    }

    // test the integrity of the range and count objects
    size_t count = 1;
    if(range.dimensionsSize != dims_count)
        return UA_STATUSCODE_BADINTERNALERROR;
    for(UA_Int32 i = 0; i < range.dimensionsSize; i++) {
        if(range.dimensions[i].min > range.dimensions[i].max)
            return UA_STATUSCODE_BADINDEXRANGEINVALID;
        if(range.dimensions[i].max > (UA_UInt32)(dims[i]))
            return UA_STATUSCODE_BADINDEXRANGENODATA;
        count *= (range.dimensions[i].max - range.dimensions[i].min) + 1;
    }

    dst->dataPtr = UA_malloc(src->type->memSize * count);
    if(!dst->dataPtr)
        return UA_STATUSCODE_BADOUTOFMEMORY;
    
    // copy a subset of the tensor with as few calls as possible.
    // shift from copying single elements to contiguous blocks
    size_t elem_size = src->type->memSize;
    uintptr_t nextsrc = (uintptr_t)src->dataPtr; // the start ptr of the next copy operation
    size_t contiguous_elems = src->arrayLength; // the length of the copy operation
    ptrdiff_t jump_length = elem_size * dims[0]; // how far to jump until the next contiguous copy
    size_t copy_count = count; // how often to copy

    size_t running_dimssize = 1; // how big is a contiguous block for the dimensions k_max to k
    UA_Boolean found_contiguous = UA_FALSE;
    for(UA_Int32 k = dims_count - 1; k >= 0; k--) {
        if(!found_contiguous) {
            if(range.dimensions[k].min != 0 || range.dimensions[k].max + 1 != (UA_UInt32)dims[k]) {
                found_contiguous = UA_TRUE;
                contiguous_elems = (range.dimensions[k].max - range.dimensions[k].min + 1) * running_dimssize;
                jump_length = ((dims[k] * running_dimssize) - contiguous_elems) * elem_size;
                copy_count /= range.dimensions[k].max - range.dimensions[k].min + 1;
            } else
                copy_count /= dims[k];
        } 
        nextsrc += running_dimssize * range.dimensions[k].min * elem_size;
        running_dimssize *= dims[k];
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    uintptr_t nextdst = (uintptr_t)dst->dataPtr;
    size_t copied = 0;
    for(size_t i = 0; i < copy_count; i++) {
        if(src->type->fixedSize) {
            memcpy((void*)nextdst, (void*)nextsrc, elem_size * contiguous_elems);
        } else {
            for(size_t j = 0; j < contiguous_elems; j++)
                retval = UA_copy((const void*)(nextsrc + (j * elem_size)), (void*)(nextdst + (j * elem_size)),
                                 src->type);
            if(retval != UA_STATUSCODE_GOOD) {
                UA_Array_delete(dst->dataPtr, src->type, copied);
                return retval;
            }
            copied += contiguous_elems;
        }
        nextdst += elem_size * contiguous_elems;
        nextsrc += jump_length;
    }

    if(src->arrayDimensionsSize > 0) {
        retval = UA_Array_copy(dims, (void**)&dst->arrayDimensions, &UA_TYPES[UA_TYPES_INT32], dims_count);
        if(retval != UA_STATUSCODE_GOOD) {
            UA_Array_delete(dst->dataPtr, src->type, copied);
            return retval;
        }
        for(UA_Int32 k = 0; k < dims_count; k++)
            dst->arrayDimensions[k] = range.dimensions[k].max - range.dimensions[k].min + 1;
        dst->arrayDimensionsSize = dims_count;
    }
    dst->arrayLength = count;
    dst->type = src->type;
    dst->storageType = UA_VARIANT_DATA;
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode UA_Variant_setScalar(UA_Variant *v, void *p, const UA_DataType *type) {
    return UA_Variant_setArray(v, p, -1, type);
}

UA_StatusCode UA_Variant_setScalarCopy(UA_Variant *v, const void *p, const UA_DataType *type) {
    void *new = UA_malloc(type->memSize);
    if(!new)
        return UA_STATUSCODE_BADOUTOFMEMORY;
    UA_StatusCode retval = UA_copy(p, new, type);
	if(retval != UA_STATUSCODE_GOOD) {
		UA_delete(new, type);
		return retval;
	}
    return UA_Variant_setArray(v, new, -1, type);
}

UA_StatusCode UA_Variant_setArray(UA_Variant *v, void *array, UA_Int32 noElements,
                                  const UA_DataType *type) {
    v->type = type;
    v->arrayLength = noElements;
    v->dataPtr = array;
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode UA_Variant_setArrayCopy(UA_Variant *v, const void *array, UA_Int32 noElements,
                                      const UA_DataType *type) {
    void *new;
    UA_StatusCode retval = UA_Array_copy(array, &new, type, noElements);
    if(retval != UA_STATUSCODE_GOOD)
        return retval;
    return UA_Variant_setArray(v, new, noElements, type);
}

/* DiagnosticInfo */
UA_TYPE_DELETE_DEFAULT(UA_DiagnosticInfo)
void UA_DiagnosticInfo_deleteMembers(UA_DiagnosticInfo *p) {
    UA_String_deleteMembers(&p->additionalInfo);
    if(p->hasInnerDiagnosticInfo && p->innerDiagnosticInfo) {
        UA_DiagnosticInfo_delete(p->innerDiagnosticInfo);
        p->innerDiagnosticInfo = UA_NULL;
    }
}

UA_TYPE_NEW_DEFAULT(UA_DiagnosticInfo)
void UA_DiagnosticInfo_init(UA_DiagnosticInfo *p) {
	*((UA_Byte*)p) = 0; // zero out the bitfield
    p->symbolicId          = 0;
    p->namespaceUri        = 0;
    p->localizedText       = 0;
    p->locale              = 0;
    UA_String_init(&p->additionalInfo);
    p->innerDiagnosticInfo = UA_NULL;
    UA_StatusCode_init(&p->innerStatusCode);
}

UA_StatusCode UA_DiagnosticInfo_copy(UA_DiagnosticInfo const *src, UA_DiagnosticInfo *dst) {
    UA_DiagnosticInfo_init(dst);
    *((UA_Byte*)dst) = *((const UA_Byte*)src); // the bitfield
    
    dst->symbolicId = src->symbolicId;
    dst->namespaceUri = src->namespaceUri;
    dst->localizedText = src->localizedText;
    dst->locale = src->locale;
    dst->innerStatusCode = src->innerStatusCode;
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    if(src->hasAdditionalInfo)
       retval = UA_String_copy(&src->additionalInfo, &dst->additionalInfo);
    if(src->hasInnerDiagnosticInfo && src->innerDiagnosticInfo) {
        if((dst->innerDiagnosticInfo = UA_malloc(sizeof(UA_DiagnosticInfo)))) {
            retval |= UA_DiagnosticInfo_copy(src->innerDiagnosticInfo, dst->innerDiagnosticInfo);
            dst->hasInnerDiagnosticInfo = src->hasInnerDiagnosticInfo;
        }
        else
            retval |= UA_STATUSCODE_BADOUTOFMEMORY;
    }
    if(retval) {
        UA_DiagnosticInfo_deleteMembers(dst);
        UA_DiagnosticInfo_init(dst);
    }
    return retval;
}

/*******************/
/* Structure Types */
/*******************/

void UA_init(void *p, const UA_DataType *dataType) {
    /* Do not check if the index is a builtin-type here. Builtins will be called
       with their very own _init functions normally. In the off case, that the
       generic function is called with the index of a builtin, their layout
       contains a single member of the builtin type, that will be inited in the
       for loop. */

    uintptr_t ptr = (uintptr_t)p;
    UA_Byte membersSize = dataType->membersSize;
    for(size_t i=0;i<membersSize; i++) {
        const UA_DataTypeMember *member = &dataType->members[i];
        if(member->isArray) {
            /* Padding contains bit-magic to split into padding before and after
               the length integer */
            ptr += (member->padding >> 3);
            *((UA_Int32*)ptr) = -1;
            ptr += sizeof(UA_Int32) + (member->padding & 0x07);
            *((void**)ptr) = UA_NULL;
            ptr += sizeof(void*);
            continue;
        }

        ptr += member->padding;
        if(!member->namespaceZero) {
            // pointer arithmetic
            const UA_DataType *memberType = dataType - dataType->typeIndex + member->memberTypeIndex;
            UA_init((void*)ptr, memberType);
            ptr += memberType->memSize;
            continue;
        }

        switch(member->memberTypeIndex) {
        case UA_TYPES_BOOLEAN:
        case UA_TYPES_SBYTE:
        case UA_TYPES_BYTE:
            *(UA_Byte*)ptr = 0;
            break;
        case UA_TYPES_INT16:
        case UA_TYPES_UINT16:
            *(UA_Int16*)ptr = 0;
            break;
        case UA_TYPES_INT32:
        case UA_TYPES_UINT32:
        case UA_TYPES_STATUSCODE:
        case UA_TYPES_FLOAT:
            *(UA_Int32*)ptr = 0;
            break;
        case UA_TYPES_INT64:
        case UA_TYPES_UINT64:
        case UA_TYPES_DOUBLE:
        case UA_TYPES_DATETIME:
            *(UA_Int64*)ptr = 0;
            break;
        case UA_TYPES_GUID:
            UA_Guid_init((UA_Guid*)ptr);
            break;
        case UA_TYPES_NODEID:
            UA_NodeId_init((UA_NodeId*)ptr);
            break;
        case UA_TYPES_EXPANDEDNODEID:
            UA_ExpandedNodeId_init((UA_ExpandedNodeId*)ptr);
            break;
        case UA_TYPES_LOCALIZEDTEXT:
            UA_LocalizedText_init((UA_LocalizedText*)ptr);
            break;
        case UA_TYPES_EXTENSIONOBJECT:
            UA_ExtensionObject_init((UA_ExtensionObject*)ptr);
            break;
        case UA_TYPES_DATAVALUE:
            UA_DataValue_init((UA_DataValue*)ptr);
            break;
        case UA_TYPES_VARIANT:
            UA_Variant_init((UA_Variant*)ptr);
            break;
        case UA_TYPES_DIAGNOSTICINFO:
            UA_DiagnosticInfo_init((UA_DiagnosticInfo*)ptr);
            break;
        default:
            // QualifiedName, LocalizedText and strings are treated as structures, also
            UA_init((void*)ptr, &UA_TYPES[member->memberTypeIndex]);
        }
        ptr += UA_TYPES[member->memberTypeIndex].memSize;
    }
}

void * UA_new(const UA_DataType *dataType) {
    void *p = UA_malloc(dataType->memSize);
    if(p)
        UA_init(p, dataType);
    return p;
}

UA_StatusCode UA_copy(const void *src, void *dst, const UA_DataType *dataType) {
    if(dataType->fixedSize) {
        memcpy(dst, src, dataType->memSize);
        return UA_STATUSCODE_GOOD;
    }
    UA_init(dst, dataType);
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    uintptr_t ptrs = (uintptr_t)src;
    uintptr_t ptrd = (uintptr_t)dst;
    UA_Byte membersSize = dataType->membersSize;
    for(size_t i=0;i<membersSize; i++) {
        const UA_DataTypeMember *member = &dataType->members[i];
        const UA_DataType *memberType;
        if(member->namespaceZero)
            memberType = &UA_TYPES[member->memberTypeIndex];
        else
            memberType = dataType - dataType->typeIndex + member->memberTypeIndex;

        if(member->isArray) {
            ptrs += (member->padding >> 3);
            ptrd += (member->padding >> 3);
            UA_Int32 *dstNoElements = (UA_Int32*)ptrd;
            const UA_Int32 noElements = *((const UA_Int32*)ptrs);
            ptrs += sizeof(UA_Int32) + (member->padding & 0x07);
            ptrd += sizeof(UA_Int32) + (member->padding & 0x07);
            retval = UA_Array_copy(*(void* const*)ptrs, (void**)ptrd, memberType, noElements);
            if(retval != UA_STATUSCODE_GOOD) {
                UA_deleteMembers(dst, dataType);
                UA_init(dst, dataType);
                return retval;
            }
            *dstNoElements = noElements;
            ptrs += sizeof(void*);
            ptrd += sizeof(void*);
            continue;
        }

        ptrs += member->padding;
        ptrd += member->padding;
        if(!member->namespaceZero) {
            retval = UA_copy((const void*)ptrs, (void*)ptrd, memberType);
            if(retval != UA_STATUSCODE_GOOD) {
                UA_deleteMembers(dst, dataType);
                UA_init(dst, dataType);
                return retval;
            }
            ptrs += memberType->memSize;
            ptrd += memberType->memSize;
            continue;
        }

        switch(member->memberTypeIndex) {
        case UA_TYPES_BOOLEAN:
        case UA_TYPES_SBYTE:
        case UA_TYPES_BYTE:
            *((UA_Byte*)ptrd) = *((const UA_Byte*)ptrs);
            break;
        case UA_TYPES_INT16:
        case UA_TYPES_UINT16:
            *((UA_Int16*)ptrd) = *((const UA_Byte*)ptrs);
            break;
        case UA_TYPES_INT32:
        case UA_TYPES_UINT32:
        case UA_TYPES_STATUSCODE:
        case UA_TYPES_FLOAT:
            *((UA_Int32*)ptrd) = *((const UA_Int32*)ptrs);
            break;
        case UA_TYPES_INT64:
        case UA_TYPES_UINT64:
        case UA_TYPES_DOUBLE:
        case UA_TYPES_DATETIME:
            *((UA_Int64*)ptrd) = *((const UA_Int64*)ptrs);
            break;
        case UA_TYPES_GUID:
            *((UA_Guid*)ptrd) = *((const UA_Guid*)ptrs);
            break;
        case UA_TYPES_NODEID:
            retval |= UA_NodeId_copy((const UA_NodeId*)ptrs, (UA_NodeId*)ptrd);
            break;
        case UA_TYPES_EXPANDEDNODEID:
            retval |= UA_ExpandedNodeId_copy((const UA_ExpandedNodeId*)ptrs, (UA_ExpandedNodeId*)ptrd);
            break;
        case UA_TYPES_LOCALIZEDTEXT:
            retval |= UA_LocalizedText_copy((const UA_LocalizedText*)ptrs, (UA_LocalizedText*)ptrd);
            break;
        case UA_TYPES_EXTENSIONOBJECT:
            retval |= UA_ExtensionObject_copy((const UA_ExtensionObject*)ptrs, (UA_ExtensionObject*)ptrd);
            break;
        case UA_TYPES_DATAVALUE:
            retval |= UA_DataValue_copy((const UA_DataValue*)ptrs, (UA_DataValue*)ptrd);
            break;
        case UA_TYPES_VARIANT:
            retval |= UA_Variant_copy((const UA_Variant*)ptrs, (UA_Variant*)ptrd);
            break;
        case UA_TYPES_DIAGNOSTICINFO:
            retval |= UA_DiagnosticInfo_copy((const UA_DiagnosticInfo*)ptrs, (UA_DiagnosticInfo*)ptrd);
            break;
        default:
            // QualifiedName, LocalizedText and strings are treated as structures, also
            retval |= UA_copy((const void *)ptrs, (void*)ptrd, memberType);
        }
        ptrs += memberType->memSize;
        ptrd += memberType->memSize;
    }
    if(retval != UA_STATUSCODE_GOOD) {
        UA_deleteMembers(dst, dataType);
        UA_init(dst, dataType);
    }
    return retval;
}

void UA_deleteMembers(void *p, const UA_DataType *dataType) {
    uintptr_t ptr = (uintptr_t)p;
    if(dataType->fixedSize)
        return;
    UA_Byte membersSize = dataType->membersSize;
    for(size_t i=0;i<membersSize; i++) {
        const UA_DataTypeMember *member = &dataType->members[i];
        const UA_DataType *memberType;
        if(member->namespaceZero)
            memberType = &UA_TYPES[member->memberTypeIndex];
        else
            memberType = dataType - dataType->typeIndex + member->memberTypeIndex;

        if(member->isArray) {
            ptr += (member->padding >> 3);
            UA_Int32 noElements = *((UA_Int32*)ptr);
            ptr += sizeof(UA_Int32) + (member->padding & 0x07);
            UA_Array_delete(*(void**)ptr, memberType, noElements);
            *(void**)ptr = UA_NULL;
            ptr += sizeof(void*);
            continue;
        }

        ptr += member->padding;
        if(!member->namespaceZero) {
            UA_deleteMembers((void*)ptr, memberType);
            ptr += memberType->memSize;
            continue;
        }

        switch(member->memberTypeIndex) {
        case UA_TYPES_BOOLEAN:
        case UA_TYPES_SBYTE:
        case UA_TYPES_BYTE:
        case UA_TYPES_INT16:
        case UA_TYPES_UINT16:
        case UA_TYPES_INT32:
        case UA_TYPES_UINT32:
        case UA_TYPES_STATUSCODE:
        case UA_TYPES_FLOAT:
        case UA_TYPES_INT64:
        case UA_TYPES_UINT64:
        case UA_TYPES_DOUBLE:
        case UA_TYPES_DATETIME:
        case UA_TYPES_GUID:
            break;
        case UA_TYPES_NODEID:
            UA_NodeId_deleteMembers((UA_NodeId*)ptr);
            break;
        case UA_TYPES_EXPANDEDNODEID:
            UA_ExpandedNodeId_deleteMembers((UA_ExpandedNodeId*)ptr);
            break;
        case UA_TYPES_LOCALIZEDTEXT:
            UA_LocalizedText_deleteMembers((UA_LocalizedText*)ptr);
            break;
        case UA_TYPES_EXTENSIONOBJECT:
            UA_ExtensionObject_deleteMembers((UA_ExtensionObject*)ptr);
            break;
        case UA_TYPES_DATAVALUE:
            UA_DataValue_deleteMembers((UA_DataValue*)ptr);
            break;
        case UA_TYPES_VARIANT:
            UA_Variant_deleteMembers((UA_Variant*)ptr);
            break;
        case UA_TYPES_DIAGNOSTICINFO:
            UA_DiagnosticInfo_deleteMembers((UA_DiagnosticInfo*)ptr);
            break;
        default:
            // QualifiedName, LocalizedText and strings are treated as structures, also
            UA_deleteMembers((void*)ptr, memberType);
        }
        ptr += memberType->memSize;
    }
}

void UA_delete(void *p, const UA_DataType *dataType) {
    UA_deleteMembers(p, dataType);
    UA_free(p);
}

/******************/
/* Array Handling */
/******************/

void* UA_Array_new(const UA_DataType *dataType, UA_Int32 noElements) {
    if(noElements <= 0)
        return UA_NULL;

    if((UA_Int32)dataType->memSize * noElements < 0 || dataType->memSize * noElements > MAX_ARRAY_SIZE )
        return UA_NULL;

    if(dataType->fixedSize)
        return calloc(noElements, dataType->memSize);

    void *p = malloc(dataType->memSize * (size_t)noElements);
    if(!p)
        return p;

    uintptr_t ptr = (uintptr_t)p;
    for(int i = 0; i<noElements; i++) {
        UA_init((void*)ptr, dataType);
        ptr += dataType->memSize;
    }
    return p;
}

UA_StatusCode UA_Array_copy(const void *src, void **dst, const UA_DataType *dataType, UA_Int32 noElements) {
    if(noElements <= 0) {
        *dst = UA_NULL;
        return UA_STATUSCODE_GOOD;
    }

    if(!(*dst = UA_malloc((size_t)noElements * dataType->memSize)))
        return UA_STATUSCODE_BADOUTOFMEMORY;

    if(dataType->fixedSize) {
        memcpy(*dst, src, dataType->memSize * (size_t)noElements);
        return UA_STATUSCODE_GOOD;
    }

    uintptr_t ptrs = (uintptr_t)src;
    uintptr_t ptrd = (uintptr_t)*dst;
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    for(int i=0;i<noElements; i++) {
        retval = UA_copy((void*)ptrs, (void*)ptrd, dataType);
        ptrs += dataType->memSize;
        ptrd += dataType->memSize;
    }

    if(retval != UA_STATUSCODE_GOOD)
        UA_Array_delete(*dst, dataType, noElements);
        
    return retval;
}

void UA_Array_delete(void *p, const UA_DataType *dataType, UA_Int32 noElements) {
    if(!dataType->fixedSize) {
        uintptr_t ptr = (uintptr_t)p;
        for(UA_Int32 i = 0; i<noElements; i++) {
            UA_deleteMembers((void*)ptr, dataType);
            ptr += dataType->memSize;
        }
    }
    UA_free(p);
}
