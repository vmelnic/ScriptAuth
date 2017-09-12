#include "../../structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

pthread_mutex_t script_mutex;

static int already_loaded = 0;
static struct auth scriptauth;
static unsigned char *service = NULL;
static struct pluginlink *pl;

/**
 * Implements scriptauthFunc()
 */
static int scriptauthFunc(struct clientparam *param) {
    int rc = 0;
    struct sockaddr_in sa;
    SASIZETYPE sasize = sizeof(sa);

    if (!param->username || !param->password) return 4;

    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];

    int rc0 = getsockname(param->clisock, (struct sockaddr *) &sa, &sasize);
    int rc1 = getnameinfo((struct sockaddr *) &sa, sasize, hoststr, sizeof(hoststr), portstr, sizeof(portstr),
                          NI_NUMERICHOST | NI_NUMERICSERV);

    char cmd[256];

    snprintf(cmd, sizeof cmd, "%s %s %s %s %s", (char *) service, param->username, param->password, hoststr, portstr);

    pthread_mutex_lock(&script_mutex);

    int status = system(cmd);

    pthread_mutex_unlock(&script_mutex);

    if (status != 0) {
        rc = 1;
    }

    return rc;
}

#ifdef WATCOM
#pragma aux start "*" parm caller [ ] value struct float struct routine [eax] modify [eax ecx edx]
#undef PLUGINCALL
#define PLUGINCALL
#endif

/**
 * Init/start plugin.
 */
PLUGINAPI int PLUGINCALL start(struct pluginlink *pluginlink, int argc, unsigned char **argv) {
    if (argc < 2) return 1;

    pl = pluginlink;

    if (service) pl->myfree(service);

    service = (unsigned char *) pl->mystrdup((char *) argv[1]);

    if (already_loaded) { return (0); }

    already_loaded = 1;

    pthread_mutex_init(&script_mutex, NULL);

    scriptauth.authenticate = scriptauthFunc;
    scriptauth.authorize = pluginlink->checkACL;
    scriptauth.desc = "script";
    scriptauth.next = pluginlink->authfuncs->next;
    pluginlink->authfuncs->next = &scriptauth;

    return 0;
}
