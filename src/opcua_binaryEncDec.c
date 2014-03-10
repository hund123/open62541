/*
 * opcua_binaryEncDec.c
 *
 *  Created on: Dec 18, 2013
 *      Author: opcua
 */

#include "opcua_binaryEncDec.h"
#include "opcua_types.h"
#include <stdio.h>
#include <stdlib.h>

#include "opcua_builtInDatatypes.h"
#include "opcua_advancedDatatypes.h"

Int32 encoder_encodeBuiltInDatatype(void *data, Int32 type, Int32 *pos,
		char *dstBuf) {
	switch (type) {
	case BOOLEAN:
		encodeBoolean((*(Boolean*) data), pos, dstBuf);
		break;
	case SBYTE:
		encodeSByte((*(Byte*) data), pos, dstBuf);
		break;
	case BYTE:
		encodeByte((*(Byte*) data), pos, dstBuf);
		break;
	case INT16:
		encodeInt16((*(Int16*) data), pos, dstBuf);
		break;
	case UINT16:
		encodeUInt16((*(UInt16*) data), pos, dstBuf);
		break;
	case INT32:
		encodeInt32((*(Int32*) data), pos, dstBuf);
		break;
	case UINT32:
		encodeUInt32(*(UInt32*) (data), pos, dstBuf);
		break;
	case INT64:
		encodeInt64((*(Int64*) data), pos, dstBuf);
		break;
	case UINT64:
		encodeUInt64((*(UInt64*) data), pos, dstBuf);
		break;
	case FLOAT:
		encodeFloat((*(Float*) data), pos, dstBuf);
		break;
	case DOUBLE:
		encodeDouble((*(Double*) data), pos, dstBuf);
		break;
	case STRING:
		encodeUAString(((UA_String*) data), pos, dstBuf);
		break;
	case DATE_TIME:
		encodeUADateTime((*(UA_DateTime*) data), pos, dstBuf);
		break;
	case GUID:
		encodeUAGuid(((UA_Guid*) data), pos, dstBuf);
		break;
	case BYTE_STRING:
		encodeUAByteString(((UA_ByteString*) data), pos, dstBuf);
		break;
	case XML_ELEMENT:
		encodeXmlElement((UA_XmlElement*) data, pos, dstBuf);
		break;
	case NODE_ID:
		encodeUANodeId((UA_NodeId*) data, pos, dstBuf);
		break;
	case EXPANDED_NODE_ID:
		encodeExpandedNodeId((UA_ExpandedNodeId*) data, pos, dstBuf);
		break;
	case STATUS_CODE:
		encodeUInt32(*((UInt32*) data), pos, dstBuf);
		break;
	case QUALIFIED_NAME:
		encodeQualifiedName(((UA_QualifiedName*) data), pos, dstBuf);
		break;
	case LOCALIZED_TEXT:
		encodeLocalizedText(((UA_LocalizedText*) data), pos, dstBuf);
		break;
	case EXTENSION_OBJECT:
		encodeExtensionObject((UA_ExtensionObject*) data, pos, dstBuf);
		break;
	case DATA_VALUE:
		encodeDataValue((UA_DataValue*) data, pos, dstBuf);
		break;
	case VARIANT:
		encodeVariant((UA_Variant*) data, pos, dstBuf);
		break;
	case DIAGNOSTIC_INFO:
		encodeDiagnosticInfo((UA_DiagnosticInfo*) data, pos, dstBuf);
		break;
	}
	return UA_NO_ERROR;
}

