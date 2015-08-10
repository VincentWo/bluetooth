#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <iostream>
#include <memory>

using std::unique_ptr;
using std::cout;

int main(int argc, char** argv)
{
	char addr[19]  = { 0 };
	char name[248] = { 0 };

	auto dev_id = hci_get_route(NULL);
	auto sock = hci_open_dev(dev_id);
	if(dev_id < 0 || sock < 0)
	{
		perror("opening socket");
		exit(1);
	}

	auto len = 8;
	auto max_rsp = 255;
	auto flags = IREQ_CACHE_FLUSH;
	auto ii = new inquiry_info[max_rsp];

	auto num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if(num_rsp < 0)
	{
		perror("hci_inquiry");
	}

	for(auto i = 0; i < num_rsp; ++i)
	{
		ba2str(&ii[i].bdaddr, addr);
		memset(name, 0, sizeof(name)); 
		if(hci_read_remote_name(sock, &ii[i].bdaddr, sizeof(name), name, 0) < 0)
		{
			strcpy(name, "[unknown]");
		}
		cout << addr << ": " << name << '\n';
	}

	free( ii );
	close( sock );
	return 0;
}
