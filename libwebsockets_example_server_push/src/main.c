/*
 * https://gist.github.com/martinsik/3654228
 *
 * http://www.codepool.biz/libwebsockets-introduction.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "libwebsockets.h"


static int callback_http(struct libwebsocket_context * this,
		struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
		void *user, void *in, size_t len) {
	return 0;
}

void send_message();

static int callback_time_signal(struct libwebsocket_context * this,
		struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
		void *user, void *in, size_t len) {
	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
		printf("LWS_CALLBACK_ESTABLISHED\n");
		break;
	case LWS_CALLBACK_PROTOCOL_DESTROY:
		printf("LWS_CALLBACK_PROTOCOL_DESTROY\n");
	  	break;
	case LWS_CALLBACK_SERVER_WRITEABLE: {
		printf("LWS_CALLBACK_SERVER_WRITEABLE\n");
		time_t atime;
		time_t rtime = time( &atime );
		char * msg;
		msg = ctime( &rtime );
		printf( "time signal: %s" , msg );/* chrtimeの末尾は\n\0 */
		const size_t length = strlen(msg);
		unsigned char buf[
			LWS_SEND_BUFFER_PRE_PADDING + length + LWS_SEND_BUFFER_POST_PADDING
		];
		sprintf(&buf[LWS_SEND_BUFFER_PRE_PADDING], "%s", msg);
		printf("msg: %s\n", buf + LWS_SEND_BUFFER_PRE_PADDING);
		libwebsocket_write(
			wsi,
			&buf[LWS_SEND_BUFFER_PRE_PADDING],
			length,
			LWS_WRITE_TEXT
		);

		break;
	}
	default:
		printf("LWS_(%d)\n", reason);
		break;
	}

	return 0;
}

static struct libwebsocket_protocols protocols[] = {
		/* first protocol must always be HTTP handler */
	{ "http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{ "my-protocol",
		callback_time_signal,
		0
	},
	{
		NULL, NULL, 0 /* End of list */
	}
};

static void on_timer() {
	static unsigned long msec_cnt = 0;
	printf("signal: %lu\n", msec_cnt++);
	libwebsocket_callback_on_writable_all_protocol(&protocols[1]);
	return;
}
static void start_echo_timer() {
	struct sigaction action;
	struct itimerval timer;

	memset(&action, 0, sizeof(action));

	/* set signal handler */
	action.sa_handler = on_timer;
	action.sa_flags = SA_RESTART;
	sigemptyset(&action.sa_mask);
	if(sigaction(SIGALRM, &action, NULL) < 0){
		perror("sigaction error");
		exit(1);
	}

	/* set intarval timer (10ms) */
	timer.it_value.tv_sec = 3;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 3;
	timer.it_interval.tv_usec = 0;
	if(setitimer(ITIMER_REAL, &timer, NULL) < 0){
		perror("setitimer error");
		exit(1);
	}
}

int main(void) {
	int port = 7681;
	const char *interface = NULL;
	struct libwebsocket_context *context;
	int opts = 0;

	struct lws_context_creation_info info;

	memset(&info, 0, sizeof info);
	info.port = port;
	info.iface = interface;
	info.protocols = protocols;
	info.extensions = libwebsocket_get_internal_extensions();
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		return -1;
	}

	printf("starting server...\n");

	start_echo_timer();

	while (1) {
		libwebsocket_service(context, 200);
	}

	libwebsocket_context_destroy(context);

	return 0;
}