Int32 decoder_decodeBuiltInDatatype(char const * srcBuf, Int32 type, Int32 *pos,
		void *dstStructure) {
	Boolean tmp;

	switch (type) {
	case BOOLEAN:

		decodeBoolean(srcBuf, pos, (Boolean*) dstStructure);

		break;
	case SBYTE:
		decodeSByte(srcBuf, pos, (SByte*) dstStructure);
		break;
	case BYTE:
		decodeByte(srcBuf, pos, (Byte*) dstStructure);
		break;
	case INT16:
		decodeInt16(srcBuf, pos, (Int16*) dstStructure);
		break;
	case UINT16:
		decodeUInt16(srcBuf, pos, (UInt16*) dstStructure);
		break;
	case INT32:
		decodeInt32(srcBuf, pos, (Int32*) dstStructure);
		break;
	case UINT32:
		decodeUInt32(srcBuf, pos, (UInt32*) dstStructure);
		break;
	case INT64:
		decodeInt64(srcBuf, pos, (Int64*) dstStructure);
		break;
	case UINT64:
		decodeUInt64(srcBuf, pos, (UInt64*) dstStructure);
		break;
	case FLOAT:
		decodeFloat(srcBuf, pos, (Float*) dstStructure);
		break;
	case DOUBLE:
		decodeDouble(srcBuf, pos, (Double*) dstStructure);
		break;
	case STRING:
		decodeUAByteString(srcBuf, pos, (UA_String*) dstStructure);
		break;
	case DATE_TIME:
		decodeUADateTime(srcBuf, pos, (UA_DateTime*) dstStructure);
		break;
	case GUID:
		decodeUAGuid(srcBuf, pos, (UA_Guid*) dstStructure);
		break;
	case BYTE_STRING:
		decodeUAByteString(srcBuf, pos, (UA_ByteString*) dstStructure);
		break;
	case XML_ELEMENT:
		decodeXmlElement(srcBuf, pos, (UA_XmlElement*) dstStructure);
		break;
	case NODE_ID:
		decodeUANodeId(srcBuf, pos, (UA_NodeId*) dstStructure);
		break;
	case EXPANDED_NODE_ID:
		decodeExpandedNodeId(srcBuf, pos, (UA_ExpandedNodeId*) dstStructure);
		break;
	case STATUS_CODE:
		decodeUAStatusCode(srcBuf, pos, (UA_StatusCode*) dstStructure);
		break;
	case QUALIFIED_NAME:
		decodeQualifiedName(srcBuf, pos, (UA_QualifiedName*) dstStructure);
		break;
	case LOCALIZED_TEXT:
		decodeLocalizedText(srcBuf, pos, (UA_LocalizedText*) dstStructure);
		break;
	case EXTENSION_OBJECT:
		decodeExtensionObject(srcBuf, pos, (UA_ExtensionObject*) dstStructure);
		break;
	case DATA_VALUE:
		decodeDataValue(srcBuf, pos, (UA_DataValue*) dstStructure);
		break;
	case VARIANT:
		decodeVariant(srcBuf, pos, (UA_Variant*) dstStructure);
		break;
	case DIAGNOSTIC_INFO:
		decodeDiagnosticInfo(srcBuf, pos, (UA_DiagnosticInfo*) dstStructure);
		break;
	}
	return UA_NO_ERROR;
}
/*

 Int32 decoder_decodeVariantBody(char *srcBuf,Int32 type,Int32 *pos, UA_VariantUnion *dstVariantUnion)
 {
 Int32 i = 0;
 dstVariantUnion->Array->Value->
 switch (type)
 {
 case BOOLEAN_ARRAY:

 (*(Boolean_Array*)dstArrayStructure).arrayLength = arrayLength;
 (*(Boolean_Array*)dstArrayStructure).data = (Boolean*)opcua_malloc(arrayLength * sizeof(Boolean));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,BOOLEAN,pos,&(*(Boolean_Array*)dstArrayStructure).data[i]);
 }
 if(arrayDim > 0)
 {
 (*(Boolean_Array*)dstArrayStructure).dimensions.data
 (*(Boolean_Array*)dstArrayStructure).dimensions->data = (Int32*)opcua_malloc(arrayDim->arrayLength * sizeof(Int32));
 (*(Boolean_Array*)dstArrayStructure).dimensions- = arrayDim;
 }
 else
 {
 (*(Boolean_Array*)dstArrayStructure).dimensions = 1;
 (*(Boolean_Array*)dstArrayStructure).data = (*(Boolean_Array*)dstArrayStructure);
 }
 break;

 case SBYTE_ARRAY:
 (*(SByte_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(SByte_Array*)dstArrayStructure).data = (SByte*)opcua_malloc(arrayLength * sizeof(SByte));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,SBYTE,pos,&(*(SByte_Array*)dstArrayStructure).data[i]);
 }
 break;
 case BYTE_ARRAY:
 (*(Byte_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Byte_Array*)dstArrayStructure).data = (Byte*)opcua_malloc(arrayLength * sizeof(Byte));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,BYTE,pos,&(*(Byte_Array*)dstArrayStructure).data[i]);
 }
 break;
 case INT16_ARRAY:
 (*(Int16_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Int16_Array*)dstArrayStructure).data = (Int16*)opcua_malloc(arrayLength * sizeof(Int16));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,INT16,pos,&(*(Int16_Array*)dstArrayStructure).data[i]);
 }
 break;
 case UINT16_ARRAY:
 (*(UInt16_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(UInt16_Array*)dstArrayStructure).data = (UInt16*)opcua_malloc(arrayLength * sizeof(UInt16));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,UINT16,pos,(*(UInt16_Array*)dstArrayStructure).data[i]);
 }
 break;
 case INT32_ARRAY:
 (*(Int32_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Int32_Array*)dstArrayStructure).data = (Int32*)opcua_malloc(arrayLength * sizeof(Int32));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,INT32,pos,(*(Int32_Array*)dstArrayStructure).data[i]);
 }
 break;
 case UINT32_ARRAY:
 (*(UInt32_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(UInt32_Array*)dstArrayStructure).data = (UInt32*)opcua_malloc(arrayLength * sizeof(UInt32));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,UINT32,pos,(*(UInt32_Array*)dstArrayStructure).data[i]);
 }
 break;
 case INT64_ARRAY:
 (*(Int64_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Int64_Array*)dstArrayStructure).data = (Int64*)opcua_malloc(arrayLength * sizeof(Int64));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,INT64,pos,(*(Int64_Array*)dstArrayStructure).data[i]);
 }
 break;
 case UINT64_ARRAY:
 (*(UInt64_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(UInt64_Array*)dstArrayStructure).data = (UInt64*)opcua_malloc(arrayLength * sizeof(UInt64));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,UINT64,pos,(*(UInt64_Array*)dstArrayStructure).data[i]);
 }
 break;
 case FLOAT_ARRAY:
 (*(Float_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Float_Array*)dstArrayStructure).data = (Float*)opcua_malloc(arrayLength * sizeof(Float));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,FLOAT,pos,(*(Float_Array*)dstArrayStructure).data[i]);
 }
 break;
 case DOUBLE_ARRAY:
 (*(Double_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Double_Array*)dstArrayStructure).data = (Double*)opcua_malloc(arrayLength * sizeof(Double));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,DOUBLE,pos,(*(Double_Array*)dstArrayStructure).data[i]);
 }
 break;
 case STRING_ARRAY:
 (*(String_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(String_Array*)dstArrayStructure).data = (UA_String*)opcua_malloc(arrayLength * sizeof(UA_String));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,STRING,pos,(*(String_Array*)dstArrayStructure).data[i]);
 }
 break;
 case DATE_TIME_ARRAY:
 (*(DateTime_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(DateTime_Array*)dstArrayStructure).data = (UA_DateTime*)opcua_malloc(arrayLength * sizeof(UA_DateTime));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,DATE_TIME,pos,(*(DateTime_Array*)dstArrayStructure).data[i]);
 }
 break;
 case GUID_ARRAY:
 (*(Guid_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(Guid_Array*)dstArrayStructure).data = (UA_Guid*)opcua_malloc(arrayLength * sizeof(UA_Guid));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,GUID,pos,(*(Guid_Array*)dstArrayStructure).data[i]);
 }
 break;
 case BYTE_STRING_ARRAY:
 (*(ByteString_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(ByteString_Array*)dstArrayStructure).data = (UA_ByteString*)opcua_malloc(arrayLength * sizeof(UA_ByteString));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,BYTE_STRING,pos,(*(ByteString_Array*)dstArrayStructure).data[i]);
 }
 break;
 case XML_ELEMENT_ARRAY:
 (*(XmlElement_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(XmlElement_Array*)dstArrayStructure).data = (UA_XmlElement*)opcua_malloc(arrayLength * sizeof(UA_XmlElement));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,XML_ELEMENT,pos,(*(XmlElement_Array*)dstArrayStructure).data[i]);
 }
 break;
 case NODE_ID_ARRAY:
 (*(NodeId_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(NodeId_Array*)dstArrayStructure).data = (UA_NodeId*)opcua_malloc(arrayLength * sizeof(UA_NodeId));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,NODE_ID,pos,(*(NodeId_Array*)dstArrayStructure).data[i]);
 }
 break;
 case EXPANDED_NODE_ID_ARRAY:
 (*(ExpandedNodeId_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(NodeId_Array*)dstArrayStructure).data = (UA_ExpandedNodeId*)opcua_malloc(arrayLength * sizeof(UA_ExpandedNodeId));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,EXPANDED_NODE_ID,pos,(*(ExpandedNodeId_Array*)dstArrayStructure).data[i]);
 }
 break;
 case STATUS_CODE_ARRAY:
 (*(StatusCode_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(StatusCode_Array*)dstArrayStructure).data = (UA_StatusCode*)opcua_malloc(arrayLength * sizeof(UA_StatusCode));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,STATUS_CODE,pos,(*(StatusCode_Array*)dstArrayStructure).data[i]);
 }
 break;
 case QUALIFIED_NAME_ARRAY:
 (*(QualifiedName_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(QualifiedName_Array*)dstArrayStructure).data = (UA_QualifiedName*)opcua_malloc(arrayLength * sizeof(UA_QualifiedName));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,QUALIFIED_NAME,pos,(*(QualifiedName_Array*)dstArrayStructure).data[i]);
 }
 break;
 case LOCALIZED_TEXT_ARRAY:
 (*(LocalizedText_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(LocalizedText_Array*)dstArrayStructure).data = (UA_LocalizedText*)opcua_malloc(arrayLength * sizeof(UA_LocalizedText));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,LOCALIZED_TEXT,pos,(*(LocalizedText_Array*)dstArrayStructure).data[i]);
 }
 break;
 case EXTENSION_OBJECT_ARRAY:
 (*(ExtensionObject_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(LocalizedText_Array*)dstArrayStructure).data = (UA_ExtensionObject*)opcua_malloc(arrayLength * sizeof(UA_ExtensionObject));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,EXTENSION_OBJECT,pos,(*(ExtensionObject_Array*)dstArrayStructure).data[i]);
 }
 break;
 case DATA_VALUE_ARRAY:
 (*(DataValue_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(DataValue_Array*)dstArrayStructure).data = (UA_DataValue*)opcua_malloc(arrayLength * sizeof(UA_DataValue));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,DATA_VALUE,pos,(*(DataValue_Array*)dstArrayStructure).data[i]);
 }
 break;
 case VARIANT_ARRAY:
 (*(Variant_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(DataValue_Array*)dstArrayStructure).data = (UA_Variant*)opcua_malloc(arrayLength * sizeof(UA_Variant));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,VARIANT,pos,(*(Variant_Array)dstArrayStructure).data[i]);
 }
 break;
 case DIAGNOSTIC_INFO_ARRAY:
 (*(DiagnosticInfo_Array*)dstArrayStructure).arrayLength = arrayLength;

 (*(DiagnosticInfo_Array*)dstArrayStructure).data = (UA_DiagnosticInfo*)opcua_malloc(arrayLength * sizeof(UA_DiagnosticInfo));
 for(i = 0; i < arrayLength; i++)
 {
 decoder_decodeBuiltInDatatype(srcBuf,DIAGNOSTIC_INFO,pos,(*(DiagnosticInfo_Array)dstArrayStructure).data[i]);
 }
 break;
 }
 return UA_NO_ERROR;
 }


 /* not tested */
