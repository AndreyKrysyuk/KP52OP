#include "web.h"

struct keyvalue_s{
    char key[256];
    char value[256];
};

struct http_request_s{
    char method[8];
    char uri[256];
    keyvalue_t * form;
    int formLength;
};

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
	strcpy((char*)stream, (char*)ptr);
	return size*count;
}

static void sendErrorMessageXML(int code, char * text, socket_t * clientSocket){
     char result_msg[MSG_LENGTH];
    sprintf(result_msg,
            "<message>\n"
            "\t<status>Error</status>\n"
            "\t<code>%i</code>\n"
            "\t<text>%s</text>\n"
            "</message>\n",
            code, text);
    socket_write_string(clientSocket, result_msg);
}

http_request_t
http_request_parse(const char * const request) {
    http_request_t req;
    req.form = NULL;
    req.formLength = 0;
    // get method
    ptrdiff_t methodLen = strstr(request, " ") - request;  // find first whitespace
    memcpy(req.method, request, methodLen);
    req.method[methodLen] = '\0';
    // get uri
    const char * uriStartPtr = request + strlen(req.method) + 1;
    const char * uriEndPtr = strstr(uriStartPtr, " ");  // find second whitespace
    ptrdiff_t uriLen = uriEndPtr - uriStartPtr;
    memcpy(req.uri, uriStartPtr, uriLen);
    req.uri[uriLen] = '\0';
    // parse form data
    const char * bodyStartPtr = strstr(request, "\r\n\r\n") + strlen("\r\n\r\n");
    const char * cur = bodyStartPtr;
    const char * pairEndPtr = cur;
    const char * eqPtr = cur;
    while (strlen(cur) > 0) {
        pairEndPtr = strchr(cur, '&');
        if (NULL == pairEndPtr) {
            pairEndPtr = cur + strlen(cur);
        }
        keyvalue_t kv;
        // get key
        eqPtr = strchr(cur, '=');
        ptrdiff_t keyLen = eqPtr - cur;
        memcpy(kv.key, cur, keyLen);
        kv.key[keyLen] = '\0';
        // get value
        eqPtr++;
        ptrdiff_t valueLen = pairEndPtr - eqPtr;
        memcpy(kv.value, eqPtr, valueLen);
        kv.value[valueLen] = '\0';
        // insert key-value pair into request form list
        req.formLength += 1;
        req.form = realloc(req.form, sizeof(keyvalue_t) * req.formLength);
        req.form[req.formLength - 1] = kv;
        cur = pairEndPtr + ((strlen(pairEndPtr) > 0) ? 1 : 0);
    }
    return req;
}

const char *
http_request_getArg(http_request_t * self, const char * key) {
    for (int i = 0; i < self->formLength; i++) {
        if (strcmp(self->form[i].key, key) == 0) {
            return self->form[i].value;
        }
    }
    return NULL;
}

const char * keyvalue_toString(keyvalue_t * self) {
    char * str = malloc(sizeof(char) * (strlen(self->key) + strlen(self->value) + 2));
    sprintf(str, "%s=%s\0", self->key, self->value);
    return str;
}

void http_request_startServer(pupils_all_t pupils){
    lib_init();
    socket_t * serverSocket = socket_new();
    socket_bind(serverSocket, 5000);
    socket_listen(serverSocket);
    while(1){
        puts("Waiting for connections");
        socket_t * clientSocket = socket_accept(serverSocket);
        puts("New client");
        char buff[BUFFER_LENGTH];
        int readLength = socket_read(clientSocket, buff, BUFFER_LENGTH);
        if(readLength == 0){
            socket_close(clientSocket);
            socket_free(clientSocket);
            puts("Skipping empty request");
            continue;
        }

        printf("Got Request:\n---------------\n%s\n----------------\n", buff);

        http_request_t req = http_request_parse(buff);

        printf("Method: %s\nURI: %s\n", req.method, req.uri);
        puts("Data:");
        for(int i = 0; i < req.formLength; i++){
            char * kvStr = keyvalue_toString(&req.form[i]);
            printf("\t%s\n", kvStr);
            free(kvStr);
        }
        http_request_chooseMethod(req, clientSocket, pupils);
        socket_close(clientSocket);
        socket_free(clientSocket);
    }
    socket_close(serverSocket);
    socket_free(serverSocket);
    lib_free();
}

