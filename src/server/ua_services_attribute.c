#include "ua_server_internal.h"
#include "ua_types_generated.h"
#include "ua_services.h"
#include "ua_statuscodes.h"
#include "ua_nodestore.h"
#include "ua_util.h"

static UA_StatusCode
parse_numericrange(const UA_String str, UA_NumericRange *range)
{
    if(str.length < 0 || str.length >= 1023)
        return UA_STATUSCODE_BADINTERNALERROR;
    char *cstring = UA_alloca(str.length+1);
    UA_memcpy(cstring, str.data, str.length);
    cstring[str.length] = 0;
    UA_Int32 index = 0;
    size_t dimensionsIndex = 0;
    size_t dimensionsMax = 3; // more should be uncommon
    struct UA_NumericRangeDimension *dimensions = UA_malloc(sizeof(struct UA_NumericRangeDimension) * 3);
    if(!dimensions)
        return UA_STATUSCODE_BADOUTOFMEMORY;
    
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    do {
        UA_Int32 min, max;
        UA_Int32 progress;
        UA_Int32 res = sscanf(&cstring[index], "%" SCNu32 "%n", &min, &progress);
        if(res <= 0 || min < 0) {
            retval = UA_STATUSCODE_BADINDEXRANGEINVALID;
            break;
        }
        index += progress;
        if(index >= str.length || cstring[index] == ',')
            max = min;
        else {
            res = sscanf(&cstring[index], ":%" SCNu32 "%n", &max, &progress);
            if(res <= 0 || max < 0 || min >= max) {
                retval = UA_STATUSCODE_BADINDEXRANGEINVALID;
                break;
            }
            index += progress;
        }
        
        if(dimensionsIndex >= dimensionsMax) {
            struct UA_NumericRangeDimension *newDimensions =
                UA_realloc(dimensions, sizeof(struct UA_NumericRangeDimension) * 2 * dimensionsMax);
            if(!newDimensions) {
                UA_free(dimensions);
                return UA_STATUSCODE_BADOUTOFMEMORY;
            }
            dimensions = newDimensions;
            dimensionsMax *= 2;
        }

        dimensions[dimensionsIndex].min = min;
        dimensions[dimensionsIndex].max = max;
        dimensionsIndex++;
    } while(retval == UA_STATUSCODE_GOOD && index + 1 < str.length && cstring[index] == ',' && ++index);

    if(retval != UA_STATUSCODE_GOOD) {
        UA_free(dimensions);
        return retval;
    }
        
    range->dimensions = dimensions;
    range->dimensionsSize = dimensionsIndex;
    return retval;
}

#define CHECK_NODECLASS(CLASS)                                  \
    if(!(node->nodeClass & (CLASS))) {                          \
        v->hasStatus = UA_TRUE;                                 \
        v->status = UA_STATUSCODE_BADATTRIBUTEIDINVALID;        \
        break;                                                  \
    }

static void handleServerTimestamps(UA_TimestampsToReturn timestamps, UA_DataValue* v) {
	if (v && (timestamps == UA_TIMESTAMPSTORETURN_SERVER
			|| timestamps == UA_TIMESTAMPSTORETURN_BOTH)) {
		v->hasServerTimestamp = UA_TRUE;
		v->serverTimestamp = UA_DateTime_now();
	}
}

static void handleSourceTimestamps(UA_TimestampsToReturn timestamps, UA_DataValue* v) {
	if(timestamps == UA_TIMESTAMPSTORETURN_SOURCE || timestamps == UA_TIMESTAMPSTORETURN_BOTH) {
		v->hasSourceTimestamp = UA_TRUE;
		v->sourceTimestamp = UA_DateTime_now();
	}
}

