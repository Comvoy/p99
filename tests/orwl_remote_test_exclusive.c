/*
** orwl_wait_list.c
** 
** Made by (Jens Gustedt)
** Login   <gustedt@damogran.loria.fr>
** 
** Started on  Sat Feb 20 10:17:04 2010 Jens Gustedt
** Last update Sun May 12 01:17:25 2002 Speed Blue
*/

#include <signal.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "orwl_thread.h"
#include "orwl_rand.h"
#include "orwl_remote_queue.h"
#include "orwl_callback.h"
#include "orwl_server.h"

static orwl_mirror location;
static orwl_handle *handle = NULL;
static size_t phases = 4;

#define threadof(x) ((((size_t)x) + orwl_np) % orwl_np)

typedef struct {
  size_t mynum;
  size_t phase;
} cb_t;

cb_t* cb_t_init(cb_t *cb) {
  cb->mynum = 0;
  cb->phase = 0;
  return cb;
}

void cb_t_destroy(cb_t *cb) {
  /* empty */
}

DECLARE_NEW_DELETE(cb_t);
DEFINE_NEW_DELETE(cb_t);

DECLARE_CALLBACK(cb_t);

DEFINE_CALLBACK(cb_t) {
  if (!Arg->mynum)
    fprintf(stderr, "callback %zu phase %zu\n",
            Arg->mynum, Arg->phase);
}

typedef struct _arg_t {
  size_t mynum;
  size_t phases;
} arg_t;

arg_t* arg_t_init(arg_t *arg, size_t def) {
  arg->mynum = def;
  arg->phases = def;
  return arg;
}

PROTOTYPE(arg_t*, arg_t_init, arg_t *, size_t);
#define arg_t_init(...) CALL_WITH_DEFAULTS(arg_t_init, 2, __VA_ARGS__)
DECLARE_DEFARG(arg_t_init, , TNULL(size_t));
DEFINE_DEFARG(arg_t_init, , TNULL(size_t));


void arg_t_destroy(arg_t *arg) {
}

DECLARE_NEW_DELETE(arg_t);
DEFINE_NEW_DELETE(arg_t);

DECLARE_THREAD(arg_t);

DEFINE_THREAD(arg_t) {
  size_t const orwl_mynum = Arg->mynum;
  size_t const phases = Arg->phases;
  /* The buffer for the reentrant pseudo random generator */
  rand48_t seed = RAND48_T_INITIALIZER;

  for (size_t orwl_phase = 0; orwl_phase < phases; ++orwl_phase) {
    double const twait = 0.01;
    double const iwait = twait / 10.0;
    double const rwait = twait * orwl_drand(&seed);
    double const await = twait - rwait;
    /* the position to be requested */
    size_t preq = threadof(orwl_mynum + (orwl_phase>>1)) + (orwl_phase % 2)*orwl_np;
    /* the postion where we put the callback and that we acquire */
    size_t pacq = orwl_mynum + (orwl_phase % 2)*orwl_np;
    orwl_state ostate = orwl_write_request(&location, handle + preq, &seed);
    report(!orwl_mynum,  "req, handle %zu, %s",
           preq, orwl_state_getname(ostate));
    /**/
    cb_t *cb = NEW(cb_t);
    cb->mynum = orwl_mynum;
    cb->phase = orwl_phase;
    ostate = orwl_invalid;
    for (size_t try = 0; ostate != orwl_requested; ++try) {
      ostate = orwl_test(handle + pacq);
      if (ostate == orwl_requested || ostate == orwl_acquired) break;
      progress(!orwl_mynum,  try, "test, handle %zu, %s",
               pacq, orwl_state_getname(ostate));
      sleepfor(iwait);
    }
    report(!orwl_mynum, "test, handle %zu, %s",
           pacq, orwl_state_getname(ostate));
    orwl_callback_attach_cb_t(cb, handle[pacq].wh);
    /**/
    sleepfor(await);
    ostate = orwl_acquire(handle + pacq);
    report(!orwl_mynum,  "acq, handle %zu, state %s                            ",
           pacq, orwl_state_getname(ostate));
    sleepfor(rwait);
    orwl_release(handle + pacq, &seed);
    report(!orwl_mynum,  "rel, handle %zu", pacq);
  }
  report(true, "finished");

}

int main(int argc, char **argv) {
  int ret = 0;
  if (argc > 1) phases = str2size_t(argv[1]);
  if (argc > 2) orwl_np = str2size_t(argv[2]);

  report(1, "%s: starting with %zu phases and %zu threads",
          argv[0], phases, orwl_np);
  orwl_types_init();

  orwl_server* srv = NEW(orwl_server, 4, 10);
  pthread_t srv_id;
  orwl_server_create(srv, &srv_id);
  rand48_t seed = RAND48_T_INITIALIZER;

  if (argc > 1) {
    report(1, "connecting to %s", argv[3]);
    orwl_endpoint other = { INITIALIZER };
    orwl_endpoint_parse(&other, argv[3]);

    /* Initialization of the static location */
    orwl_mirror_init(&location, srv->host.ep, other);

    /* wait until the other side is up. */
    /* ep.port is already in host order */
    while (orwl_rpc(&other, &seed, auth_sock_insert_peer, port2host(&srv->host.ep.port))
           == TONES(uint64_t)) {
      ret = pthread_kill(srv_id, 0);
      if (ret) break;
      sleepfor(0.2);
    }
    handle = orwl_handle_vnew(2 * orwl_np);


    /* Half of the threads are created detached and half joinable */
    pthread_t *id = pthread_t_vnew(orwl_np/2);
    arg_t *arg = arg_t_vnew(orwl_np/2);

    for (size_t i = 0; i < orwl_np; ++i) {
      arg_t *myarg = (i%2 ? arg + (i/2) : NEW(arg_t));
      myarg->mynum = i;
      myarg->phases = phases;
      if (i%2)
        arg_t_create(myarg, id + (i/2));
      else
        arg_t_create(myarg, NULL);
    }


    report(1, "%s: waiting for %zu joinable threads",
           argv[0], orwl_np/2);
    for (size_t i = 0; i < orwl_np/2; ++i) {
      arg_t_join(id[i]);
    }

    report(1, "%s: waiting for %zu detached threads",
           argv[0], orwl_np - orwl_np/2);
    if (ret) {
      char mesg[256] = INITIALIZER;
      strerror_r(ret, mesg, 256);
      report(1, "Server already terminated: %s", mesg);
    }
    orwl_pthread_wait_detached();
    report(1, "%s: killing server", argv[0]);
    orwl_server_terminate(srv, &seed);
    orwl_server_join(srv_id);
    report(1, "host %p and next %p", (void*)srv->host.next, (void*)&srv->host);
    orwl_server_delete(srv);

    report(1, "freeing arg");
    arg_t_vdelete(arg);
    report(1, "freeing id");
    pthread_t_vdelete(id);
    report(1, "freeing handle");
    orwl_handle_vdelete(handle);
    report(1, "destroying location");
    orwl_mirror_destroy(&location);
  }  else {
    for (size_t t = 0; ; ++t) {
      ret = pthread_kill(srv_id, 0);
      if (ret) break;
      sleepfor(0.1);
      progress(1, t, " server idle                                           ");
    }
    orwl_server_join(srv_id);
    orwl_server_delete(srv);
  }


  return 0;
}