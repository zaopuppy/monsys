
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <string.h>

ngx_module_t ngx_http_monsys_module;

static void* ngx_http_monsys_create_loc_conf(ngx_conf_t *ngx_conf);
static char* ngx_http_monsys_merge_loc_conf(ngx_conf_t *ngx_conf, void *parent, void *child);
static char* ngx_http_monsys(ngx_conf_t *ngx_conf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_monsys_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_monsys_create_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_monsys_reinit_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_monsys_process_header(ngx_http_request_t *r);
static void ngx_http_monsys_abort_request(ngx_http_request_t *r);
static void ngx_http_monsys_finalize_request(ngx_http_request_t *r, ngx_int_t rc);

/////////////////////////////////////////////////////////////////////
// configuration
typedef struct {
	ngx_http_upstream_conf_t       upstream;
} ngx_http_monsys_conf_t;

/////////////////////////////////////////////////////////////////////
// command

static ngx_command_t ngx_http_monsys_commands[] = {
	{ ngx_string("monsys"),
		NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
		ngx_http_monsys,
		NGX_HTTP_LOC_CONF_OFFSET,
		0,
		NULL },

	ngx_null_command
};

static char*
ngx_http_monsys(ngx_conf_t *ngx_conf, ngx_command_t *cmd, void *conf)
{
	ngx_http_monsys_conf_t *monsys_conf = conf;
	ngx_http_core_loc_conf_t *core_loc_conf;

	core_loc_conf = ngx_http_conf_get_module_loc_conf(ngx_conf, ngx_http_core_module);

	// POINT 1:
	core_loc_conf->handler = ngx_http_monsys_handler;

	ngx_str_t url = ngx_string("127.0.0.1:1983");

	ngx_url_t u;

	ngx_memzero(&u, sizeof(ngx_url_t));

	u.url.len = url.len;
	u.url.data = url.data;
	u.default_port = 1983;
	u.uri_part = 1;
	u.no_resolve = 1;

	monsys_conf->upstream.upstream = ngx_http_upstream_add(ngx_conf, &u, 0);
	if (monsys_conf->upstream.upstream == NULL) {
		return NGX_CONF_ERROR;
	}

	return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_monsys_handler(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_handler()");

	ngx_http_monsys_conf_t *monsys_conf =
		ngx_http_get_module_loc_conf(r, ngx_http_monsys_module);

	/* check method */
	if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_POST))) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
				"GET/POST method only.");
		return NGX_HTTP_NOT_ALLOWED;
	}

	// // /* discard request body */
	// ngx_int_t rc = ngx_http_discard_request_body(r);
	// if (rc != NGX_OK) {
	// 	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
	// 			"failed to invoke ngx_http_discard_request_body()");
	// 	return rc;
	// }

	if (ngx_http_set_content_type(r) != NGX_OK) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
				"failed to invoke ngx_http_set_content_type()");
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}

	if (ngx_http_upstream_create(r) != NGX_OK) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
				"failed to invoke ngx_http_upstream_create()");
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}

	ngx_http_upstream_t *u = r->upstream;
	// ngx_http_upstream_t *u = ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_t));
	if (u == NULL) {
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}

	ngx_str_set(&u->schema, "monsys://");

	u->peer.log = r->connection->log;
	u->peer.log_error = NGX_ERROR_ERR;

	u->output.tag = (ngx_buf_tag_t) &ngx_http_monsys_module;

	// XXX where does conf->upstream come from?
	u->conf = &(monsys_conf->upstream);

	// attach the callback functions
	u->create_request = ngx_http_monsys_create_request;
	u->reinit_request = ngx_http_monsys_reinit_request;
	u->process_header = ngx_http_monsys_process_header;
	u->abort_request = ngx_http_monsys_abort_request;
	u->finalize_request = ngx_http_monsys_finalize_request;

	// use our filter instead of standard http filter
	// u->input_filter_init = ngx_http_monsys_filter_init;
	// u->input_filter = ngx_http_monsys_filter;
	// u->input_filter_ctx = ctx;

	// already there
	// r->upstream = u;

	// http://tengine.taobao.org/book/chapter_05.html
	// r->main->count++;

	// ngx_http_upstream_init(r);
	ngx_int_t rv = ngx_http_read_client_request_body(r, ngx_http_upstream_init);
	if (rv >= NGX_HTTP_SPECIAL_RESPONSE) {
		return rv;
	}

	return NGX_DONE;
}

static ngx_int_t
ngx_http_monsys_create_request(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_create_request()");

	// r->upstream->request_bufs = r->request_body->bufs;
	return NGX_OK;
}

static ngx_int_t
ngx_http_monsys_reinit_request(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_reinit_request()");
	return NGX_OK;
}

