#include "../../structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];

    int rc0 = getsockname(param->clisock, (struct sockaddr *) &sa, &sasize);
    int rc1 = getnameinfo(
        (struct sockaddr *) &sa, 
        sasize, 
        hoststr, 
        sizeof(hoststr), 
        portstr, 
        sizeof(portstr),
        NI_NUMERICHOST | NI_NUMERICSERV
    );

    char addrstr[16];
    snprintf(addrstr, sizeof addrstr, "%u.%u.%u.%u", 
        (unsigned)(((unsigned char *)(SAADDR(&param->sincr)))[0]),
        (unsigned)(((unsigned char *)(SAADDR(&param->sincr)))[1]),
        (unsigned)(((unsigned char *)(SAADDR(&param->sincr)))[2]),
        (unsigned)(((unsigned char *)(SAADDR(&param->sincr)))[3])
    );

    // char * argv[] = {param->username, param->password, hoststr, portstr, addrstr, NULL};
    // int status;
    // status = execve((char *) service, argv, NULL);
    // fprintf(stderr, "[>>>] status: %d\n", status);

    char *username = "-";
    if (param->username) {
        username = param->username;
    }

    char *password = "-";
    if (param->password) {
        password = param->password;
    }

    char cmd[256];
    snprintf(cmd, sizeof cmd, "%s %s %s %s %s %s", (char *) service, username, password, hoststr, portstr, addrstr);
    // fprintf(stderr, "[>>>] cmd: %s\n", cmd);

    int status = system(cmd);
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

    scriptauth.authenticate = scriptauthFunc;
    scriptauth.authorize = pluginlink->checkACL;
    scriptauth.desc = "script";
    scriptauth.next = pluginlink->authfuncs->next;
    pluginlink->authfuncs->next = &scriptauth;

    return 0;
}