/** Reads a single attribute from a node in the nodestore. */
static void readValue(UA_Server *server, UA_TimestampsToReturn timestamps,
                      const UA_ReadValueId *id, UA_DataValue *v) {

	if(id->dataEncoding.name.length >= 0){
		if(memcmp(id->dataEncoding.name.data, "DefaultBinary", 13) != 0 &&
           memcmp(id->dataEncoding.name.data, "DefaultXml", 10) != 0) {
			v->hasStatus = UA_TRUE;
			v->status = UA_STATUSCODE_BADDATAENCODINGINVALID;
			return;
		}
	}

	//index range for a non-value
	if(id->indexRange.length >= 0 && id->attributeId != UA_ATTRIBUTEID_VALUE){
		v->hasStatus = UA_TRUE;
		v->status = UA_STATUSCODE_BADINDEXRANGENODATA;
		return;
	}

    UA_Node const *node = UA_NodeStore_get(server->nodestore, &(id->nodeId));
    if(!node) {
        v->hasStatus = UA_TRUE;
        v->status = UA_STATUSCODE_BADNODEIDUNKNOWN;
        return;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    switch(id->attributeId) {
    case UA_ATTRIBUTEID_NODEID:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->nodeId, &UA_TYPES[UA_TYPES_NODEID]);
        break;
    case UA_ATTRIBUTEID_NODECLASS:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->nodeClass, &UA_TYPES[UA_TYPES_INT32]);
        break;
    case UA_ATTRIBUTEID_BROWSENAME:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->browseName, &UA_TYPES[UA_TYPES_QUALIFIEDNAME]);
        break;
    case UA_ATTRIBUTEID_DISPLAYNAME:
        retval |= UA_Variant_setScalarCopy(&v->value, &node->displayName, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
        if(retval == UA_STATUSCODE_GOOD)
            v->hasVariant = UA_TRUE;
        break;
    case UA_ATTRIBUTEID_DESCRIPTION:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->description, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
        break;
    case UA_ATTRIBUTEID_WRITEMASK:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->writeMask, &UA_TYPES[UA_TYPES_UINT32]);
        break;
    case UA_ATTRIBUTEID_USERWRITEMASK:
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &node->userWriteMask, &UA_TYPES[UA_TYPES_UINT32]);
        break;
    case UA_ATTRIBUTEID_ISABSTRACT:
        CHECK_NODECLASS(UA_NODECLASS_REFERENCETYPE | UA_NODECLASS_OBJECTTYPE | UA_NODECLASS_VARIABLETYPE |
                        UA_NODECLASS_DATATYPE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ReferenceTypeNode *)node)->isAbstract,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;
    case UA_ATTRIBUTEID_SYMMETRIC:
        CHECK_NODECLASS(UA_NODECLASS_REFERENCETYPE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ReferenceTypeNode *)node)->symmetric,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;
    case UA_ATTRIBUTEID_INVERSENAME:
        CHECK_NODECLASS(UA_NODECLASS_REFERENCETYPE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ReferenceTypeNode *)node)->inverseName,
                                          &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);
        break;
    case UA_ATTRIBUTEID_CONTAINSNOLOOPS:
        CHECK_NODECLASS(UA_NODECLASS_VIEW);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ViewNode *)node)->containsNoLoops,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;
    case UA_ATTRIBUTEID_EVENTNOTIFIER:
        CHECK_NODECLASS(UA_NODECLASS_VIEW | UA_NODECLASS_OBJECT);
        v->hasVariant = UA_TRUE;
        if(node->nodeClass == UA_NODECLASS_VIEW){
        	retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ViewNode *)node)->eventNotifier,
                                          	  &UA_TYPES[UA_TYPES_BYTE]);
        } else {
        	retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_ObjectNode *)node)->eventNotifier,
                                              &UA_TYPES[UA_TYPES_BYTE]);
        }
        break;

    case UA_ATTRIBUTEID_VALUE:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE | UA_NODECLASS_VARIABLETYPE);
        {
        	if(node->nodeClass != UA_NODECLASS_VARIABLE) {
    			v->hasVariant = UA_FALSE;
    			handleSourceTimestamps(timestamps, v);
            }

            UA_Boolean hasRange = UA_FALSE;
            UA_NumericRange range;
            if(id->indexRange.length > 0) {
                hasRange = UA_TRUE;
                retval = parse_numericrange(id->indexRange, &range);
                if(retval != UA_STATUSCODE_GOOD)
                    break;
            }

            const UA_VariableNode *vn = (const UA_VariableNode*)node;

            if(vn->variableType == UA_VARIABLENODETYPE_VARIANT) {
                if(hasRange)
                    retval |= UA_Variant_copyRange(&vn->variable.variant, &v->value, range);
                else
                    retval |= UA_Variant_copy(&vn->variable.variant, &v->value);
                if(retval == UA_STATUSCODE_GOOD) {
                    v->hasVariant = UA_TRUE;
                    handleSourceTimestamps(timestamps, v);
                }
            } else {
                UA_DataValue val;
                UA_DataValue_init(&val);
                UA_Boolean sourceTimeStamp = (timestamps == UA_TIMESTAMPSTORETURN_SOURCE ||
                                              timestamps == UA_TIMESTAMPSTORETURN_BOTH);
                retval |= vn->variable.dataSource.read(vn->variable.dataSource.handle, sourceTimeStamp, &val);
                if(retval == UA_STATUSCODE_GOOD) {
                    retval |= UA_DataValue_copy(&val, v); // todo: selection of indexranges
                }
                vn->variable.dataSource.release(vn->variable.dataSource.handle, &val);
            }

            if(hasRange)
                UA_free(range.dimensions);
        }
        break;

    case UA_ATTRIBUTEID_DATATYPE:
		CHECK_NODECLASS(UA_NODECLASS_VARIABLE | UA_NODECLASS_VARIABLETYPE);
		v->hasVariant = UA_TRUE;
		if(node->nodeClass == UA_NODECLASS_VARIABLETYPE){
			retval |= UA_Variant_setScalarCopy(&v->value,
											  &((const UA_VariableTypeNode *)node)->value.type->typeId,
											  &UA_TYPES[UA_TYPES_NODEID]);
		} else {
			const UA_VariableNode *vn = (const UA_VariableNode*)node;
			if(vn->variableType == UA_VARIABLENODETYPE_VARIANT)
				retval |= UA_Variant_setScalarCopy(&v->value, &vn->variable.variant.type->typeId,
												  &UA_TYPES[UA_TYPES_NODEID]);
			else {
				UA_DataValue val;
				UA_DataValue_init(&val);
				retval |= vn->variable.dataSource.read(vn->variable.dataSource.handle, UA_FALSE, &val);
				if(retval != UA_STATUSCODE_GOOD)
					break;
				retval |= UA_Variant_setScalarCopy(&v->value, &val.value.type->typeId,
												  &UA_TYPES[UA_TYPES_NODEID]);
				vn->variable.dataSource.release(vn->variable.dataSource.handle, &val);
				if(retval != UA_STATUSCODE_GOOD)
					break;
			}
		}
        break;

    case UA_ATTRIBUTEID_VALUERANK:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE | UA_NODECLASS_VARIABLETYPE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_VariableTypeNode *)node)->valueRank,
                                          &UA_TYPES[UA_TYPES_INT32]);
        break;

    case UA_ATTRIBUTEID_ARRAYDIMENSIONS:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE | UA_NODECLASS_VARIABLETYPE);
        {
        	//TODO: handle indexRange
            if(node->nodeClass == UA_NODECLASS_VARIABLETYPE)
                break;
                
            const UA_VariableNode *vn = (const UA_VariableNode *)node;
            if(vn->variableType == UA_VARIABLENODETYPE_VARIANT) {
                retval = UA_Variant_setArrayCopy(&v->value, vn->variable.variant.arrayDimensions,
                                                 vn->variable.variant.arrayDimensionsSize,
                                                 &UA_TYPES[UA_TYPES_INT32]);
                if(retval == UA_STATUSCODE_GOOD)
                    v->hasVariant = UA_TRUE;
            } else {
                UA_DataValue val;
                UA_DataValue_init(&val);
                retval |= vn->variable.dataSource.read(vn->variable.dataSource.handle, UA_FALSE, &val);
                if(retval != UA_STATUSCODE_GOOD)
                    break;
                if(!val.hasVariant)
                    retval = UA_STATUSCODE_BADNOTREADABLE;
                else
                    retval = UA_Variant_setArrayCopy(&v->value, val.value.arrayDimensions,
                                                     val.value.arrayDimensionsSize, &UA_TYPES[UA_TYPES_INT32]);
                vn->variable.dataSource.release(vn->variable.dataSource.handle, &val);
            }
        }
        break;

    case UA_ATTRIBUTEID_ACCESSLEVEL:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_VariableNode *)node)->accessLevel,
                                          &UA_TYPES[UA_TYPES_BYTE]);
        break;

    case UA_ATTRIBUTEID_USERACCESSLEVEL:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_VariableNode *)node)->userAccessLevel,
                                          &UA_TYPES[UA_TYPES_BYTE]);
        break;

    case UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_VariableNode *)node)->minimumSamplingInterval,
                                          &UA_TYPES[UA_TYPES_DOUBLE]);
        break;

    case UA_ATTRIBUTEID_HISTORIZING:
        CHECK_NODECLASS(UA_NODECLASS_VARIABLE);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_VariableNode *)node)->historizing,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;

    case UA_ATTRIBUTEID_EXECUTABLE:
        CHECK_NODECLASS(UA_NODECLASS_METHOD);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_MethodNode *)node)->executable,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;

    case UA_ATTRIBUTEID_USEREXECUTABLE:
        CHECK_NODECLASS(UA_NODECLASS_METHOD);
        v->hasVariant = UA_TRUE;
        retval |= UA_Variant_setScalarCopy(&v->value, &((const UA_MethodNode *)node)->userExecutable,
                                          &UA_TYPES[UA_TYPES_BOOLEAN]);
        break;

    default:
        v->hasStatus = UA_TRUE;
        v->status = UA_STATUSCODE_BADATTRIBUTEIDINVALID;
        break;
    }

    UA_NodeStore_release(node);

    if(retval != UA_STATUSCODE_GOOD) {
        v->hasStatus = UA_TRUE;
        v->status = retval;
    }

    handleServerTimestamps(timestamps, v);
}