void http_request_chooseMethod(http_request_t req, socket_t * clientSocket, pupils_all_t pupils){
    if(strcmp(req.uri, "/info") == 0){
        if(strcmp(req.method, "GET") == 0){
            char result[MSG_LENGTH];
            char * buffer = malloc(sizeof(char)*BUFFER_LENGTH);
            web_printInfoNodeXML(buffer);
            sprintf(result,
                        "HTTP/1.1 200 OK\n"
                        "Content-length: %zu\n"
                        "Content-type: application/xml\n"
                        "\n"
                        "%s\0",
                        strlen(buffer), buffer);
                socket_write_string(clientSocket, result);
        }
    } else if (strcmp(req.uri, "/database") == 0){
        if(strcmp(req.method, "GET") == 0){
         char result[MSG_LENGTH];
        sprintf(result,
                        "HTTP/1.1 200 OK\n"
                        "Content-length: %zu\n"
                        "Content-type: application/xml\n"
                        "\n"
                        "%s\0",
                        strlen(pupils_all_msgAll(pupils)), pupils_all_msgAll(pupils));
                socket_write_string(clientSocket, result);
        }

    } else if(strcmp(req.uri, "/external") == 0){
         if(strcmp(req.method, "GET") == 0){
                	/*CURL *curl;
                    CURLcode res;

                    curl_global_init(CURL_GLOBAL_ALL);

                    curl = curl_easy_init();
                    char resString[5000];

                    char* resuestFields = "result=4";

                    curl_easy_setopt(curl, CURLOPT_URL, "http://pb-homework.appspot.com/test/var/26?format=xml2");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, resuestFields);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resString);

                    res = curl_easy_perform(curl);

                    if (res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

                    puts(resString);
                    curl_easy_cleanup(curl);

                    curl_global_cleanup();*/
         }
    } else if(strcmp(req.uri, "/files/") > 0){
         if(strcmp(req.method, "GET") == 0){
                char * name = strstr(req.uri, "s/");
                char * filename = malloc(sizeof(char)*1000);
                for(int i = 0; i < strlen(name); i++){
                    filename[i] = name[i+2];
                }
                strcat(filename, ".txt");
                if(file_exists(filename) == 0){
                    sendErrorMessageXML(404, "File not found", clientSocket);
                } else {
                     char text [1000000];
                     FILE* finput = fopen(filename, "r");
                     int len = fread(text, sizeof(char), sizeof(text)/sizeof(char), finput);
                     text[len] = '\0';
                     fclose(finput);
                    char buff[100000];
                     list_t * list = web_textToList(text);

                     socket_write_string(clientSocket, web_listToHTML(list));
                }
         }
    }
    else sendErrorMessageXML(404, "NOT FOUND", clientSocket);
}



void web_printInfoNodeXML(char * buffer){
  	xmlDoc * doc = NULL;
	xmlNode * rootNode = NULL;
	char strBuf[100];

	doc = xmlNewDoc("1.0");
	rootNode = xmlNewNode(NULL, "student");
	xmlDocSetRootElement(doc, rootNode);
	xmlNewChild(rootNode, NULL, "student", "Andrey Krysyuk");
	xmlNewChild(rootNode, NULL, "group", "KP-52");
	sprintf(strBuf, "%i", 26);
	xmlNewChild(rootNode, NULL, "variant", strBuf);
	xmlBuffer * bufferPtr = xmlBufferCreate();
	xmlNodeDump(bufferPtr, NULL, (xmlNode *)doc, 0, 1);
	strcpy(buffer, strstr((char *)bufferPtr->content, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+1);
	xmlBufferFree(bufferPtr);

    xmlFreeDoc(doc);
	xmlCleanupParser();
}

list_t * web_textToList(char * text){
    list_t * list = list_new();
    char * word = strtok(text, "\" ,;:)([]");
    list_push_back(list, word);
    while(word != NULL){
        word = strtok(NULL, "\" ,;:)([]");
        if(word != NULL){
            list_push_back(list, word);
        }
    }
    return list;
}

char * web_listToHTML(list_t * list){
    char buff[1000000];
    strcat(buff, "<!DOCTYPE html>\n"
                    "<html>\n"
                    "<head>\n"
                    "<meta charset=\"UTF-8\"\n>"
                    "<title>Sample</title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<ul>\n");
    for(int i = 0; i < list_getSize(list); i++){
       strcat(buff, "<li>");
       strcat(buff, list_get(list, i));
       strcat(buff, "</li>\n");
    }
    strcat(buff, "</ui>\n"
                    "</body>\n"
                    "</html>\n");
    return buff;
}