/*needs to be reimplemented */
Int32 encode_builtInDatatypeArray(void *data, Int32 size, Int32 type,
		Int32 *pos, char *dstBuf) {
	int i;
	void * pItem;
	encoder_encodeBuiltInDatatype((void*) (size), INT32, pos, dstBuf);
	for (i = 0; i < size;) {
		encoder_encodeBuiltInDatatype(pItem, type, pos, dstBuf);
		switch (type) {
		case BOOLEAN:
			pItem = (Boolean*) (data) + 1;
			break;
		case SBYTE:
			pItem = (SByte*) (data) + 1;
			break;
		case BYTE:
			pItem = (Byte*) (data) + 1;
			break;
		case INT16:
			pItem = (Int16*) (data) + 1;
			break;
		case UINT16:
			pItem = (UInt16*) (data) + 1;
			break;
		case INT32:
			pItem = (Int32*) (data) + 1;
			break;
		case UINT32:
			pItem = (UInt32*) (data) + 1;
			break;
		case INT64:
			pItem = (Int64*) (data) + 1;
			break;
		case UINT64:
			pItem = (UInt64*) (data) + 1;
			break;
		case FLOAT:
			pItem = (Float*) (data) + 1;
			break;
		case DOUBLE:
			pItem = (Double*) (data) + 1;
			break;
		case STRING:
			pItem = (UA_String*) (data) + 1;
			break;
		case DATE_TIME:
			pItem = (UA_DateTime*) (data) + 1;
			break;
		case GUID:
			pItem = (UA_Guid*) (data) + 1;
			break;
		case BYTE_STRING:
			pItem = (UA_ByteString*) (data) + 1;
			break;
		case XML_ELEMENT:
			pItem = (UA_XmlElement*) (data) + 1;
			break;
		case NODE_ID:
			pItem = (UA_NodeId*) (data) + 1;
			break;
		case EXPANDED_NODE_ID:
			pItem = (UA_ExpandedNodeId*) (data) + 1;
			break;
		case STATUS_CODE:
			pItem = (UA_StatusCode*) (data) + 1;
			break;
		case QUALIFIED_NAME:
			pItem = (UA_QualifiedName*) (data) + 1;
			break;
		case LOCALIZED_TEXT:
			pItem = (UA_LocalizedText*) (data) + 1;
			break;
		case EXTENSION_OBJECT:
			pItem = (UA_ExtensionObject*) (data) + 1;
			break;
		case DATA_VALUE:
			pItem = (UA_DataValue*) (data) + 1;
			break;
		case VARIANT:
			pItem = (UA_Variant*) (data) + 1;
			break;
		case DIAGNOSTIC_INFO:
			pItem = (UA_DiagnosticInfo*) (data) + 1;
			break;
		}
	}
	return UA_NO_ERROR;
}

Int32 decodeBoolean(char const * buf, Int32 *pos, Boolean *dst) {
	*dst = ((Boolean) (buf[*pos]) > 0) ? UA_TRUE : UA_FALSE;
	return UA_NO_ERROR;
}
void encodeBoolean(Boolean value, Int32 *pos, char *dstBuf) {
	Boolean tmpBool = ((value > 0) ? UA_TRUE : UA_FALSE);
	memcpy(&(dstBuf[*pos]), &tmpBool, sizeof(Boolean));
}

Int32 decodeSByte(char const * buf, Int32 *pos, SByte *dst) {
	*pos = (*pos) + 1;
	*dst = (SByte) buf[(*pos) - 1];
	return UA_NO_ERROR;

}
void encodeSByte(SByte value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(SByte));
	*pos = (*pos) + 1;

}
Int32 decodeByte(char const * buf, Int32 *pos, Byte* dst) {
	*pos = (*pos) + 1;
	*dst = (Byte) buf[(*pos) - 1];
	return UA_NO_ERROR;

}
void encodeByte(Byte value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Byte));
	*pos = (*pos) + 1;
}

Int32 decodeUInt16(char const * buf, Int32 *pos, UInt16 *dst) {
	Byte t1 = buf[*pos];
	UInt16 t2 = (UInt16) (buf[*pos + 1] << 8);
	*pos += 2;
	*dst = t1 + t2;
	return UA_NO_ERROR;
}
void encodeUInt16(UInt16 value, Int32 *pos, char* dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(UInt16));
	*pos = (*pos) + sizeof(UInt16);
}

Int32 decodeInt16(char const * buf, Int32 *pos, Int16 *dst) {
	Int16 t1 = (Int16) (((SByte) (buf[*pos]) & 0xFF));
	Int16 t2 = (Int16) (((SByte) (buf[*pos + 1]) & 0xFF) << 8);
	*pos += 2;
	*dst = t1 + t2;
	return UA_NO_ERROR;
}
void encodeInt16(Int16 value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Int16));
	*pos = (*pos) + sizeof(Int16);
}

Int32 decodeInt32(char const * buf, Int32 *pos, Int32 *dst) {
	Int32 t1 = (Int32) (((SByte) (buf[*pos]) & 0xFF));
	Int32 t2 = (Int32) (((SByte) (buf[*pos + 1]) & 0xFF) << 8);
	Int32 t3 = (Int32) (((SByte) (buf[*pos + 2]) & 0xFF) << 16);
	Int32 t4 = (Int32) (((SByte) (buf[*pos + 3]) & 0xFF) << 24);
	*pos += sizeof(Int32);
	*dst = t1 + t2 + t3 + t4;
	return UA_NO_ERROR;
}
void encodeInt32(Int32 value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Int32));
	*pos = (*pos) + sizeof(Int32);
}

Int32 decodeUInt32(char const * buf, Int32 *pos, UInt32 *dst) {
	Byte t1 = buf[*pos];
	UInt32 t2 = (UInt32) buf[*pos + 1] << 8;
	UInt32 t3 = (UInt32) buf[*pos + 2] << 16;
	UInt32 t4 = (UInt32) buf[*pos + 3] << 24;
	*pos += sizeof(UInt32);
	*dst = t1 + t2 + t3 + t4;
	return UA_NO_ERROR;
}

void encodeUInt32(UInt32 value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(UInt32));
	*pos += 4;
}

Int32 decodeInt64(char const * buf, Int32 *pos, Int64 *dst) {

	SByte t1 = buf[*pos];
	Int64 t2 = (Int64) buf[*pos + 1] << 8;
	Int64 t3 = (Int64) buf[*pos + 2] << 16;
	Int64 t4 = (Int64) buf[*pos + 3] << 24;
	Int64 t5 = (Int64) buf[*pos + 4] << 32;
	Int64 t6 = (Int64) buf[*pos + 5] << 40;
	Int64 t7 = (Int64) buf[*pos + 6] << 48;
	Int64 t8 = (Int64) buf[*pos + 7] << 56;
	*pos += 8;
	*dst = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8;
	return UA_NO_ERROR;
}
void encodeInt64(Int64 value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Int64));
	*pos = (*pos) + sizeof(Int64);
}