static ngx_int_t
ngx_http_monsys_process_header(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_process_header()");

	ngx_http_upstream_t *u = r->upstream;

	u->headers_in.status_n = 200;
	u->state->status = 200;
	u->headers_in.content_length_n = u->buffer.last - u->buffer.pos;

	return NGX_OK;
}

static void
ngx_http_monsys_abort_request(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_abort_request()");

	return;
}

static void
ngx_http_monsys_finalize_request(ngx_http_request_t *r, ngx_int_t rc)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_monsys_finalize_request()");

	return;
}

// static ngx_int_t
// ngx_http_monsys_filter_init(void *data)
// {
// 	// ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
// 	// 		"ngx_http_monsys_filter_init()");
// 	return NGX_OK;
// }
// 
// static ngx_int_t
// ngx_http_monsys_filter(void *data, ssize_t bytes)
// {
// 	// ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
// 	// 		"ngx_http_monsys_filter()");
// 	return NGX_OK;
// }

/////////////////////////////////////////////////////////////////////
// context
static ngx_http_module_t ngx_http_monsys_module_ctx = {
	NULL,		// pre-configuration
	NULL,		// post-configuration

	NULL,		// create main configuration
	NULL,		// init main configuration

	NULL,		// create server configuration
	NULL,		// merge server configuration

	ngx_http_monsys_create_loc_conf,	// create location configuration
	ngx_http_monsys_merge_loc_conf,	// merge location configuration
};

static void*
ngx_http_monsys_create_loc_conf(ngx_conf_t *ngx_conf)
{
	ngx_http_monsys_conf_t *conf;

	conf = ngx_pcalloc(ngx_conf->pool, sizeof(ngx_http_monsys_conf_t));
	if (conf == NULL) {
		return NGX_CONF_ERROR;
	}

	conf->upstream.connect_timeout = NGX_CONF_UNSET_MSEC;
	conf->upstream.send_timeout = NGX_CONF_UNSET_MSEC;
	conf->upstream.read_timeout = NGX_CONF_UNSET_MSEC;
	conf->upstream.buffer_size = NGX_CONF_UNSET_SIZE;

	return conf;
}

static char*
ngx_http_monsys_merge_loc_conf(ngx_conf_t *ngx_conf, void *parent, void *child)
{
	ngx_http_monsys_conf_t *prev = parent;
	ngx_http_monsys_conf_t *conf = child;

	// --- BEGIN ---
	// very important T_T
	ngx_conf_merge_msec_value(
			conf->upstream.connect_timeout,
			prev->upstream.connect_timeout, 60000);

	ngx_conf_merge_msec_value(
			conf->upstream.send_timeout,
			prev->upstream.send_timeout, 60000);

	ngx_conf_merge_msec_value(
			conf->upstream.read_timeout,
			prev->upstream.read_timeout, 60000);

	ngx_conf_merge_size_value(
			conf->upstream.buffer_size,
			prev->upstream.buffer_size, (size_t) ngx_pagesize);
	// ---  END  ---

	// ngx_conf_merge_uint_value(conf->min_radius, prev->min_radius, 10);
	// if (conf->min_radius < 1) {
	//   ngx_conf_log_error(NGX_LOG_EMERG, ngx_conf, 0,
	//     "min_radius must be equal or more than 1");
	//   return NGX_CONF_ERROR;
	// }

	// u->buffer_size = 1 << 10;
	return NGX_CONF_OK;
}

/////////////////////////////////////////////////////////////////////
// module definition

ngx_module_t ngx_http_monsys_module = {
	NGX_MODULE_V1,
	&ngx_http_monsys_module_ctx,	// module context
	ngx_http_monsys_commands,		// module directives
	NGX_HTTP_MODULE,							// module type
	NULL,													// init master
	NULL,													// init module
	NULL,													// init process
	NULL,													// init thread
	NULL,													// exit thread
	NULL,													// exit process
	NULL,													// exit master
	NGX_MODULE_V1_PADDING
};

// upstream.store_access
// upstream.ignore_client_abort
// upstream.local
// upstream.connect_timeout
// upstream.send_timeout
// upstream.send_lowat
// upstream.buffergsize
// upstream.pass_request_headers
// upstream.pass_request_body
// upstream.intercept_errors
// upstream.read_timeout
// upstream.bufs
// upstream.busy_buffers_size_conf
// upstream.cache_bypass
// upstream.no_cache
// upstream.cache_valid
// upstream.cache_min_uses
// upstream.cache_use_stale
// upstream.cache_methods
// upstream.cache_lock
// upstream.cache_lock_timeout
// upstream.temp_path
// upstream.max_temp_file_size_conf
// upstream.temp_file_write_size_conf
// upstream.next_upstream
// upstream.pass_headers
// upstream.hide_headers
// upstream.ignore_headers


