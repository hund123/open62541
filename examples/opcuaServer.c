/*
 * This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information.
 */

#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>
#include <errno.h> // errno, EINTR

// provided by the open62541 lib
#include "ua_server.h"
#include "ua_namespace_0.h"

// provided by the user, implementations available in the /examples folder
#include "logger_stdout.h"
#include "networklayer_tcp.h"

UA_Boolean running = 1;

void stopHandler(int sign) {
	running = 0;
}

UA_ByteString loadCertificate() {
    UA_ByteString certificate = UA_STRING_NULL;
	FILE *fp = NULL;
	//FIXME: a potiential bug of locating the certificate, we need to get the path from the server's config
	fp=fopen("localhost.der", "rb");

	if(!fp) {
        errno = 0; // we read errno also from the tcp layer...
        return certificate;
    }

    fseek(fp, 0, SEEK_END);
    certificate.length = ftell(fp);
    certificate.data = malloc(certificate.length*sizeof(UA_Byte));

    fseek(fp, 0, SEEK_SET);
    if(fread(certificate.data, sizeof(UA_Byte), certificate.length, fp) < (size_t)certificate.length)
        UA_ByteString_deleteMembers(&certificate); // error reading the cert
    fclose(fp);

    return certificate;
}

int main(int argc, char** argv) {
	signal(SIGINT, stopHandler); /* catches ctrl-c */

	UA_String endpointUrl;
    UA_String_copycstring("opc.tcp://localhost:16664",&endpointUrl);
	UA_ByteString certificate = loadCertificate();
	UA_Server *server = UA_Server_new(&endpointUrl, &certificate);

	//add a node to the adresspace
    UA_Int32 *myInteger = malloc(sizeof(UA_Int32));
    *myInteger = 42;
    UA_QualifiedName myIntegerName;
    UA_QualifiedName_copycstring("the answer is",&myIntegerName);
    UA_Server_addScalarVariableNode(server, &myIntegerName, myInteger, &UA_TYPES[UA_INT32],
                                    &UA_EXPANDEDNODEIDS[UA_OBJECTSFOLDER], &UA_NODEIDS[UA_ORGANIZES]);
    UA_QualifiedName_deleteMembers(&myIntegerName);
    
#ifdef BENCHMARK
    UA_UInt32 nodeCount = 500;
    UA_Int32 data = 42;
    char str[15];
    for(UA_UInt32 i = 0;i<nodeCount;i++) {
        UA_VariableNode *tmpNode = UA_VariableNode_new();
        sprintf(str,"%d",i);
        UA_QualifiedName_copycstring(str,&tmpNode->browseName);
        UA_LocalizedText_copycstring(str,&tmpNode->displayName);
        UA_LocalizedText_copycstring("integer value", &tmpNode->description);
        tmpNode->nodeId.identifier.numeric = 19000+i;
        tmpNode->nodeClass = UA_NODECLASS_VARIABLE;
        //tmpNode->valueRank = -1;
        tmpNode->value.vt = &UA_TYPES[UA_INT32];
        tmpNode->value.storage.data.dataPtr = &data;
        tmpNode->value.storageType = UA_VARIANT_DATA_NODELETE;
        tmpNode->value.storage.data.arrayLength = 1;
        UA_Server_addNode(server, (const UA_Node**)&tmpNode, &UA_EXPANDEDNODEIDS[UA_OBJECTSFOLDER],
                          &UA_NODEIDS[UA_HASCOMPONENT]);
    }
#endif
	
	#define PORT 16664
    UA_Server_addNetworkLayer(server, NetworkLayerTCP_new(UA_ConnectionConfig_standard, PORT));
	printf("Server started, connect to to opc.tcp://127.0.0.1:%i\n", PORT);
    UA_StatusCode retval = UA_Server_run(server, 1, &running);
	UA_Server_delete(server);
    UA_String_deleteMembers(&endpointUrl);
	return retval;
}