Int32 decodeUInt64(char const * buf, Int32 *pos, UInt64 *dst) {

	Byte t1 = buf[*pos];
	UInt64 t2 = (UInt64) buf[*pos + 1] << 8;
	UInt64 t3 = (UInt64) buf[*pos + 2] << 16;
	UInt64 t4 = (UInt64) buf[*pos + 3] << 24;
	UInt64 t5 = (UInt64) buf[*pos + 4] << 32;
	UInt64 t6 = (UInt64) buf[*pos + 5] << 40;
	UInt64 t7 = (UInt64) buf[*pos + 6] << 48;
	UInt64 t8 = (UInt64) buf[*pos + 7] << 56;
	*pos += 8;
	*dst = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8;
	return UA_NO_ERROR;
}
void encodeUInt64(UInt64 value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(UInt64));
	*pos = (*pos) + sizeof(UInt64);
}

Int32 decodeFloat(char const * buf, Int32 *pos, Float *dst) {
	Float tmpFloat;
	memcpy(&tmpFloat, &(buf[*pos]), sizeof(Float));
	*pos += sizeof(Float);
	*dst = tmpFloat;
	return UA_NO_ERROR;
}
Int32 encodeFloat(Float value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Float));
	*pos += sizeof(Float);
	return UA_NO_ERROR;
}

Int32 decodeDouble(char const * buf, Int32 *pos, Double *dst) {
	Double tmpDouble;
	tmpDouble = (Double) (buf[*pos]);
	*pos += sizeof(Double);
	*dst = tmpDouble;
	return UA_NO_ERROR;
}
Int32 encodeDouble(Double value, Int32 *pos, char *dstBuf) {
	memcpy(&(dstBuf[*pos]), &value, sizeof(Double));
	*pos *= sizeof(Double);
	return UA_NO_ERROR;
}

Int32 decodeUAString(char const * buf, Int32 *pos, UA_String * dstUAString) {

	decoder_decodeBuiltInDatatype(buf, INT32, pos, &(dstUAString->Length));

	if (dstUAString->Length > 0) {
		dstUAString->Data = &(buf[*pos]);
	} else {
		dstUAString->Length = 0;
		dstUAString->Data = (void*) 0;
	}
	*pos += dstUAString->Length;
	return 0;
}

Int32 encodeUAString(UA_String *string, Int32 *pos, char *dstBuf) {
	if (string->Length > 0) {
		memcpy(&(dstBuf[*pos]), &(string->Length), sizeof(Int32));
		*pos += sizeof(Int32);
		memcpy(&(dstBuf[*pos]), string->Data, string->Length);
		*pos += string->Length;
	} else {
		int lengthNULL = 0xFFFFFFFF;
		memcpy(&(dstBuf[*pos]), &lengthNULL, sizeof(Int32));
		*pos += sizeof(Int32);
	}
	return 0;
}
Int32 UAString_calcSize(UA_String *string) {
	if (string->Length > 0) {
		return string->Length + sizeof(string->Length);
	} else {
		return sizeof(Int32);
	}
}

Int32 decodeUADateTime(char const * buf, Int32 *pos, UA_DateTime *dst) {
	decoder_decodeBuiltInDatatype(buf, INT64, pos, dst);
	return UA_NO_ERROR;
}
void encodeUADateTime(UA_DateTime time, Int32 *pos, char *dstBuf) {
	encodeInt64(time, pos, dstBuf);
}

Int32 decodeUAGuid(char const * buf, Int32 *pos, UA_Guid *dstGUID) {
	decoder_decodeBuiltInDatatype(buf, INT32, pos, &(dstGUID->Data1));

	decoder_decodeBuiltInDatatype(buf, INT16, pos, &(dstGUID->Data2));

	decoder_decodeBuiltInDatatype(buf, INT16, pos, &(dstGUID->Data3));

	decoder_decodeBuiltInDatatype(buf, STRING, pos, &(dstGUID->Data4));
	decodeUAByteString(buf, pos, &(dstGUID->Data4));
	return UA_NO_ERROR;
}

Int32 encodeUAGuid(UA_Guid *srcGuid, Int32 *pos, char *buf) {
	encodeUInt32(srcGuid->Data1, pos, buf);
	encodeUInt16(srcGuid->Data2, pos, buf);
	encodeUInt16(srcGuid->Data3, pos, buf);
	encodeUAByteString(srcGuid->Data4, pos, buf);
	return UA_NO_ERROR;

}
Int32 UAGuid_calcSize(UA_Guid *guid) {
	return sizeof(guid->Data1) + sizeof(guid->Data2) + sizeof(guid->Data3)
			+ UAByteString_calcSize(&(guid->Data4));
}

Int32 decodeUAByteString(char const * buf, Int32* pos,
		UA_ByteString *dstBytestring) {

	return decodeUAString(buf, pos, (UA_String*) dstBytestring);

}
Int32 encodeUAByteString(UA_ByteString *srcByteString, Int32* pos, char *dstBuf) {
	return encodeUAString((UA_String*) srcByteString, pos, dstBuf);
}

Int32 encodeXmlElement(UA_XmlElement *xmlElement, Int32 *pos, char *dstBuf) {
	return encodeUAByteString(&(xmlElement->Data), pos, dstBuf);
}
Int32 decodeXmlElement(char const * buf, Int32* pos, UA_XmlElement *xmlElement) {
	return decodeUAByteString(buf, pos, &xmlElement->Data);
}

Int32 UAByteString_calcSize(UA_ByteString *byteString) {
	return UAString_calcSize((UA_String*) byteString);
}

