extern "C" {
	#include <rdma.h>
}

int main (int argc, char *argv[]) { 
	struct resources res;
	int rc = 1;
	char temp_char;
	/* parse the command line parameters */
	while (1)
	{
		int c;
		option long_options[] = {
			{name : "port", has_arg : 1, flag : NULL, val : 'p'},
			{name : "ib-dev", has_arg : 1, flag : NULL, val : 'd'},
			{name : "ib-port", has_arg : 1, flag : NULL, val : 'i'},
			{name : "gid-idx", has_arg : 1, flag : NULL, val : 'g'},
			{name : NULL, has_arg : 0, flag : NULL, val : '\0'}
		};
		c = getopt_long(argc, argv, "p:d:i:g:", long_options, NULL);
		if (c == -1)
			break;
		switch (c)
		{
		case 'p':
			config.tcp_port = strtoul(optarg, NULL, 0);
			break;
		case 'd':
			config.dev_name = strdup(optarg);
			break;
		case 'i':
			config.ib_port = strtoul(optarg, NULL, 0);
			if (config.ib_port < 0)
			{
				usage(argv[0]);
				return 1;
			}
			break;
		case 'g':
			config.gid_idx = strtoul(optarg, NULL, 0);
			if (config.gid_idx < 0)
			{
				usage(argv[0]);
				return 1;
			}
			break;
		default:
			usage(argv[0]);
			return 1;
		}
	}
	/* parse the last parameter (if exists) as the server name */
	if (optind == argc - 1)
		config.server_name = argv[optind];
	if (config.server_name) {
		printf("servername=%s\n", config.server_name);
	}
	else if (optind < argc)
	{
		usage(argv[0]);
		return 1;
	}
	/* print the used parameters for info*/
	print_config();
	/* init all of the resources, so cleanup will be easy */
	resources_init(&res);
	/* create resources before using them */
	if (resources_create(&res))
	{
		fprintf(stderr, "failed to create resources\n");
		goto main_exit;
	}
	/* connect the QPs */
	if (connect_qp(&res))
	{
		fprintf(stderr, "failed to connect QPs\n");
		goto main_exit;
	}
	/* let the server post the sr */
	if (!config.server_name)
		if (post_send(&res, IBV_WR_SEND))
		{
			fprintf(stderr, "failed to post sr\n");
			goto main_exit;
		}
	/* in both sides we expect to get a completion */
	if (poll_completion(&res))
	{
		fprintf(stderr, "poll completion failed\n");
		goto main_exit;
	}
	/* after polling the completion we have the message in the client buffer too */
	if (config.server_name)
		fprintf(stdout, "Message is: '%s'\n", res.buf);
	else
	{
		/* setup server buffer with read message */
		strcpy(res.buf, RDMAMSGR);
	}
	/* Sync so we are sure server side has data ready before client tries to read it */
	if (sock_sync_data(res.sock, 1, "R", &temp_char)) /* just send a dummy char back and forth */
	{
		fprintf(stderr, "sync error before RDMA ops\n");
		rc = 1;
		goto main_exit;
	}
	/* Now the client performs an RDMA read and then write on server.
Note that the server has no idea these events have occured */
	if (config.server_name)
	{
		/* First we read contens of server's buffer */
		if (post_send(&res, IBV_WR_RDMA_READ))
		{
			fprintf(stderr, "failed to post SR 2\n");
			rc = 1;
			goto main_exit;
		}
		if (poll_completion(&res))
		{
			fprintf(stderr, "poll completion failed 2\n");
			rc = 1;
			goto main_exit;
		}
		fprintf(stdout, "Contents of server's buffer: '%s'\n", res.buf);
		/* Now we replace what's in the server's buffer */
		strcpy(res.buf, RDMAMSGW);
		fprintf(stdout, "Now replacing it with: '%s'\n", res.buf);
		if (post_send(&res, IBV_WR_RDMA_WRITE))
		{
			fprintf(stderr, "failed to post SR 3\n");
			rc = 1;
			goto main_exit;
		}
		if (poll_completion(&res))
		{
			fprintf(stderr, "poll completion failed 3\n");
			rc = 1;
			goto main_exit;
		}
	}
	/* Sync so server will know that client is done mucking with its memory */
	if (sock_sync_data(res.sock, 1, "W", &temp_char)) /* just send a dummy char back and forth */
	{
		fprintf(stderr, "sync error after RDMA ops\n");
		rc = 1;
		goto main_exit;
	}
	if (!config.server_name)
		fprintf(stdout, "Contents of server buffer: '%s'\n", res.buf);
	rc = 0;
main_exit:
	if (resources_destroy(&res))
	{
		fprintf(stderr, "failed to destroy resources\n");
		rc = 1;
	}
	if (config.dev_name)
		free((char*)config.dev_name);
	fprintf(stdout, "\ntest result is %d\n", rc);
	return rc;
 }