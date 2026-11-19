#pragma once

#include <libwebsockets.h>
#include <stdbool.h>
#include <stddef.h>

#include "config.h"
#include "commons.h"
#include "linkedlist.h"
#include "log.h"
#include "buspackage.h"
#include "packagering.h"

// Callback functies voor gebeurtenissen
typedef void (*lws_client_connect_cb)(void *user_data);
typedef void (*lws_client_close_cb)(void *user_data);

typedef struct {
    const char *host;
    int port;
    const char *path;
    int use_ssl; // 0 = geen SSL, 1 = SSL/WSS, 2 = SSL met zelf-getekend certificaat negeren
    void *user_data;
    
    lws_client_connect_cb on_connect;
    lws_client_close_cb on_close;
    void (*onreceivefun)(struct lws *wsi, BUSPACKAGE *bp);
} WEBSOCKETCLIENTCONFIG;

typedef struct WEBSOCKETCLIENT WEBSOCKETCLIENT;

// API functies
WEBSOCKETCLIENT *WEBSOCKETCLIENT_CREATE(const WEBSOCKETCLIENTCONFIG *config);
void WEBSOCKETCLIENT_DESTROY(WEBSOCKETCLIENT *client);

// Start de service loop (blokkerend of per iteratie aan te roepen via timeout_ms)
int WEBSOCKETCLIENT_SERVICE(WEBSOCKETCLIENT *client, int timeout_ms);

// Verstuur data over de websocket

//int WEBSOCKETCLIENT_SEND(WEBSOCKETCLIENT *client, const unsigned char *data, size_t len, enum lws_write_protocol protocol);
int WEBSOCKETCLIENT_SEND(WEBSOCKETCLIENT *v, BUSPACKAGE *bp);
int WEBSOCKETCLIENT_SENDCHAT(WEBSOCKETCLIENT *v, char *msg);
int WEBSOCKETCLIENT_SENDPCM(WEBSOCKETCLIENT *v, char *pcm, int len);
int WEBSOCKETCLIENT_SENDEND(WEBSOCKETCLIENT *v);

bool WEBSOCKETCLIENT_ISCONNECTED(WEBSOCKETCLIENT *client);