/* Serialization of UANodeID is specified in 62541-6, §5.2.2.9 */
Int32 decodeUANodeId(char const * buf, Int32 *pos, UA_NodeId *dstNodeId) {
	// Vars for overcoming decoder_decodeXXX's non-endian-savenes
	Byte dstByte;
	UInt16 dstUInt16;

	decoder_decodeBuiltInDatatype(buf, BYTE, pos, &(dstNodeId->EncodingByte));

	switch (dstNodeId->EncodingByte) {
	case NIEVT_TWO_BYTE: // Table 7
		decoder_decodeBuiltInDatatype(buf, BYTE, pos, &dstByte);
		dstNodeId->Identifier.Numeric = dstByte;
		dstNodeId->Namespace = 0; // default OPC UA Namespace
		break;
	case NIEVT_FOUR_BYTE: // Table 8
		decoder_decodeBuiltInDatatype(buf, BYTE, pos, &dstByte);
		dstNodeId->Namespace = dstByte;
		decoder_decodeBuiltInDatatype(buf, UINT16, pos, &dstUInt16);
		dstNodeId->Identifier.Numeric = dstUInt16;
		break;
	case NIEVT_NUMERIC: // Table 6, first entry
		decoder_decodeBuiltInDatatype(buf, UINT16, pos,
				&(dstNodeId->Namespace));
		decoder_decodeBuiltInDatatype(buf, UINT32, pos,
				&(dstNodeId->Identifier.Numeric));
		break;
	case NIEVT_STRING: // Table 6, second entry
		decoder_decodeBuiltInDatatype(buf, UINT16, pos,
				&(dstNodeId->Namespace));
		decoder_decodeBuiltInDatatype(buf, STRING, pos,
				&(dstNodeId->Identifier.String));
		break;
	case NIEVT_GUID: // Table 6, third entry
		decoder_decodeBuiltInDatatype(buf, UINT16, pos,
				&(dstNodeId->Namespace));
		decoder_decodeBuiltInDatatype(buf, GUID, pos,
				&(dstNodeId->Identifier.Guid));
		break;
	case NIEVT_BYTESTRING: // Table 6, "OPAQUE"
		decoder_decodeBuiltInDatatype(buf, UINT16, pos,
				&(dstNodeId->Namespace));
		decoder_decodeBuiltInDatatype(buf, BYTE_STRING, pos,
				&(dstNodeId->Identifier.ByteString));
		break;
	}
	return UA_NO_ERROR;
}
Int32 encodeUANodeId(UA_NodeId *srcNodeId, Int32 *pos, char *buf) {
	buf[*pos] = srcNodeId->EncodingByte;
	*pos += sizeof(Byte);
	switch (srcNodeId->EncodingByte) {
	case NIEVT_TWO_BYTE:
		memcpy(&(buf[*pos]), &(srcNodeId->Identifier.Numeric), sizeof(Byte));
		*pos += sizeof(Byte);
		break;
	case NIEVT_FOUR_BYTE:
		encodeByte((Byte) (srcNodeId->Namespace & 0xFF), pos, buf);
		encodeUInt16((UInt16) (srcNodeId->Identifier.Numeric & 0xFFFF), pos,
				buf);
		break;
	case NIEVT_NUMERIC:
		encodeUInt16((UInt16) (srcNodeId->Namespace & 0xFFFF), pos, buf);
		encodeUInt32(srcNodeId->Identifier.Numeric, pos, buf);
		break;
	case NIEVT_STRING:
		encodeUInt16(srcNodeId->Namespace, pos, buf);
		encodeUAString(&(srcNodeId->Identifier.String), pos, buf);
		break;
	case NIEVT_GUID:
		encodeUInt16(srcNodeId->Namespace, pos, buf);
		encodeUAGuid(&(srcNodeId->Identifier.Guid), pos, buf);
		break;
	case NIEVT_BYTESTRING:
		encodeUInt16(srcNodeId->Namespace, pos, buf);
		encodeUAByteString(&(srcNodeId->Identifier.ByteString), pos, buf);
		break;
	}
	return UA_NO_ERROR;
}
Int32 nodeId_calcSize(UA_NodeId *nodeId) {
	Int32 length = 0;
	switch (nodeId->EncodingByte) {
	case NIEVT_TWO_BYTE:
		length += 2 * sizeof(Byte);
		break;
	case NIEVT_FOUR_BYTE:
		length += 4 * sizeof(Byte);
		break;
	case NIEVT_NUMERIC:
		length += sizeof(Byte) + sizeof(UInt16) + sizeof(UInt32);
		break;
	case NIEVT_STRING:
		length += sizeof(Byte) + sizeof(UInt16) + sizeof(UInt32)
				+ nodeId->Identifier.String.Length;
		break;
	case NIEVT_GUID:
		length += sizeof(Byte) + sizeof(UInt16) + sizeof(UInt32)
				+ sizeof(UInt16) + sizeof(UInt16) + 8 * sizeof(Byte);
		break;
	case NIEVT_BYTESTRING:
		length += sizeof(Byte) + sizeof(UInt16) + sizeof(UInt32)
				+ nodeId->Identifier.ByteString.Length;
		break;
	default:
		break;
	}
	return length;
}
/**
 * IntegerId
 * Part: 4
 * Chapter: 7.13
 * Page: 118
 */
Int32 decodeIntegerId(char const * buf, Int32 *pos, Int32 *dst) {
	decoder_decodeBuiltInDatatype(buf, INT32, pos, dst);
	return UA_NO_ERROR;
}
void encodeIntegerId(UA_AD_IntegerId integerId, Int32 *pos, char *buf) {
	encodeInt32(integerId, pos, buf);
}

Int32 decodeExpandedNodeId(char const * buf, Int32 *pos,
		UA_ExpandedNodeId *nodeId) {

	decoder_decodeBuiltInDatatype(buf, BYTE, pos,
			&(nodeId->NodeId.EncodingByte));

	switch (nodeId->NodeId.EncodingByte) {
	case NIEVT_TWO_BYTE:
		decoder_decodeBuiltInDatatype(buf, BYTE, pos,
				&(nodeId->NodeId.Identifier.Numeric));

		break;
	case NIEVT_FOUR_BYTE:
		decoder_decodeBuiltInDatatype(buf, UINT16, pos,
				&(nodeId->NodeId.Identifier.Numeric));
		break;
	case NIEVT_NUMERIC:
		decoder_decodeBuiltInDatatype(buf, UINT32, pos,
				&(nodeId->NodeId.Identifier.Numeric));
		break;
	case NIEVT_STRING:
		decoder_decodeBuiltInDatatype(buf, STRING, pos,
				&(nodeId->NodeId.Identifier.String));
		break;
	case NIEVT_GUID:
		decoder_decodeBuiltInDatatype(buf, GUID, pos,
				&(nodeId->NodeId.Identifier.Guid));
		break;
	case NIEVT_BYTESTRING:
		decoder_decodeBuiltInDatatype(buf, BYTE_STRING, pos,
				&(nodeId->NodeId.Identifier.ByteString));
		break;
	}
	if (nodeId->NodeId.EncodingByte & NIEVT_NAMESPACE_URI_FLAG) {
		nodeId->NodeId.Namespace = 0;
		decoder_decodeBuiltInDatatype(buf, STRING, pos,
				&(nodeId->NamespaceUri));

	}
	if (nodeId->NodeId.EncodingByte & NIEVT_SERVERINDEX_FLAG) {

		decoder_decodeBuiltInDatatype(buf, UINT32, pos, &(nodeId->ServerIndex));

	}
	return UA_NO_ERROR;
}
Int32 encodeExpandedNodeId(UA_ExpandedNodeId *nodeId, Int32 *pos, char *dstBuf) {
	encoder_encodeBuiltInDatatype((void*) &(nodeId->NodeId.EncodingByte), BYTE,
			pos, dstBuf);
	switch (nodeId->NodeId.EncodingByte) {
	case NIEVT_TWO_BYTE:
		encoder_encodeBuiltInDatatype(
				(void*) &(nodeId->NodeId.Identifier.Numeric), BYTE, pos,
				dstBuf);
		break;
	case NIEVT_FOUR_BYTE:
		encoder_encodeBuiltInDatatype(
				(void*) &(nodeId->NodeId.Identifier.Numeric), UINT16, pos,
				dstBuf);
		break;
	case NIEVT_NUMERIC:
		encoder_encodeBuiltInDatatype(
				(void*) &(nodeId->NodeId.Identifier.Numeric), UINT32, pos,
				dstBuf);
		break;
	case NIEVT_STRING:
		encoder_encodeBuiltInDatatype(
				(void*) &(nodeId->NodeId.Identifier.String), STRING, pos,
				dstBuf);
		break;
	case NIEVT_GUID:
		encoder_encodeBuiltInDatatype((void*) &(nodeId->NodeId.Identifier.Guid),
				STRING, pos, dstBuf);
		break;
	case NIEVT_BYTESTRING:
		encoder_encodeBuiltInDatatype(
				(void*) &(nodeId->NodeId.Identifier.ByteString), BYTE_STRING,
				pos, dstBuf);
		break;
	}
	if (nodeId->NodeId.EncodingByte & NIEVT_NAMESPACE_URI_FLAG) {
		nodeId->NodeId.Namespace = 0;
		encoder_encodeBuiltInDatatype((void*) &(nodeId->NamespaceUri), STRING,
				pos, dstBuf);
	}
	if (nodeId->NodeId.EncodingByte & NIEVT_SERVERINDEX_FLAG) {
		encoder_encodeBuiltInDatatype((void*) &(nodeId->ServerIndex), UINT32,
				pos, dstBuf);
	}
	return UA_NO_ERROR;
}

