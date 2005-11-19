#include "net.h"

void NET::Init()
{
	SDLNet_Init();
}

NET::NET()
{
	server = false;
	pin = pout = NULL;
	remote_socket = NULL;
	datacounter_rx = 0;
	datacounter_tx = 0;
}

NET::~NET()
{
	Disconnect();
	SDLNet_Quit();
}

bool NET::Connect(string host, int port)
{
	server = false;
	bool err = false;
	
	if (host == "Server")
		server = true;
	
	if (remote_socket != NULL)
	{
		SDLNet_UDP_Close(remote_socket);
		remote_socket = NULL;
	}
	
	if (server)
		remote_socket = SDLNet_UDP_Open(port);
	else
		remote_socket = SDLNet_UDP_Open(0);
	
	if (!server)
	{
		SDLNet_ResolveHost(&remote_ip, host.c_str(), port);
		
		SDLNet_UDP_Unbind(remote_socket, 0);
		if (SDLNet_UDP_Bind(remote_socket, 0, &remote_ip)==-1)
		{
			err = true;
			
			if (NET_DEBUG)
			{
				cout << "net:  couldn't bind UDP socket to host " << host << " port " << port << endl;
				printf("SDLNet_UDP_Bind: %s\n",SDLNet_GetError());
			}
		}
	}
	
	//try a handshake
	if (pout == NULL)
		pout = SDLNet_AllocPacket(MAX_PACKET_SIZE);
	if (pin == NULL)
		pin = SDLNet_AllocPacket(MAX_PACKET_SIZE);

	if (server)
	{
	}
	else
	{
		err = ClientHandshake();
	}
	
	return !err;
}

void NET::Update()
{
	if (remote_socket == NULL)
		return;
	
	int retval = SDLNet_UDP_Recv(remote_socket, pin);
	
	if (retval >= 1 && connected)
	{
		BufferPacket(pin);
	}
	
	if (retval >= 1 && !connected)
	{
		ServerHandshake();
	}
}

bool NET::Connected()
{
	/*if (remote_socket != NULL)
		return true;
	else
		return false;*/
	
	return connected;
}

void NET::Disconnect()
{
	if (remote_socket != NULL)
	{
		SDLNet_UDP_Close(remote_socket);
		remote_socket = NULL;
		connected = false;
	}
}

bool NET::IsEqualToPacket(UDPpacket * p, Uint8 control, void * data, int size)
{
	Uint8 * sourcedata = (Uint8 *) data;
	
	if (p == NULL)
	{
		if (NET_DEBUG)
		{
			cout << "net:  IsEqualToPacket:  invalid packet" << endl;
			return false;
		}
	}
	
	if (size + 2 >= p->maxlen)
	{
		if (NET_DEBUG)
		{
			cout << "net:  IsEqualToPacket:  packet size is too large:  " << size << endl;
			return false;
		}
	}
	
	if (p->len != size + 2)
		return false;
	
	if (p->data[0] != replay.Get_FuncNetControl())
		return false;
	
	if (p->data[1] != control)
		return false;
	
	int i;
	for (i = 0; i < size; i++)
	{
		if (p->data[i+2] != sourcedata[i])
			return false;
	}
	
	return true;
}

void NET::WriteToPacket(UDPpacket * p, Uint8 control, void * data, int size)
{
	Uint8 * sourcedata = (Uint8 *) data;
	
	if (p == NULL)
	{
		if (NET_DEBUG)
		{
			cout << "net:  WriteToPacket:  invalid packet" << endl;
			return;
		}
	}
	
	if (size + 2 >= p->maxlen)
	{
		if (NET_DEBUG)
		{
			cout << "net:  WriteToPacket:  packet size is too large:  " << size << endl;
			return;
		}
	}
	
	p->len = size + 2;
	
	p->data[0] = replay.Get_FuncNetControl();
	p->data[1] = control;
	
	int i;
	for (i = 0; i < size; i++)
	{
		p->data[i + 2] = sourcedata[i];
	}
}

int NET::UDPSend(UDPpacket * p, int channel)
{
	datacounter_tx += p->len;
	
	if (!SDLNet_UDP_Send(remote_socket, channel, p))
		return UDP_FAILURE;
	else
		return UDP_SUCCESS;
}

int NET::UDPRecv(UDPpacket * p, int channel, int timeout)
{
	Uint32 t, t2;
	
	t = SDL_GetTicks();
	t2 = SDL_GetTicks();
	
	bool success = false;
	
	while (!success && t2 - t < (Uint32) timeout)
	{
		int retval = SDLNet_UDP_Recv(remote_socket, p);
		if (retval >= 1)
		{
			success = true;
		}
		
		t2=SDL_GetTicks();
	}
	
	if (success)
	{
		datacounter_rx += p->len;
		return UDP_SUCCESS;
	}
	
	if (t2 - t >= (Uint32) timeout)
	{
		return UDP_TIMEOUT;
	}
	
	return UDP_FAILURE;
}

