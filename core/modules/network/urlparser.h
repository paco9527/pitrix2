#ifndef _URLPARSER_H
#define _URLPARSER_H

typedef struct _STRCLIP_INFO
{
    char* head;
    int len;
}STRCLIP_INFO;

typedef struct _URL_INFO
{
    char* org_url;
    STRCLIP_INFO scheme;
    STRCLIP_INFO host;
    STRCLIP_INFO port;
    STRCLIP_INFO path;
    STRCLIP_INFO param;
    STRCLIP_INFO anchor;
}URL_INFO;

int urlparse(char* org_url, URL_INFO* out_url_info);

#endif