Int32 ExpandedNodeId_calcSize(UA_ExpandedNodeId *nodeId) {
	Int32 length = 0;

	length += sizeof(UInt32); //nodeId->NodeId.EncodingByte

	switch (nodeId->NodeId.EncodingByte) {
	case NIEVT_TWO_BYTE:
		length += sizeof(Byte); //nodeId->NodeId.Identifier.Numeric
		break;
	case NIEVT_FOUR_BYTE:
		length += sizeof(UInt16); //nodeId->NodeId.Identifier.Numeric
		break;
	case NIEVT_NUMERIC:
		length += sizeof(UInt32); //nodeId->NodeId.Identifier.Numeric
		break;
	case NIEVT_STRING:
		//nodeId->NodeId.Identifier.String
		length += UAString_calcSize(&(nodeId->NodeId.Identifier.String));
		break;
	case NIEVT_GUID:
		//nodeId->NodeId.Identifier.Guid
		length += UAGuid_calcSize(&(nodeId->NodeId.Identifier.Guid));
		break;
	case NIEVT_BYTESTRING:
		//nodeId->NodeId.Identifier.ByteString
		length += UAByteString_calcSize(
				&(nodeId->NodeId.Identifier.ByteString));
		break;
	}
	if (nodeId->NodeId.EncodingByte & NIEVT_NAMESPACE_URI_FLAG) {
		length += sizeof(UInt16); //nodeId->NodeId.Namespace
		length += UAString_calcSize(&(nodeId->NamespaceUri)); //nodeId->NamespaceUri
	}
	if (nodeId->NodeId.EncodingByte & NIEVT_SERVERINDEX_FLAG) {
		length += sizeof(UInt32); //nodeId->ServerIndex
	}
	return length;
}

Int32 decodeUAStatusCode(char const * buf, Int32 *pos, UA_StatusCode* dst) {
	decoder_decodeBuiltInDatatype(buf, UINT32, pos, dst);
	return UA_NO_ERROR;

}

Int32 decodeQualifiedName(char const * buf, Int32 *pos,
		UA_QualifiedName *dstQualifiedName) {
	//TODO memory management for ua string
	decoder_decodeBuiltInDatatype(buf, STRING, pos,
			&(dstQualifiedName->NamespaceIndex));
	decoder_decodeBuiltInDatatype(buf, STRING, pos, &(dstQualifiedName->Name));
	return UA_NO_ERROR;
}
Int32 encodeQualifiedName(UA_QualifiedName *qualifiedName, Int32 *pos,
		char *dstBuf) {
	encoder_encodeBuiltInDatatype((void*) &(qualifiedName->NamespaceIndex),
			UINT16, pos, dstBuf);
	encoder_encodeBuiltInDatatype((void*) &(qualifiedName->Name), STRING, pos,
			dstBuf);
	return UA_NO_ERROR;
}
Int32 QualifiedName_calcSize(UA_QualifiedName *qualifiedName) {
	Int32 length = 0;

	length += sizeof(UInt16); //qualifiedName->NamespaceIndex
	length += UAString_calcSize(&(qualifiedName->Name)); //qualifiedName->Name
	length += sizeof(UInt16); //qualifiedName->Reserved

	return length;
}

Int32 decodeLocalizedText(char const * buf, Int32 *pos,
		UA_LocalizedText *dstLocalizedText) {
	//TODO memory management for ua string
	decoder_decodeBuiltInDatatype(buf, BYTE, pos,
			&(dstLocalizedText->EncodingMask));
	decoder_decodeBuiltInDatatype(buf, STRING, pos,
			&(dstLocalizedText->Locale));
	decoder_decodeBuiltInDatatype(buf, STRING, pos, &(dstLocalizedText->Text));

	return UA_NO_ERROR;
}
Int32 encodeLocalizedText(UA_LocalizedText *localizedText, Int32 *pos,
		char *dstBuf) {
	if (localizedText->EncodingMask & 0x01) {
		encoder_encodeBuiltInDatatype((void*) &(localizedText->Locale), STRING,
				pos, dstBuf);
	}
	if (localizedText->EncodingMask & 0x02) {
		encoder_encodeBuiltInDatatype((void*) &(localizedText->Text), STRING,
				pos, dstBuf);
	}
	return UA_NO_ERROR;
}
Int32 LocalizedText_calcSize(UA_LocalizedText *localizedText) {
	Int32 length = 0;

	length += localizedText->EncodingMask;
	if (localizedText->EncodingMask & 0x01) {
		length += UAString_calcSize(&(localizedText->Locale));
	}
	if (localizedText->EncodingMask & 0x02) {
		length += UAString_calcSize(&(localizedText->Text));
	}

	return length;
}

Int32 decodeExtensionObject(char const * buf, Int32 *pos,
		UA_ExtensionObject *dstExtensionObject) {
	decoder_decodeBuiltInDatatype(buf, NODE_ID, pos,
			&(dstExtensionObject->TypeId));
	decoder_decodeBuiltInDatatype(buf, BYTE, pos,
			&(dstExtensionObject->Encoding));
	switch (dstExtensionObject->Encoding) {
	case NO_BODY_IS_ENCODED:
		break;
	case BODY_IS_BYTE_STRING:
	case BODY_IS_XML_ELEMENT:
		decoder_decodeBuiltInDatatype(buf, BYTE_STRING, pos,
				&(dstExtensionObject->Body));
		break;
	}
	return UA_NO_ERROR;
}

Int32 encodeExtensionObject(UA_ExtensionObject *extensionObject, Int32 *pos,
		char *dstBuf) {
	encoder_encodeBuiltInDatatype((void*) &(extensionObject->TypeId), NODE_ID,
			pos, dstBuf);
	encoder_encodeBuiltInDatatype((void*) &(extensionObject->Encoding), BYTE,
			pos, dstBuf);
	switch (extensionObject->Encoding) {
	case NO_BODY_IS_ENCODED:
		break;
	case BODY_IS_BYTE_STRING:
	case BODY_IS_XML_ELEMENT:
		encoder_encodeBuiltInDatatype((void*) &(extensionObject->Body),
				BYTE_STRING, pos, dstBuf);
		break;
	}
	return UA_NO_ERROR;
}
Int32 ExtensionObject_calcSize(UA_ExtensionObject *extensionObject) {
	Int32 length = 0;

	length += nodeId_calcSize(&(extensionObject->TypeId));
	length += sizeof(Byte); //extensionObject->Encoding
	switch (extensionObject->Encoding) {
	case 0x00:
		length += sizeof(Int32); //extensionObject->Body.Length
		break;
	case 0x01:
		length += UAByteString_calcSize(&(extensionObject->Body));
		break;
	case 0x02:
		length += UAByteString_calcSize(&(extensionObject->Body));
		break;
	}

	return length;
}