bool NET::ClientHandshake()
{
	string handshake = HANDSHAKESTR;
	string handshake_reply = HANDSHAKEREPLYSTR;
	int timeout = HANDSHAKETIMEOUT;
	
	bool err = false;
	
	WriteToPacket(pout, CONTROL_HANDSHAKE, (void *) handshake.c_str(), handshake.length());
	UDPSend(pout, 0);
	int ret = UDPRecv(pin, 0, timeout);
	if (ret == UDP_SUCCESS)
	{
		if (IsEqualToPacket(pin, CONTROL_HANDSHAKE, (void *) handshake_reply.c_str(), handshake_reply.length()))
		{
			connected = true;
			if (NET_DEBUG)
				cout << "net:  Connection complete." << endl;
		}
		else 
		{
			if (NET_DEBUG)
				cout << "net:  Invalid handshake reply." << endl;
			err = true;
		}
	}
	if (ret == UDP_TIMEOUT)
	{
		err = true;
		if (NET_DEBUG)
				cout << "net:  Handshake timeout, couldn't connect" << endl;
	}
	if (ret == UDP_FAILURE)
	{
		err = true;
		if (NET_DEBUG)
				cout << "net:  Handshake error." << endl;
	}
	
	return err;
}

//this function assumes pin already has an incoming packet from the client
bool NET::ServerHandshake()
{
	string handshake = HANDSHAKESTR;
	string handshake_reply = HANDSHAKEREPLYSTR;
//	int timeout = HANDSHAKETIMEOUT;
	
	bool err = false;
	
	if (IsEqualToPacket(pin, CONTROL_HANDSHAKE, (void *) handshake.c_str(), handshake.length()))
	{
		SDLNet_UDP_Unbind(remote_socket, 0);
		if (SDLNet_UDP_Bind(remote_socket, 0, &(pin->address))==-1)
		{
			err = true;
			
			if (NET_DEBUG)
			{
				cout << "net:  couldn't bind UDP socket to host " << SDLNet_ResolveIP(&(pin->address)) << endl;
				printf("SDLNet_UDP_Bind: %s\n",SDLNet_GetError());
			}
		}
		WriteToPacket(pout, CONTROL_HANDSHAKE, (void *) handshake_reply.c_str(), handshake_reply.length());
		UDPSend(pout, 0);
		connected = true;
		if (NET_DEBUG)
			cout << "net:  incoming connection established" << endl;
	}
	else 
	{
		if (NET_DEBUG)
			cout << "net:  Invalid handshake from " << SDLNet_ResolveIP(&(pin->address)) << endl;
		err = true;
	}
	
	return err;
}

void NET::Send(void * data, int size)
{
	Uint8 * sourcedata = (Uint8 *) data;
	
	if (pout == NULL)
	{
		if (NET_DEBUG)
		{
			cout << "net:  Send:  invalid packet" << endl;
			return;
		}
	}
	
	if (size >= pout->maxlen)
	{
		if (NET_DEBUG)
		{
			cout << "net:  Send:  packet size is too large:  " << size << endl;
			return;
		}
	}
	
	pout->len = size;
	
	int i;
	for (i = 0; i < size; i++)
	{
		pout->data[i] = sourcedata[i];
	}
	
	UDPSend(pout, 0);
}

int NET::RecvBlock(void * dest, int destsize, int timeout)
{
	int r = UDPRecv(pin, 0, timeout);
	
	Uint8 * destdata = (Uint8 *) dest;
	
	if (r == UDP_SUCCESS)
	{
		int i;
		int mins = pin->len;
		if (destsize < mins)
		{
			mins = destsize;
			if (NET_DEBUG)
				cout << "net:  RecvBlock:  overflow (" << pin->len << ">" << destsize << endl;
		}
		for (i = 0; i < mins; i++)
		{
			destdata[i] = pin->data[i];
		}
		return mins;
	}
	else
		return r;
}

void PBUFFER::Put(UDPpacket * newp)
{
	valid = true;
	
	if (data == NULL)
		data = new Uint8 [MAX_PACKET_SIZE];
	
	int i;
	int mins = newp->len;
	if (MAX_PACKET_SIZE < mins)
		mins = MAX_PACKET_SIZE;
	for (i = 0; i < mins; i++)
	{
		data[i] = newp->data[i];
	}
	
	len = mins;
	
	/*packet->channel = newp->channel;
	packet->len = newp->len;
	packet->status = newp->status;
	packet->address = newp->address;*/
}

int NET::BufferPacket(UDPpacket * newp)
{
	datacounter_rx += newp->len;
	
	int idx = -1;
	int i;
	//find a free buffer slot
	for (i = 0; idx < 0 && i < PACKET_BUFFER_LEN; i++)
	{
		if (!buffer[i].Valid())
			idx = i;
	}
	
	if (idx >= 0)
	{
		buffer[idx].Put(newp);
	}
	else
	{
		if (NET_DEBUG)
		{
			cout << "net:  BufferPacket:  packet buffer is full, dropping packet!" << endl;
		}
	}
	
	return idx;
}

int NET::NumBufferedPackets()
{
	int count = 0;
	int i;
	for (i = 0; i < PACKET_BUFFER_LEN; i++)
		if (buffer[i].Valid())
			count++;
		
	return count;
}
