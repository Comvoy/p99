/*
** orwl_socket.c
** 
** Made by (Jens Gustedt)
** Login   <gustedt@damogran.loria.fr>
** 
** Started on  Sun Mar 21 22:00:19 2010 Jens Gustedt
** Last update Sun May 12 01:17:25 2002 Speed Blue
*/

#include <arpa/inet.h>
#include <stdio.h>

#include "orwl_socket.h"
#include "orwl_rand.h"

static uint32_t mycode = 0;

typedef uint32_t U;
#define Code(C0, C1, C2, C3) ((((U)C0) << 0) | (((U)C1) << 8) | (((U)C2) << 16) | (((U)C3) << 24))

#define ifCode(C0, C1, C2, C3)                          \
do {                                                    \
  static uint32_t const c = Code(C0, C1, C2, C3);       \
  if (ntohl(c) == Code(0, 1, 2, 3)) {                   \
    mycode = c;                                         \
    goto END;                                           \
  }                                                     \
 } while (false)


DECLARE_ONCE_UPON(mycode, mycode);
DEFINE_ONCE_UPON(mycode, mycode) {
    ifCode(0, 1, 2, 3);
    ifCode(0, 1, 3, 2);
    ifCode(0, 2, 1, 3);
    ifCode(0, 2, 3, 1);
    ifCode(0, 3, 1, 2);
    ifCode(0, 3, 2, 1);

    ifCode(1, 0, 2, 3);
    ifCode(1, 0, 3, 2);
    ifCode(1, 2, 0, 3);
    ifCode(1, 2, 3, 0);
    ifCode(1, 3, 0, 2);
    ifCode(1, 3, 2, 0);

    ifCode(2, 0, 1, 3);
    ifCode(2, 0, 3, 1);
    ifCode(2, 1, 0, 3);
    ifCode(2, 1, 3, 0);
    ifCode(2, 3, 0, 1);
    ifCode(2, 3, 1, 0);

    ifCode(3, 0, 1, 2);
    ifCode(3, 0, 2, 1);
    ifCode(3, 1, 0, 2);
    ifCode(3, 1, 2, 0);
    ifCode(3, 2, 0, 1);
    ifCode(3, 2, 1, 0);
 END:;
}

bool same_endianess(uint32_t c) {
  INIT_ONCE_UPON(mycode, mycode);
  return c == mycode;
}

void FUNC_DEFAULT(orwl_hton)(uint32_t *n, uint64_t const *h, size_t l);
define_default_arg(orwl_hton, 2, size_t);
void FUNC_DEFAULT(orwl_ntoh)(uint64_t* h, uint32_t const *n, size_t l);
define_default_arg(orwl_ntoh, 2, size_t);

static
bool orwl_send_(int fd, uint64_t const*mess, size_t len) {
  uint32_t *buf = uint32_t_vnew(2 * len);
  orwl_hton(buf, mess, len);
  ssize_t ret = send(fd, &buf, sizeof(uint64_t) * len, MSG_WAITALL);
  uint32_t_vdelete(buf);
  return ret != sizeof(uint64_t) * len;
}

static
bool orwl_recv_(int fd, uint64_t *mess, size_t len) {
  uint32_t *buf = uint32_t_vnew(2 * len);
  ssize_t ret = recv(fd, &buf, sizeof(uint64_t) * len, MSG_WAITALL);
  orwl_ntoh(mess, buf , len);
  uint32_t_vdelete(buf);
  return ret != sizeof(uint64_t) * len;
}


in_addr_t _inet4_addr = INITIALIZER;

DEFINE_ONCE_UPON(inet4_addr, _inet4_addr) {
  char const* str = getenv("INET4");
  struct in_addr inaddr = INITIALIZER;
  if (inet_aton(str, &inaddr)) {
    _inet4_addr = inaddr.s_addr;
  }
}

in_addr_t inet4_addr(void);


void FUNC_DEFAULT(orwl_endpoint_init)
     (orwl_endpoint *endpoint,
      in_addr_t addr,
      in_port_t port
      );

void orwl_endpoint_destroy(orwl_endpoint *endpoint);


define_default_arg(orwl_endpoint_init, 2, in_port_t);
define_default_arg(orwl_endpoint_init, 1, in_addr_t);

DEFINE_NEW_DELETE(orwl_endpoint);

enum { header_t_els = 2 };

typedef uint64_t header_t[header_t_els];

struct auth_sock {
  int fd;
  size_t len;
  server_cb_t const cb;
};

typedef struct auth_sock auth_sock;

void auth_sock_init(auth_sock *sock) {
  /* empty */
}