Int32 decodeVariant(char const * buf, Int32 *pos, UA_Variant *dstVariant) {
	decoder_decodeBuiltInDatatype(buf, BYTE, pos, &(dstVariant->EncodingMask));

	if (dstVariant->EncodingMask & (1 << 7)) {
		decoder_decodeBuiltInDatatype(buf, INT32, pos,
				&(dstVariant->ArrayLength));
		//	dstVariant->Value->
	}

	//TODO implement the multiarray decoding
	return UA_NO_ERROR;
}
Int32 encodeVariant(UA_Variant *variant, Int32 *pos, char *dstBuf) {
	encoder_encodeBuiltInDatatype((void*) &(variant->EncodingMask), BYTE, pos,
			dstBuf);
	/* array of values is encoded */
	if (variant->EncodingMask & (1 << 7)) // array length is encoded
			{
		encoder_encodeBuiltInDatatype((void*) &(variant->ArrayLength), INT32,
				pos, dstBuf);
		if (variant->ArrayLength > 0) {
			//encode array as given by variant type
			encode_builtInDatatypeArray((void*) variant->Value,
					variant->ArrayLength, (variant->EncodingMask & 31), pos,
					dstBuf);
		}
		//single value to encode
		encoder_encodeBuiltInDatatype((void*) variant->Value,
				(variant->EncodingMask & 31), pos, dstBuf);
	} else //single value to encode
	{
		encoder_encodeBuiltInDatatype((void*) variant->Value,
				(variant->EncodingMask & 31), pos, dstBuf);
	}
	if (variant->EncodingMask & (1 << 6)) // encode array dimension field
			{
		encoder_encodeBuiltInDatatype((void*) variant->Value,
				(variant->EncodingMask & 31), pos, dstBuf);
	}
	return UA_NO_ERROR;
}
Int32 Variant_calcSize(UA_Variant *variant) {
	Int32 length = 0;

	length += sizeof(Byte); //variant->EncodingMask
	if (variant->EncodingMask & (1 << 7)) // array length is encoded
			{
		length += sizeof(Int32); //variant->ArrayLength
		if (variant->ArrayLength > 0) {
			//encode array as given by variant type
			//ToDo: tobeInsert: length += the calcSize for VariantUnions
		}
		//single value to encode
		//ToDo: tobeInsert: length += the calcSize for VariantUnions
	} else //single value to encode
	{
		//ToDo: tobeInsert: length += the calcSize for VariantUnions
	}
	if (variant->EncodingMask & (1 << 6)) // encode array dimension field
			{
		//ToDo: tobeInsert: length += the calcSize for VariantUnions
	}

	return length;
}

Int32 decodeDataValue(char const * buf, Int32 *pos, UA_DataValue *dstDataValue) {

	decoder_decodeBuiltInDatatype(buf, BYTE, pos,
			&(dstDataValue->EncodingMask));
	decoder_decodeBuiltInDatatype(buf, VARIANT, pos, &(dstDataValue->Value));

	decoder_decodeBuiltInDatatype(buf, STATUS_CODE, pos,
			&(dstDataValue->Status));

	decoder_decodeBuiltInDatatype(buf, DATE_TIME, pos,
			&(dstDataValue->SourceTimestamp));

	decoder_decodeBuiltInDatatype(buf, UINT16, pos,
			&(dstDataValue->SourcePicoseconds));

	if (dstDataValue->SourcePicoseconds > MAX_PICO_SECONDS) {
		dstDataValue->SourcePicoseconds = MAX_PICO_SECONDS;
	}

	decoder_decodeBuiltInDatatype(buf, DATE_TIME, pos,
			&(dstDataValue->ServerTimestamp));

	decoder_decodeBuiltInDatatype(buf, UINT16, pos,
			&(dstDataValue->ServerPicoseconds));

	if (dstDataValue->ServerPicoseconds > MAX_PICO_SECONDS) {
		dstDataValue->ServerPicoseconds = MAX_PICO_SECONDS;
	}

	//TODO to be implemented
	return UA_NO_ERROR;
}
Int32 encodeDataValue(UA_DataValue *dataValue, Int32 *pos, char *dstBuf) {
	encoder_encodeBuiltInDatatype((void*) &(dataValue->EncodingMask), BYTE, pos,
			dstBuf);

	if (dataValue->EncodingMask & 0x01) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->Value), VARIANT, pos,
				dstBuf);
	}
	if (dataValue->EncodingMask & 0x02) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->Status), STATUS_CODE,
				pos, dstBuf);
	}
	if (dataValue->EncodingMask & 0x04) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->SourceTimestamp),
				DATE_TIME, pos, dstBuf);
	}
	if (dataValue->EncodingMask & 0x08) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->ServerTimestamp),
				DATE_TIME, pos, dstBuf);
	}
	if (dataValue->EncodingMask & 0x10) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->SourcePicoseconds),
				UINT16, pos, dstBuf);
	}

	if (dataValue->EncodingMask & 0x20) {
		encoder_encodeBuiltInDatatype((void*) &(dataValue->ServerPicoseconds),
				UINT16, pos, dstBuf);
	}
	return UA_NO_ERROR;

}
Int32 DataValue_calcSize(UA_DataValue *dataValue) {
	Int32 length = 0;

	length += sizeof(Byte); //dataValue->EncodingMask

	if (dataValue->EncodingMask & 0x01) {
		length += Variant_calcSize(&(dataValue->Value));
	}
	if (dataValue->EncodingMask & 0x02) {
		length += sizeof(UInt32); //dataValue->Status
	}
	if (dataValue->EncodingMask & 0x04) {
		length += sizeof(Int64); //dataValue->SourceTimestamp
	}
	if (dataValue->EncodingMask & 0x08) {
		length += sizeof(Int64); //dataValue->ServerTimestamp
	}
	if (dataValue->EncodingMask & 0x10) {
		length += sizeof(Int64); //dataValue->SourcePicoseconds
	}
	if (dataValue->EncodingMask & 0x20) {
		length += sizeof(Int64); //dataValue->ServerPicoseconds
	}
	return length;
}
/**
 * DiagnosticInfo
 * Part: 4
 * Chapter: 7.9
 * Page: 116
 */
