#include <rdma.h>
int msg_size;
std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point end;
int main (int argc, char *argv[]) { 
	struct resources res;
	int rc = 1;
	int trans_times;
	char temp_char;
	
	char* p1 = new char[msg_size];
	char* p2 = new char[msg_size];
	memset(p1, 0, msg_size);
	memset(p2, 1, msg_size);
	start = std::chrono::steady_clock::now();
	memcpy(p2, p1, msg_size);
	end = std::chrono::steady_clock::now();
	std::cout << "memory copy Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
		<< " ns" << std::endl;
	
	/* parse the command line parameters */
	while (1)
	{
		int c;
		//Options  opts(*argv, optv);
		//OptArgvIter  iter(--argc, ++argv);
		//const char* optarg, * str = NULL;
		//int  errors = 0, msg_size = 0;
		//const char* optv[] = {
		//   "s:size <number>",
		//   NULL
		//		};
		//while (char optchar = opts(iter, optarg)) {
		//	switch (optchar) {
		//	case 's':
		//		msg_size = optarg; break;
		//	default:  ++errors; break;
		//	} //switch
		//}
		struct option long_options[] = {
			{name: "msg_size", has_arg : 1, flag : nullptr, val : 's'},
			//{name: "transmit times", has_arg : 1, flag : nullptr, val : 'n'},
			{name: nullptr, has_arg : 0, flag : nullptr, val : '\0'}
		};
		c = getopt_long(argc, argv, "s:", long_options, nullptr);
		if (c == -1)
			break;
		switch (c)
		{
		//case 's':
		//	msg_size = std::stoi(optarg); 
		//	break;
		case 'n':
			trans_times = std::stoi(optarg);
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
	/*
	start = std::chrono::steady_clock::now();
	sock_sync_data(res.sock, msg_size, p1, p2);
	end = std::chrono::steady_clock::now();
	std::cout << "socket Elapsed time in nanoseconds :"
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
		<< " ns" << std::endl;*/
	/* connect the QPs */
	if (connect_qp(&res))
	{
		fprintf(stderr, "failed to connect QPs\n");
		goto main_exit;
	}
	/* let the server post the sr */
	
	if (!config.server_name) {
		

		if (post_send(&res, IBV_WR_SEND))
		{
			fprintf(stderr, "failed to post sr\n");
			goto main_exit;
		}

	}
		//void* = start;
		 
	/* in both sides we expect to get a completion */
	//void* end;
	
	if (poll_completion(&res))
	{	
		
		fprintf(stderr, "poll completion failed\n");
		goto main_exit;
	}
	
	/* after polling the completion we have the message in the client buffer too */
	if (config.server_name)
		fprintf(stdout, "Message is: \n");
	else
	{	
		std::cout << "SEND Elapsed time in nanoseconds :"
			<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
			<< " ns" << std::endl;
		/* setup server buffer with read message */
		//strcpy(res.buf, RDMAMSGR);
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
		
		for (int i = 0; i < trans_times; i++) {
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
		}
		
		
		//fprintf(stdout, "Contents of server's buffer: '%s'\n", res.buf);
		std::cout << trans_times <<"times RDMA READ Elapsed time in nanoseconds :"
			<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
			<< " ns" << std::endl;

		/* Now we replace what's in the server's buffer */
		//strcpy(res.buf, RDMAMSGW);
		//fprintf(stdout, "Now replacing it with: '%s'\n", res.buf);
		
		for (int i = 0; i < trans_times; i++) {
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

		std::cout << trans_times <<"time RDMA WRITE Elapsed time in nanoseconds :"
			<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
			<< " ns" << std::endl;
	}
	/* Sync so server will know that client is done mucking with its memory */
	if (sock_sync_data(res.sock, 1, "W", &temp_char)) /* just send a dummy char back and forth */
	{
		fprintf(stderr, "sync error after RDMA ops\n");
		rc = 1;
		goto main_exit;
	}
	/*if (!config.server_name)
		fprintf(stdout, "Contents of server buffer: '%s'\n", res.buf);*/
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