void Service_Read(UA_Server *server, UA_Session *session, const UA_ReadRequest *request,
                  UA_ReadResponse *response) {
    if(request->nodesToReadSize <= 0) {
        response->responseHeader.serviceResult = UA_STATUSCODE_BADNOTHINGTODO;
        return;
    }

    if(request->timestampsToReturn > 3){
    	response->responseHeader.serviceResult = UA_STATUSCODE_BADTIMESTAMPSTORETURNINVALID;
    	return;
    }

    size_t size = request->nodesToReadSize;

    response->results = UA_Array_new(&UA_TYPES[UA_TYPES_DATAVALUE], size);
    if(!response->results) {
        response->responseHeader.serviceResult = UA_STATUSCODE_BADOUTOFMEMORY;
        return;
    }

    response->resultsSize = size;

    if(request->maxAge < 0) {
    	response->responseHeader.serviceResult = UA_STATUSCODE_BADMAXAGEINVALID;
        return;
    }

    /* ### Begin External Namespaces */
    UA_Boolean *isExternal = UA_alloca(sizeof(UA_Boolean) * size);
    UA_memset(isExternal, UA_FALSE, sizeof(UA_Boolean) * size);
    UA_UInt32 *indices = UA_alloca(sizeof(UA_UInt32) * size);
    for(UA_Int32 j = 0;j<server->externalNamespacesSize;j++) {
        size_t indexSize = 0;
        for(size_t i = 0;i < size;i++) {
            if(request->nodesToRead[i].nodeId.namespaceIndex != server->externalNamespaces[j].index)
                continue;
            isExternal[i] = UA_TRUE;
            indices[indexSize] = i;
            indexSize++;
        }
        if(indexSize == 0)
            continue;
        UA_ExternalNodeStore *ens = &server->externalNamespaces[j].externalNodeStore;
        ens->readNodes(ens->ensHandle, &request->requestHeader, request->nodesToRead,
                       indices, indexSize, response->results, UA_FALSE, response->diagnosticInfos);
    }
    /* ### End External Namespaces */

    for(size_t i = 0;i < size;i++) {
        if(!isExternal[i])
            readValue(server, request->timestampsToReturn, &request->nodesToRead[i], &response->results[i]);
    }

#ifdef EXTENSION_STATELESS
    if(session==&anonymousSession){
		/* expiry header */
		UA_ExtensionObject additionalHeader;
		UA_ExtensionObject_init(&additionalHeader);
		additionalHeader.encoding = UA_EXTENSIONOBJECT_ENCODINGMASK_BODYISBYTESTRING;

		UA_Variant variant;
		UA_Variant_init(&variant);
		variant.type = &UA_TYPES[UA_TYPES_DATETIME];
		variant.arrayLength = request->nodesToReadSize;

		UA_DateTime* expireArray = UA_NULL;
		expireArray = UA_Array_new(&UA_TYPES[UA_TYPES_DATETIME], request->nodesToReadSize);
		variant.dataPtr = expireArray;

		UA_ByteString str;
		UA_ByteString_init(&str);

		/*expires in 20 seconds*/
		for(UA_Int32 i = 0;i < response->resultsSize;i++) {
			expireArray[i] = UA_DateTime_now() + 20 * 100 * 1000 * 1000;
		}
		size_t offset = 0;
		str.data = UA_malloc(UA_Variant_calcSizeBinary(&variant));
		str.length = UA_Variant_calcSizeBinary(&variant);
		UA_Variant_encodeBinary(&variant, &str, &offset);
		additionalHeader.body = str;
		response->responseHeader.additionalHeader = additionalHeader;
    }
#endif
}

