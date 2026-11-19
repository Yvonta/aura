#include "websocketclient.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define WS_RINGBUFFER 1024

struct WEBSOCKETCLIENT {
    WEBSOCKETCLIENTCONFIG config;
    struct lws_context *context;
    struct lws *wsi;
    int is_connected;
    int force_exit;
    
    // Interne verzendbuffer om LWS_PRE padding te garanderen
    unsigned char send_buf[LWS_PRE + 4096];
    size_t pending_send_len;
    
    BUSPACKAGE bp;
    int receivedbytes;
    
    PACKAGERING *rb;
    BUSPACKAGE storage[WS_RINGBUFFER];
};

// Interne libwebsockets callback
static int WEBSOCKETCLIENT_CALLBACK(struct lws *wsi, enum lws_callback_reasons reason,
                              void *user, void *in, size_t len)
{
    WEBSOCKETCLIENT *client = (WEBSOCKETCLIENT *)lws_context_user(lws_get_context(wsi));
    
    // Als de clientinstantie is meegegeven via de wsi user data of context user data:
    // In deze opzet slaan we de client pointer op in de context user data of via wsi user per protocol.
    // Voor flexibiliteit halen we 'client' op via de context user data hieronder ingesteld.
    
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            if(client) 
            {
                client->is_connected = 1;
                if (client->config.on_connect) 
                {
                    client->config.on_connect(client->config.user_data);
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (client && client->config.onreceivefun) 
            {            
                // Check if incoming data exceeds remaining buffer space
                if(client->receivedbytes + len > sizeof(BUSPACKAGE)) 
                {
                  E("Error: Package size overflow! Resetting buffer.");
                  client->receivedbytes = 0;
                  return -1; // Optional: close connection on protocol error
                }

                // Copy incoming chunk into our persistent session buffer
                memcpy(((char*)&client->bp) + client->receivedbytes, in, len);
                client->receivedbytes += len;

                // Check if we have received the full fixed package
                if(client->receivedbytes == sizeof(BUSPACKAGE)) 
                {
                 // Call your handler with the complete fixed-size block
                  client->config.onreceivefun(wsi, &client->bp);
                  client->receivedbytes = 0; 
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if(!client || !client->rb) 
            {
              break;
            }
            
            size_t available = PACKAGERING_AVAILABLEREAD(client->rb);
            if(available > 0)
            {
               BUSPACKAGE bp; 
               PACKAGERING_READ(client->rb, &bp);
               if(WEBSOCKETCLIENT_SEND(client, &bp) < 0) 
               {
                  return -1; // Verbinding sluiten bij schrijffout
               }
               
               if(available > 1)
               {
                 lws_callback_on_writable(wsi);
               }  
            }
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            E("[LWS Client] Verbindingsfout: %s", (in ? (char *)in : "(geen reden)"));
            if (client) {
                client->is_connected = 0;
            }
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            if (client) {
                client->is_connected = 0;
                if (client->config.on_close) {
                    client->config.on_close(client->config.user_data);
                }
            }
            break;
       case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
          if (client && client->wsi && client->is_connected) {
              if (PACKAGERING_AVAILABLEREAD(client->rb) > 0) {
                  lws_callback_on_writable(client->wsi);
              }
          }
          break;     
        default:
            break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "persona-protocol",
        WEBSOCKETCLIENT_CALLBACK,
        0,
        4096,
        0, NULL, 0
    },
    { NULL, NULL, 0, 0, 0, NULL, 0 } // Terminator
};

WEBSOCKETCLIENT *WEBSOCKETCLIENT_CREATE(const WEBSOCKETCLIENTCONFIG *config)
{
    if(!config)
    {
      return NULL;
    }
    
    WEBSOCKETCLIENT *client = (WEBSOCKETCLIENT *)calloc(1, sizeof(WEBSOCKETCLIENT));
    if(!client)
    {
      return NULL;
    }
    memset(client, 0, sizeof(WEBSOCKETCLIENT));
    memcpy(&client->config, config, sizeof(WEBSOCKETCLIENTCONFIG));
    
    
    client->rb = PACKAGERING_CREATE(client->storage, WS_RINGBUFFER);
    

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN; // Wij luisteren niet, wij zijn client
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.user = client; // Koppel client struct aan context user data

    client->context = lws_create_context(&info);
    if(!client->context) 
    {
        E("[LWS Client] Kan LWS context niet aanmaken\n");
        free(client);
        return NULL;
    }

    struct lws_client_connect_info ccinfo;
    memset(&ccinfo, 0, sizeof(ccinfo));

    ccinfo.context = client->context;
    ccinfo.address = client->config.host;
    ccinfo.port = client->config.port;
    ccinfo.path = client->config.path;
    ccinfo.host = ccinfo.address;
    ccinfo.origin = ccinfo.address;
    ccinfo.protocol = protocols[0].name;
    
    if(client->config.use_ssl == 1) 
    {
        ccinfo.ssl_connection = LCCSCF_USE_SSL;
    } 
    else if(client->config.use_ssl == 2) 
    {
        // Handig voor lokale development met self-signed certificaten
        ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
    } 
    else 
    {
        ccinfo.ssl_connection = 0;
    }

    client->wsi = lws_client_connect_via_info(&ccinfo);
    if(!client->wsi)
    {
        E("[LWS Client] Initiële verbindingsopzet mislukt\n");
        // Context blijft bestaan, connectie kan later opnieuw geprobeerd worden
    }

    return client;
}

void WEBSOCKETCLIENT_DESTROY(WEBSOCKETCLIENT *v)
{
    if(!v)
    {
      return;
    }

    if(v->context) 
    {
      lws_context_destroy(v->context);
    }
    
    PACKAGERING_DESTROY(v->rb);
    v->rb = NULL;
    
    free(v);
}

int WEBSOCKETCLIENT_SERVICE(WEBSOCKETCLIENT *v, int timeout_ms)
{
    if(!v || !v->context)
    {
      return -1;
    }
    return lws_service(v->context, timeout_ms);
}

bool WEBSOCKETCLIENT_ISCONNECTED(WEBSOCKETCLIENT *v)
{
    return v ? v->is_connected : false;
}

int WEBSOCKETCLIENT_SEND(WEBSOCKETCLIENT *v, BUSPACKAGE *bp)
{
    if (!v || !bp || !v->wsi || !v->is_connected) return -1;

    // 1. Maak een buffer aan met ruimte voor LWS_PRE headers + onze BUSPACKAGE
    unsigned char send_buf[LWS_PRE + sizeof(BUSPACKAGE)];
    memset(send_buf, 0, sizeof(send_buf));

    // 2. Kopieer het pakket direct ná de LWS_PRE ruimte in de buffer
    memcpy(&send_buf[LWS_PRE], bp, sizeof(BUSPACKAGE));

    // 3. Stuur het weg als een binair WebSocket-pakket
    int n = lws_write(v->wsi, &send_buf[LWS_PRE], sizeof(BUSPACKAGE), LWS_WRITE_BINARY);

    if (n < 0) {
        E("Fout bij het verzenden van pakket via WebSocket!");
        return -1;
    }

    return n; // Geeft het aantal verzonden bytes terug
}

int WEBSOCKETCLIENT_SENDCHAT(WEBSOCKETCLIENT *v, char *msg)
{
    if (!v || !msg || !v->is_connected || !v->rb) return -1;

    size_t available = PACKAGERING_AVAILABLEWRITE(v->rb); 
    if(available <= 0)
    {
      return -1;
    }

    BUSPACKAGE *bp = BUSPACKAGE_CREATE();
    
    if(!bp)
    {
    	return -1;
    }
    
    memset(bp, 0, sizeof(BUSPACKAGE));
    bp->fixed = 0xDEADBEEF;
    bp->type = 1;
    bp->payloadlen = strlen(msg);

    if(bp->payloadlen > sizeof(bp->payload))
    {
      BUSPACKAGE_DESTROY(bp);
      return -1;
    }  
    memcpy(bp->payload, msg, bp->payloadlen);

    //int ret = WEBSOCKETCLIENT_SEND(v, bp);
    PACKAGERING_WRITE(v->rb, bp); 
    
    BUSPACKAGE_DESTROY(bp);
    
    lws_cancel_service(v->context);
    
    return sizeof(BUSPACKAGE);
}

int WEBSOCKETCLIENT_SENDPCM(WEBSOCKETCLIENT *v, char *pcm, int len)
{
    if (!v || !pcm || !v->wsi || !v->is_connected || !v->rb) return -1;

    size_t available = PACKAGERING_AVAILABLEWRITE(v->rb); 
    if(available <= 0)
    {
      return -1;
    }

    BUSPACKAGE *bp = BUSPACKAGE_CREATE();
    
    if(!bp)
    {
      return -1;
    }
    
    memset(bp, 0, sizeof(BUSPACKAGE));
    bp->fixed = 0xDEADBEEF;
    bp->type = 2;
    bp->payloadlen = len;

    if(bp->payloadlen > sizeof(bp->payload))
    {
      E("Error: Payload to big!");
      BUSPACKAGE_DESTROY(bp);
      return -1;
      //bp->payloadlen = sizeof(bp->payload);
    }  
    memcpy(bp->payload, pcm, bp->payloadlen);

    //int ret = WEBSOCKETCLIENT_SEND(v, bp); 
    PACKAGERING_WRITE(v->rb, bp); 
    
    BUSPACKAGE_DESTROY(bp);
    
    lws_cancel_service(v->context);
    
    return sizeof(BUSPACKAGE);
}

int WEBSOCKETCLIENT_SENDEND(WEBSOCKETCLIENT *v)
{
    if(!v || !v->is_connected || !v->rb)
    {
      return -1;
    }
    
    size_t available = PACKAGERING_AVAILABLEWRITE(v->rb); 
    if(available <= 0)
    {
      return -1;
    }

    BUSPACKAGE *bp = BUSPACKAGE_CREATE();
    if(!bp)
    {
    	return -1;
    }
    
    memset(bp, 0, sizeof(BUSPACKAGE));
    bp->fixed = 0xDEADBEEF;
    bp->type = 3;
    bp->payloadlen = 0;

    PACKAGERING_WRITE(v->rb, bp); 
    
    BUSPACKAGE_DESTROY(bp);
    
    lws_cancel_service(v->context);
    
    return sizeof(BUSPACKAGE);
}

