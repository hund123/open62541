#include "ua_server_internal.h"
#include "ua_services.h"
#include "ua_namespace_0.h"
#include "ua_statuscodes.h"
#include "ua_nodestore.h"
#include "ua_session.h"
#include "ua_util.h"

#define COPY_STANDARDATTRIBUTES do {                                    \
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_DISPLAYNAME) {  \
        vnode->displayName = attr.displayName;                          \
        UA_LocalizedText_init(&attr.displayName);                       \
    }                                                                   \
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_DESCRIPTION) {  \
        vnode->description = attr.description;                          \
        UA_LocalizedText_init(&attr.description);                       \
    }                                                                   \
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_WRITEMASK)      \
        vnode->writeMask = attr.writeMask;                              \
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_USERWRITEMASK)  \
        vnode->userWriteMask = attr.userWriteMask;                      \
    } while(0)

static UA_StatusCode parseVariableNode(UA_ExtensionObject *attributes, UA_Node **new_node,
                                       const UA_TypeVTable **vt) {
    if(attributes->typeId.identifier.numeric !=
       UA_NODEIDS[UA_VARIABLEATTRIBUTES].identifier.numeric + UA_ENCODINGOFFSET_BINARY)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;

    UA_VariableAttributes attr;
    UA_UInt32 pos = 0;
    // todo return more informative error codes from decodeBinary
    if(UA_VariableAttributes_decodeBinary(&attributes->body, &pos, &attr) != UA_STATUSCODE_GOOD)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;

    UA_VariableNode *vnode = UA_VariableNode_new();
    if(!vnode) {
        UA_VariableAttributes_deleteMembers(&attr);
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }

    // now copy all the attributes. This potentially removes them from the decoded attributes.
    COPY_STANDARDATTRIBUTES;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_ACCESSLEVEL)
        vnode->accessLevel = attr.accessLevel;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_USERACCESSLEVEL)
        vnode->userAccessLevel = attr.userAccessLevel;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_HISTORIZING)
        vnode->historizing = attr.historizing;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_MINIMUMSAMPLINGINTERVAL)
        vnode->minimumSamplingInterval = attr.minimumSamplingInterval;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_VALUERANK)
        vnode->valueRank = attr.valueRank;

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_ARRAYDIMENSIONS) {
        vnode->arrayDimensionsSize = attr.arrayDimensionsSize;
        vnode->arrayDimensions = attr.arrayDimensions;
        attr.arrayDimensionsSize = -1;
        attr.arrayDimensions = UA_NULL;
    }

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_DATATYPE ||
       attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_OBJECTTYPEORDATATYPE) {
        vnode->dataType = attr.dataType;
        UA_NodeId_init(&attr.dataType);
    }

    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_VALUE) {
        vnode->value = attr.value;
        UA_Variant_init(&attr.value);
    }

    UA_VariableAttributes_deleteMembers(&attr);

    *new_node = (UA_Node*)vnode;
    *vt = &UA_TYPES[UA_VARIABLENODE];
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode parseObjectNode(UA_ExtensionObject *attributes,
                                     UA_Node **new_node, const UA_TypeVTable **vt) {
    if(attributes->typeId.identifier.numeric !=
       UA_NODEIDS[UA_OBJECTATTRIBUTES].identifier.numeric + UA_ENCODINGOFFSET_BINARY)  // VariableAttributes_Encoding_DefaultBinary
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;
    UA_ObjectAttributes attr;
    UA_UInt32 pos = 0;
    // todo return more informative error codes from decodeBinary
    if (UA_ObjectAttributes_decodeBinary(&attributes->body, &pos, &attr) != UA_STATUSCODE_GOOD)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;
    UA_ObjectNode *vnode = UA_ObjectNode_new();
    if(!vnode) {
        UA_ObjectAttributes_deleteMembers(&attr);
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }

    // now copy all the attributes. This potentially removes them from the decoded attributes.
    COPY_STANDARDATTRIBUTES;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_EVENTNOTIFIER)
        vnode->eventNotifier = attr.eventNotifier;
    UA_ObjectAttributes_deleteMembers(&attr);
    *new_node = (UA_Node*) vnode;
    *vt = &UA_TYPES[UA_OBJECTNODE];
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode parseReferenceTypeNode(UA_ExtensionObject *attributes,
                                            UA_Node **new_node, const UA_TypeVTable **vt) {
    UA_ReferenceTypeAttributes attr;
    UA_UInt32 pos = 0;
    // todo return more informative error codes from decodeBinary
    if(UA_ReferenceTypeAttributes_decodeBinary(&attributes->body, &pos, &attr) != UA_STATUSCODE_GOOD)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;
    UA_ReferenceTypeNode *vnode = UA_ReferenceTypeNode_new();
    if(!vnode) {
        UA_ReferenceTypeAttributes_deleteMembers(&attr);
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }

    // now copy all the attributes. This potentially removes them from the decoded attributes.
    COPY_STANDARDATTRIBUTES;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_ISABSTRACT)
        vnode->isAbstract = attr.isAbstract;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_SYMMETRIC)
        vnode->symmetric = attr.symmetric;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_INVERSENAME) {
        vnode->inverseName = attr.inverseName;
        attr.inverseName.text.length = -1;
        attr.inverseName.text.data = UA_NULL;
        attr.inverseName.locale.length = -1;
        attr.inverseName.locale.data = UA_NULL;
    }
    UA_ReferenceTypeAttributes_deleteMembers(&attr);
    *new_node = (UA_Node*) vnode;
    *vt = &UA_TYPES[UA_REFERENCETYPENODE];
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode parseObjectTypeNode(UA_ExtensionObject *attributes,
                                         UA_Node **new_node, const UA_TypeVTable **vt) {
    UA_ObjectTypeAttributes attr;
    UA_UInt32 pos = 0;
    // todo return more informative error codes from decodeBinary
    if(UA_ObjectTypeAttributes_decodeBinary(&attributes->body, &pos, &attr) != UA_STATUSCODE_GOOD)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;
    UA_ObjectTypeNode *vnode = UA_ObjectTypeNode_new();
    if(!vnode) {
        UA_ObjectTypeAttributes_deleteMembers(&attr);
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
    
    // now copy all the attributes. This potentially removes them from the decoded attributes.
    COPY_STANDARDATTRIBUTES;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_ISABSTRACT) {
        vnode->isAbstract = attr.isAbstract;
    }
    UA_ObjectTypeAttributes_deleteMembers(&attr);
    *new_node = (UA_Node*) vnode;
    *vt = &UA_TYPES[UA_OBJECTTYPENODE];
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode parseViewNode(UA_ExtensionObject *attributes, UA_Node **new_node,
                                   const UA_TypeVTable **vt) {
    UA_ViewAttributes attr;
    UA_UInt32 pos = 0;
    // todo return more informative error codes from decodeBinary
    if(UA_ViewAttributes_decodeBinary(&attributes->body, &pos, &attr) != UA_STATUSCODE_GOOD)
        return UA_STATUSCODE_BADNODEATTRIBUTESINVALID;
    UA_ViewNode *vnode = UA_ViewNode_new();
    if(!vnode) {
        UA_ViewAttributes_deleteMembers(&attr);
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
    // now copy all the attributes. This potentially removes them from the decoded attributes.
    COPY_STANDARDATTRIBUTES;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_CONTAINSNOLOOPS)
        vnode->containsNoLoops = attr.containsNoLoops;
    if(attr.specifiedAttributes & UA_NODEATTRIBUTESMASK_EVENTNOTIFIER)
        vnode->eventNotifier = attr.eventNotifier;
    UA_ViewAttributes_deleteMembers(&attr);
    *new_node = (UA_Node*) vnode;
    *vt = &UA_TYPES[UA_VIEWNODE];
    return UA_STATUSCODE_GOOD;
}

static void addNodeFromAttributes(UA_Server *server, UA_Session *session, UA_AddNodesItem *item,
                                  UA_AddNodesResult *result) {
    // adding nodes to ns0 is not allowed over the wire
    if(item->requestedNewNodeId.nodeId.namespaceIndex == 0) {
        result->statusCode = UA_STATUSCODE_BADNODEIDREJECTED;
        return;
    }

    // parse the node
    UA_Node *node;
    const UA_TypeVTable *nodeVT = UA_NULL;

    switch (item->nodeClass) {
    case UA_NODECLASS_OBJECT:
        result->statusCode = parseObjectNode(&item->nodeAttributes, &node, &nodeVT);
        break;
    case UA_NODECLASS_OBJECTTYPE:
        result->statusCode = parseObjectTypeNode(&item->nodeAttributes, &node, &nodeVT);
        break;
    case UA_NODECLASS_REFERENCETYPE:
        result->statusCode = parseReferenceTypeNode(&item->nodeAttributes, &node, &nodeVT);
        break;
    case UA_NODECLASS_VARIABLE:
        result->statusCode = parseVariableNode(&item->nodeAttributes, &node, &nodeVT);
        break;
    default:
        result->statusCode = UA_STATUSCODE_BADNOTIMPLEMENTED;
    }

    if(result->statusCode != UA_STATUSCODE_GOOD)
        return;

    // add the node
    *result = UA_Server_addNodeWithSession(server, session, (const UA_Node **)&node,
                                           &item->parentNodeId, &item->referenceTypeId);
    if(result->statusCode != UA_STATUSCODE_GOOD)
        nodeVT->delete(node);
}

void Service_AddNodes(UA_Server *server, UA_Session *session, const UA_AddNodesRequest *request,
                      UA_AddNodesResponse *response) {
    if(request->nodesToAddSize <= 0) {
        response->responseHeader.serviceResult = UA_STATUSCODE_BADNOTHINGTODO;
        return;
    }

    UA_StatusCode retval = UA_Array_new((void**)&response->results, request->nodesToAddSize,
                                        &UA_TYPES[UA_ADDNODESRESULT]);
    if(retval) {
        response->responseHeader.serviceResult = retval;
        return;
    }

    /* ### Begin External Namespaces */
    UA_Boolean *isExternal = UA_alloca(sizeof(UA_Boolean) * request->nodesToAddSize);
    memset(isExternal, UA_FALSE, sizeof(UA_Boolean)*request->nodesToAddSize);
    UA_UInt32 *indices = UA_alloca(sizeof(UA_UInt32) * request->nodesToAddSize);
    for(UA_Int32 j = 0;j<server->externalNamespacesSize;j++) {
        UA_UInt32 indexSize = 0;
        for(UA_Int32 i = 0;i < request->nodesToAddSize;i++) {
            if(request->nodesToAdd[i].requestedNewNodeId.nodeId.namespaceIndex != server->externalNamespaces[j].index)
                continue;
            isExternal[i] = UA_TRUE;
            indices[indexSize] = i;
            indexSize++;
        }
        if(indexSize == 0)
            continue;
        UA_ExternalNodeStore *ens = &server->externalNamespaces[j].externalNodeStore;
        ens->addNodes(ens->ensHandle, &request->requestHeader, request->nodesToAdd,
                      indices, indexSize, response->results, response->diagnosticInfos);
    }
    /* ### End External Namespaces */
    
    response->resultsSize = request->nodesToAddSize;
    for(int i = 0;i < request->nodesToAddSize;i++) {
        if(!isExternal[i])
            addNodeFromAttributes(server, session, &request->nodesToAdd[i], &response->results[i]);
    }
}

void Service_AddReferences(UA_Server *server, UA_Session *session,
		const UA_AddReferencesRequest *request,
		UA_AddReferencesResponse *response) {
	if (request->referencesToAddSize <= 0) {
		response->responseHeader.serviceResult = UA_STATUSCODE_BADNOTHINGTODO;
		return;
	}
	response->results = UA_alloc(
			sizeof(UA_StatusCode) * request->referencesToAddSize);
	if (!response->results) {
		response->responseHeader.serviceResult = UA_STATUSCODE_BADOUTOFMEMORY;
		return;
	}
	response->resultsSize = request->referencesToAddSize;
	memset(response->results, UA_STATUSCODE_GOOD,
			sizeof(UA_StatusCode) * response->resultsSize);
	/* ### Begin External Namespaces */
//UA_Boolean isExternal[MAX_ADDREFERENCES_SIZE];
	UA_Boolean isExternal[request->referencesToAddSize];
	memset(isExternal, UA_FALSE,
			sizeof(UA_Boolean) * request->referencesToAddSize);
	UA_UInt32 indices[request->referencesToAddSize];
	for (UA_Int32 j = 0; j < server->externalNamespacesSize; j++) {
		UA_UInt32 indexSize = 0;
		for (UA_Int32 i = 0; i < request->referencesToAddSize; i++) {
			if (request->referencesToAdd[i].sourceNodeId.namespaceIndex
					!= server->externalNamespaces[j].index)
				continue;
			isExternal[i] = UA_TRUE;
			indices[indexSize] = i;
			indexSize++;
		}
		if (indexSize == 0)
			continue;
		UA_ExternalNodeStore *ens =
				&server->externalNamespaces[j].externalNodeStore;
		ens->addReferences(ens->ensHandle, &request->requestHeader,
				request->referencesToAdd, indices, indexSize, response->results,
				response->diagnosticInfos);
	}
	/* ### End External Namespaces */
	response->resultsSize = request->referencesToAddSize;
	for (UA_Int32 i = 0; i < response->resultsSize; i++) {
		if (!isExternal[i])
			UA_Server_addReference(server, &request->referencesToAdd[i]);
	}
}

void Service_DeleteNodes(UA_Server *server, UA_Session *session, const UA_DeleteNodesRequest *request,
                         UA_DeleteNodesResponse *response) {

}

void Service_DeleteReferences(UA_Server *server, UA_Session *session,
                              const UA_DeleteReferencesRequest *request,
                              UA_DeleteReferencesResponse *response) {

}