static UA_StatusCode writeValue(UA_Server *server, UA_WriteValue *wvalue) {
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    // we might repeat writing, e.g. when the node got replaced mid-work
    UA_Boolean done = UA_FALSE;
    while(!done) {
        const UA_Node *node = UA_NodeStore_get(server->nodestore, &wvalue->nodeId);
        if(!node)
            return UA_STATUSCODE_BADNODEIDUNKNOWN;

        switch(wvalue->attributeId) {
        case UA_ATTRIBUTEID_NODEID:
        case UA_ATTRIBUTEID_NODECLASS:
        case UA_ATTRIBUTEID_BROWSENAME:
        case UA_ATTRIBUTEID_DISPLAYNAME:
        case UA_ATTRIBUTEID_DESCRIPTION:
        case UA_ATTRIBUTEID_WRITEMASK:
        case UA_ATTRIBUTEID_USERWRITEMASK:
        case UA_ATTRIBUTEID_ISABSTRACT:
        case UA_ATTRIBUTEID_SYMMETRIC:
        case UA_ATTRIBUTEID_INVERSENAME:
        case UA_ATTRIBUTEID_CONTAINSNOLOOPS:
        case UA_ATTRIBUTEID_EVENTNOTIFIER:
            retval = UA_STATUSCODE_BADWRITENOTSUPPORTED;
            break;
        case UA_ATTRIBUTEID_VALUE:
            if(node->nodeClass == UA_NODECLASS_VARIABLE) {
                const UA_VariableNode *vn = (const UA_VariableNode*)node;
                if(vn->variableType == UA_VARIABLENODETYPE_DATASOURCE) {
                    if(!vn->variable.dataSource.write) {
                        retval = UA_STATUSCODE_BADWRITENOTSUPPORTED;
                        break;
                    }
                    retval = vn->variable.dataSource.write(vn->variable.dataSource.handle, &wvalue->value.value);
                    done = UA_TRUE;
                    break;
                }

                // array sizes are not compared

                if(!wvalue->value.hasVariant) {
                    retval = UA_STATUSCODE_BADTYPEMISMATCH;
                    break;
                }

                if(!UA_NodeId_equal(&vn->variable.variant.type->typeId, &wvalue->value.value.type->typeId)) {
                    if(vn->variable.variant.type->namespaceZero && wvalue->value.value.type->namespaceZero &&
                       vn->variable.variant.type->typeIndex == wvalue->value.value.type->typeIndex)
                        // an enum was sent as an int32, or an opaque type as a bytestring
                        wvalue->value.value.type = vn->variable.variant.type;
                    else if(vn->variable.variant.type == &UA_TYPES[UA_TYPES_BYTE] &&
                            (!vn->variable.variant.dataPtr || vn->variable.variant.arrayLength > -1) /* isArray */ &&
                            wvalue->value.value.type == &UA_TYPES[UA_TYPES_BYTESTRING] &&
                            wvalue->value.value.dataPtr && wvalue->value.value.arrayLength == -1 /* isScalar */) {
                        // a string is written to a byte array
                        UA_ByteString *str = (UA_ByteString*) wvalue->value.value.dataPtr;
                        wvalue->value.value.arrayLength = str->length;
                        wvalue->value.value.dataPtr = str->data;
                        wvalue->value.value.type = &UA_TYPES[UA_TYPES_BYTE];
                        UA_free(str);
                    } else {
                        retval = UA_STATUSCODE_BADTYPEMISMATCH;
                        break;
                    }
                }

                UA_VariableNode *newVn = UA_VariableNode_new();
                if(!newVn) {
                    retval = UA_STATUSCODE_BADOUTOFMEMORY;
                    break;
                }
                retval = UA_VariableNode_copy(vn, newVn);
                if(retval != UA_STATUSCODE_GOOD) {
                    UA_VariableNode_delete(newVn);
                    break;
                }
                retval = UA_Variant_copy(&wvalue->value.value, &newVn->variable.variant);
                if(retval != UA_STATUSCODE_GOOD) {
                    UA_VariableNode_delete(newVn);
                    break;
                }
                if(UA_NodeStore_replace(server->nodestore, node, (UA_Node*)newVn,
                                        UA_NULL) == UA_STATUSCODE_GOOD)
                    done = UA_TRUE;
                else
                    UA_VariableNode_delete(newVn);
            } else if(node->nodeClass == UA_NODECLASS_VARIABLETYPE) {
                const UA_VariableTypeNode *vtn = (const UA_VariableTypeNode*)node;
                if(!wvalue->value.hasVariant) {
                    retval = UA_STATUSCODE_BADWRITENOTSUPPORTED;
                    break;
                }
                if(!UA_NodeId_equal(&vtn->value.type->typeId, &wvalue->value.value.type->typeId)) {
                    if(!vtn->value.type->namespaceZero || wvalue->value.value.type->namespaceZero ||
                       vtn->value.type->typeIndex != wvalue->value.value.type->typeIndex) {
                        retval = UA_STATUSCODE_BADTYPEMISMATCH;
                        break;
                    }
                    wvalue->value.value.type = vtn->value.type;
                }

                UA_VariableTypeNode *newVtn = UA_VariableTypeNode_new();
                if(!newVtn) {
                    retval = UA_STATUSCODE_BADOUTOFMEMORY;
                    break;
                }
                retval = UA_VariableTypeNode_copy(vtn, newVtn);
                if(retval != UA_STATUSCODE_GOOD) {
                    UA_VariableTypeNode_delete(newVtn);
                    break;
                }
                retval = UA_Variant_copy(&wvalue->value.value, &newVtn->value);
                if(retval != UA_STATUSCODE_GOOD) {
                    UA_VariableTypeNode_delete(newVtn);
                    break;
                }
                if(UA_NodeStore_replace(server->nodestore, node, (UA_Node*)newVtn,
                                        UA_NULL) == UA_STATUSCODE_GOOD)
                    done = UA_TRUE;
                else
                    UA_VariableTypeNode_delete(newVtn);
            } else {
                retval = UA_STATUSCODE_BADWRITENOTSUPPORTED;
            }
            break;
        case UA_ATTRIBUTEID_DATATYPE:
        case UA_ATTRIBUTEID_VALUERANK:
        case UA_ATTRIBUTEID_ARRAYDIMENSIONS:
        case UA_ATTRIBUTEID_ACCESSLEVEL:
        case UA_ATTRIBUTEID_USERACCESSLEVEL:
        case UA_ATTRIBUTEID_MINIMUMSAMPLINGINTERVAL:
        case UA_ATTRIBUTEID_HISTORIZING:
        case UA_ATTRIBUTEID_EXECUTABLE:
        case UA_ATTRIBUTEID_USEREXECUTABLE:
            retval = UA_STATUSCODE_BADWRITENOTSUPPORTED;
            break;
        default:
            retval = UA_STATUSCODE_BADATTRIBUTEIDINVALID;
            break;
        }

        if(retval != UA_STATUSCODE_GOOD)
            break;

        UA_NodeStore_release(node);
    }

    return retval;
}