void auth_sock_destroy(auth_sock *sock) {
  if (sock->fd != -1) close(sock->fd);
  sock->fd = -1;
}

DECLARE_NEW_DELETE(auth_sock);
DEFINE_NEW_DELETE(auth_sock);

DECLARE_THREAD(auth_sock);


DEFINE_THREAD(auth_sock) {
  uint64_t *mess = uint64_t_vnew(Arg->len);
  if (!orwl_recv_(Arg->fd, mess, Arg->len))
    if (Arg->cb)
      /* do something with mess here */
      Arg->cb(mess, Arg->len);
  /* now clean up */
  uint64_t_vdelete(mess);
  header_t header = INITIALIZER;
  /* Ack the termination of the call */
  orwl_send_(Arg->fd, header, header_t_els);
  close(Arg->fd);
}


DEFINE_THREAD(orwl_server) {
  int fd_listen = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_listen != -1) {
    rand48_t seed = { time(NULL) };
    struct sockaddr_in addr = INITIALIZER;
    socklen_t len = sizeof(addr);
    addr.sin_addr.s_addr = Arg->ep.addr;
    addr.sin_port = Arg->ep.port;
    addr.sin_family = AF_INET;
    if (bind(fd_listen, (struct sockaddr*) &addr, sizeof(addr)) == -1)
      goto TERMINATE;
    /* If the port was not yet specified find and store it. */
    if (!addr.sin_port) {
      if (getsockname(fd_listen, (struct sockaddr*)&addr, &len) == -1)
        goto TERMINATE;
      Arg->ep.port = addr.sin_port;
    }
    if (listen(fd_listen, Arg->max_connections) == -1)
      goto TERMINATE;
    while (fd_listen != -1) {
      /* Do this work before being connected */
      uint64_t chal = orwl_rand64(seed);
      uint64_t repl = orwl_challenge(chal);
      header_t header = INITIALIZER;

      int fd = -1;
      do {
        fd = accept(fd_listen, NULL, &TNULL(socklen_t));
      } while(fd == -1);

      /* Receive a challenge from the new connection */
      if (orwl_recv_(fd, header, header_t_els))
        goto FINISH;
      header[1] = orwl_challenge(header[0]);
      header[0] = chal;
      /* Send the reply and a new challenge to the new connection */
      if (orwl_send_(fd, header, header_t_els))
        goto FINISH;
      /* Receive the reply and the message length from the new connection */
      if (orwl_recv_(fd, header, header_t_els))
        goto FINISH;
      if (header[1] == repl) {
        size_t len = header[0];
        if (len) {
          auth_sock sock = { .fd = fd, .len = len, .cb = Arg->cb };
          auth_sock_create(&sock, NULL);
          /* The spawned thread will close the fd. */
          continue;
        } else {
          /* The authorized empty message indicates termination */
          close(fd);
          close(fd_listen);
          fd_listen = -1;
          break;
        }
      }
    FINISH:
      close(fd);
    }
  }
 TERMINATE:
  if (fd_listen != -1) close(fd_listen);
}

void orwl_send(orwl_endpoint const* ep, rand48_t seed, uint64_t const* mess, size_t len) {
  /* do all this work before opening the socket */
  uint64_t chal = orwl_rand64(seed);
  uint64_t repl = orwl_challenge(chal);
  header_t header = { [0] = chal };
  struct sockaddr_in addr = {
    .sin_addr = { .s_addr = ep->addr },
    .sin_port = ep->port,
    .sin_family = AF_INET
  };

  int fd = socket(PF_INET, SOCK_STREAM, 0);
  if (fd == -1) return;

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    goto FINISH;
  /* Send a challenge to the other side */
  if (orwl_send_(fd, header, header_t_els))
    goto FINISH;
  /* Receive the answer and another challenge from the other side */
  if (orwl_recv_(fd, header, header_t_els))
    goto FINISH;
  if (header[1] == repl) {
    /* The other side is authorized. Send the answer and the size of
       the message to the other side. */
    header[1] = orwl_challenge(header[0]);
    header[0] = len;
    if (orwl_send_(fd, header, header_t_els))
      goto FINISH;
    /* The authorized empty message indicates termination.
       If not so, we now send the real message. */
    if (len) {
      if (orwl_send_(fd, mess, len))
        goto FINISH;
      /* Receive a final message, until the other end closes the
         connection. */
      orwl_recv_(fd, header, header_t_els);
    }
  } else  {
    /* The other side is not authorized. Terminate. */
    header[1] = 0;
    header[0] = 0;
    if (orwl_send_(fd, header, header_t_els))
      goto FINISH;
  }
 FINISH:
  close(fd);
}