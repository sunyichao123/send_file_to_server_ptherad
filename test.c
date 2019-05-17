#include "mongoose/mongoose.h"
void ev_handler(struct mg_connection *c, int ev, void* ev_data);

struct mg_connection *my_bind(struct mg_mgr *mgr, mg_event_handler_t handler);
void ev_handler(struct mg_connection *c, int ev, void* ev_data)
{
	switch(ev){
		case MG_EV_TIMER:
			printf("i am handsome");
			double next = mg_set_timer(c, 0) +1;
			mg_set_timer(c, mg_time() + next);
			break;
	}
	return;
}

struct mg_connection *my_bind(struct mg_mgr *mgr, mg_event_handler_t callback)
{
	struct mg_connect_opts opts;
	struct mg_connection *conn = NULL;
	printf("my_bind\n");
	if ((conn = (struct mg_connection *) calloc(1, sizeof(*conn))) != NULL){
		conn->handler = callback;
		conn->mgr = mgr;
		conn->last_io_time = (time_t) mg_time();
		conn->iface =(opts.iface != NULL ? opts.iface : mgr->ifaces[MG_MAIN_IFACE]);
		printf("%ld\n", conn->last_io_time);	
	}

	return conn;

}


int main()
{
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);
	struct mg_connection *c;

	c = my_bind(&mgr, ev_handler);
	printf("111\n");
	if(c == NULL)
	{
		fprintf(stderr, "bind failure");
		exit(1);
	}
	mg_set_timer(c, mg_time() + 1);
	printf("222\n");
	for(;;)
	{
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;

}