void Service_Write(UA_Server *server, UA_Session *session,
                   const UA_WriteRequest *request, UA_WriteResponse *response) {
    UA_assert(server != UA_NULL && session != UA_NULL && request != UA_NULL && response != UA_NULL);

    if(request->nodesToWriteSize <= 0){
        response->responseHeader.serviceResult = UA_STATUSCODE_BADNOTHINGTODO;
        return;
    }

    response->results = UA_Array_new(&UA_TYPES[UA_TYPES_STATUSCODE], request->nodesToWriteSize);
    if(!response->results) {
        response->responseHeader.serviceResult = UA_STATUSCODE_BADOUTOFMEMORY;
        return;
    }

    /* ### Begin External Namespaces */
    UA_Boolean *isExternal = UA_alloca(sizeof(UA_Boolean) * request->nodesToWriteSize);
    UA_memset(isExternal, UA_FALSE, sizeof(UA_Boolean)*request->nodesToWriteSize);
    UA_UInt32 *indices = UA_alloca(sizeof(UA_UInt32) * request->nodesToWriteSize);
    for(UA_Int32 j = 0;j<server->externalNamespacesSize;j++) {
        UA_UInt32 indexSize = 0;
        for(UA_Int32 i = 0;i < request->nodesToWriteSize;i++) {
            if(request->nodesToWrite[i].nodeId.namespaceIndex !=
               server->externalNamespaces[j].index)
                continue;
            isExternal[i] = UA_TRUE;
            indices[indexSize] = i;
            indexSize++;
        }
        if(indexSize == 0)
            continue;
        UA_ExternalNodeStore *ens = &server->externalNamespaces[j].externalNodeStore;
        ens->writeNodes(ens->ensHandle, &request->requestHeader, request->nodesToWrite,
                        indices, indexSize, response->results, response->diagnosticInfos);
    }
    /* ### End External Namespaces */
    
    response->resultsSize = request->nodesToWriteSize;
    for(UA_Int32 i = 0;i < request->nodesToWriteSize;i++) {
        if(!isExternal[i])
            response->results[i] = writeValue(server, &request->nodesToWrite[i]);
    }
}