Int32 decodeDiagnosticInfo(char const * buf, Int32 *pos,
		UA_DiagnosticInfo *dstDiagnosticInfo) {

	Byte encodingByte = (buf[*pos]);
	Byte mask;
	for (mask = 1; mask <= 0x40; mask << 2) {

		switch (mask & encodingByte) {
		case DIEMT_SYMBOLIC_ID:
			decoder_decodeBuiltInDatatype(buf, INT32, pos,
					&(dstDiagnosticInfo->SymbolicId));
			//dstDiagnosticInfo->symbolicId = decodeInt32(buf, pos);
			break;
		case DIEMT_NAMESPACE:
			decoder_decodeBuiltInDatatype(buf, INT32, pos,
					&(dstDiagnosticInfo->NamespaceUri));
			//dstDiagnosticInfo->namespaceUri = decodeInt32(buf, pos);
			break;
		case DIEMT_LOCALIZED_TEXT:
			decoder_decodeBuiltInDatatype(buf, INT32, pos,
					&(dstDiagnosticInfo->LocalizedText));
			//dstDiagnosticInfo->localizesText = decodeInt32(buf, pos);
			break;
		case DIEMT_LOCALE:
			decoder_decodeBuiltInDatatype(buf, INT32, pos,
					&(dstDiagnosticInfo->Locale));
			//dstDiagnosticInfo->locale = decodeInt32(buf, pos);
			break;
		case DIEMT_ADDITIONAL_INFO:
			decoder_decodeBuiltInDatatype(buf, STRING, pos,
					&(dstDiagnosticInfo->AdditionalInfo));
			decodeUAString(buf, pos, &dstDiagnosticInfo->AdditionalInfo);
			break;
		case DIEMT_INNER_STATUS_CODE:
			decoder_decodeBuiltInDatatype(buf, STATUS_CODE, pos,
					&(dstDiagnosticInfo->InnerStatusCode));
			//dstDiagnosticInfo->innerStatusCode = decodeUAStatusCode(buf, pos);
			break;
		case DIEMT_INNER_DIAGNOSTIC_INFO:
			//TODO memory management should be checked (getting memory within a function)

			dstDiagnosticInfo->InnerDiagnosticInfo =
					(UA_DiagnosticInfo*) opcua_malloc(
							sizeof(UA_DiagnosticInfo));
			decoder_decodeBuiltInDatatype(buf, DIAGNOSTIC_INFO, pos,
					&(dstDiagnosticInfo->InnerDiagnosticInfo));

			break;
		}
	}
	*pos += 1;
	return 0;
}
Int32 encodeDiagnosticInfo(UA_DiagnosticInfo *diagnosticInfo, Int32 *pos,
		char *dstbuf) {
	Byte mask;
	mask = 0;

	encoder_encodeBuiltInDatatype((void*) (&(diagnosticInfo->EncodingMask)),
			BYTE, pos, dstbuf);
	for (mask = 1; mask <= 0x40; mask = mask << 2) {

		switch (mask & (diagnosticInfo->EncodingMask)) {
		case DIEMT_SYMBOLIC_ID:
			//	puts("diagnosticInfo symbolic id");
			encoder_encodeBuiltInDatatype((void*) &(diagnosticInfo->SymbolicId),
					INT32, pos, dstbuf);
			break;
		case DIEMT_NAMESPACE:
			encoder_encodeBuiltInDatatype(
					(void*) &(diagnosticInfo->NamespaceUri), INT32, pos,
					dstbuf);
			break;
		case DIEMT_LOCALIZED_TEXT:
			encoder_encodeBuiltInDatatype(
					(void*) &(diagnosticInfo->LocalizedText), INT32, pos,
					dstbuf);
			break;
		case DIEMT_LOCALE:
			encoder_encodeBuiltInDatatype((void*) &(diagnosticInfo->Locale),
					INT32, pos, dstbuf);
			break;
		case DIEMT_ADDITIONAL_INFO:
			encoder_encodeBuiltInDatatype(
					(void*) &(diagnosticInfo->AdditionalInfo), STRING, pos,
					dstbuf);
			break;
		case DIEMT_INNER_STATUS_CODE:
			encoder_encodeBuiltInDatatype(
					(void*) &(diagnosticInfo->InnerStatusCode), STATUS_CODE,
					pos, dstbuf);
			break;
		case DIEMT_INNER_DIAGNOSTIC_INFO:
			encoder_encodeBuiltInDatatype(
					(void*) &(diagnosticInfo->InnerDiagnosticInfo),
					DIAGNOSTIC_INFO, pos, dstbuf);
			break;
		}
	}
	return UA_NO_ERROR;
}
Int32 diagnosticInfo_calcSize(UA_DiagnosticInfo *diagnosticInfo) {
	Int32 minimumLength = 1;
	Int32 length = minimumLength;
	Byte mask;
	Int32 j = 0;
	mask = 0;
	//puts("diagnosticInfo called");
	//printf("with this mask %u", diagnosticInfo->EncodingMask);
	for (mask = 1; mask <= 0x40; mask *= 2) {
		j++;
		switch (mask & (diagnosticInfo->EncodingMask)) {

		case DIEMT_SYMBOLIC_ID:
			//	puts("diagnosticInfo symbolic id");
			length += sizeof(Int32);
			break;
		case DIEMT_NAMESPACE:
			length += sizeof(Int32);
			break;
		case DIEMT_LOCALIZED_TEXT:
			length += sizeof(Int32);
			break;
		case DIEMT_LOCALE:
			length += sizeof(Int32);
			break;
		case DIEMT_ADDITIONAL_INFO:
			length += diagnosticInfo->AdditionalInfo.Length;
			length += sizeof(Int32);
			break;
		case DIEMT_INNER_STATUS_CODE:
			length += sizeof(UA_StatusCode);
			break;
		case DIEMT_INNER_DIAGNOSTIC_INFO:
			length += diagnosticInfo_calcSize(
					diagnosticInfo->InnerDiagnosticInfo);
			break;
		}
	}
	return length;
}

/**
 * RequestHeader
 * Part: 4
 * Chapter: 7.26
 * Page: 132
 */
/** \copydoc decodeRequestHeader */
Int32 decodeRequestHeader(const AD_RawMessage *srcRaw, Int32 *pos,
		UA_AD_RequestHeader *dstRequestHeader) {
	return decoder_decodeRequestHeader(srcRaw->message, pos, dstRequestHeader);
}

Int32 decoder_decodeRequestHeader(char const * message, Int32 *pos,
		UA_AD_RequestHeader *dstRequestHeader) {
	// 62541-4 §5.5.2.2 OpenSecureChannelServiceParameters
	// requestHeader - common request parameters. The authenticationToken is always omitted
	decoder_decodeBuiltInDatatype(message, NODE_ID, pos,
			&(dstRequestHeader->authenticationToken));
	decoder_decodeBuiltInDatatype(message, DATE_TIME, pos,
			&(dstRequestHeader->timestamp));
	decoder_decodeBuiltInDatatype(message, UINT32, pos,
			&(dstRequestHeader->requestHandle));
	decoder_decodeBuiltInDatatype(message, UINT32, pos,
			&(dstRequestHeader->returnDiagnostics));
	decoder_decodeBuiltInDatatype(message, STRING, pos,
			&(dstRequestHeader->auditEntryId));
	decoder_decodeBuiltInDatatype(message, UINT32, pos,
			&(dstRequestHeader->timeoutHint));
	decoder_decodeBuiltInDatatype(message, EXTENSION_OBJECT, pos,
			&(dstRequestHeader->additionalHeader));
	// AdditionalHeader will stay empty, need to be changed if there is relevant information

	return 0;
}

/**
 * ResponseHeader
 * Part: 4
 * Chapter: 7.27
 * Page: 133
 */
/** \copydoc encodeResponseHeader */
Int32 encodeResponseHeader(UA_AD_ResponseHeader const * responseHeader,
		Int32 *pos, UA_ByteString *dstBuf) {
	encodeUADateTime(responseHeader->timestamp, pos, dstBuf->Data);
	encodeIntegerId(responseHeader->requestHandle, pos, dstBuf->Data);
	encodeUInt32(responseHeader->serviceResult, pos, dstBuf->Data);
	//Kodieren von String Datentypen

	return 0;
}
Int32 extensionObject_calcSize(UA_ExtensionObject *extensionObject) {
	Int32 length = 0;

	length += nodeId_calcSize(&(extensionObject->TypeId));
	length += sizeof(Byte); //The EncodingMask Byte

	if (extensionObject->Encoding == BODY_IS_BYTE_STRING
			|| extensionObject->Encoding == BODY_IS_XML_ELEMENT) {
		length += sizeof(Int32) + extensionObject->Body.Length;
	}
	return length;
}

Int32 responseHeader_calcSize(UA_AD_ResponseHeader *responseHeader) {
	Int32 minimumLength = 20; // summation of all simple types
	Int32 i, length;
	length += minimumLength;

	for (i = 0; i < responseHeader->noOfStringTable; i++) {
		length += responseHeader->stringTable[i].Length;
		length += sizeof(UInt32); // length of the encoded length field
	}

	length += diagnosticInfo_calcSize(responseHeader->serviceDiagnostics);
	//ToDo
	length += extensionObject_calcSize(&(responseHeader->additionalHeader));

	return length;
}
