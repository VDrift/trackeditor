#include "messageq.h"

MESSAGEQ::MESSAGEQ()
{
	Clear();
	posx = 0;
	posy = 0;
	size = 6;
	fset = 1;
	qpersist = 1.0f;
	printtime = false;
	buildup = true;
}

extern bool verbose_output;
MESSAGEQ::~MESSAGEQ()
{
	if (verbose_output)
		cout << "message queue deinit" << endl;
	
	Clear();
}

void MESSAGEQ::Clear()
{
	for (int i = 0; i < MAX_Q_DEPTH; i++)
	{
		valid[i] = false;
		qlen[i] = -1;
	}
}

void MESSAGEQ::SetDepth(int newqdepth)
{
	qdepth = newqdepth;
	Clear();
}

void MESSAGEQ::SetPersist(float newqpersist)
{
	qpersist = newqpersist;
}

void MESSAGEQ::SetBuildUp(bool newbuildup)
{
	buildup = newbuildup;
}

void MESSAGEQ::SetPos(float newx, float newy, int newsize, int newfset)
{
	posx = newx;
	posy = newy;
	size = newsize;
	fset = newfset;
}

void MESSAGEQ::AddMessage(string newq)
{
	int foundopen = -1;
	int i;
	
	int enforcer = 0;
	
	//first look for an open slot
	for (i = 0; i < MAX_Q_DEPTH; i++)
	{
		if (valid[i])
			enforcer++;
		
		if (!valid[i])
			foundopen = i;
	}
	
	//okay, now look for the least recent slot
	if (foundopen < 0 || enforcer + 1 > qdepth)
	{
		float minlen = 10000.0;
		foundopen = 0;
		
		for (i = 0; i < MAX_Q_DEPTH; i++)
		{
			if (valid[i] && qlen[i] < minlen)
			{
				foundopen = i;
				minlen = qlen[i];
			}
		}
	}

	//so, now foundopen is garunteed to have a reasonable value

	q[foundopen] = newq;
	qlen[foundopen] = qpersist;	
	valid[foundopen] = true;
}

void MESSAGEQ::Draw(float timefactor, float fps, FONT & font)
{
	int i;

	
	//***update queue
	
	for (i = 0; i < MAX_Q_DEPTH; i++)
	{
		//subtract frame length
		if (valid[i])
			qlen[i] -= timefactor/fps;
		
		//remove expired messages
		if (qlen[i] < 0)
			valid[i] = false;
	}
	
	
	//***sort the queue
	
	int sortnum = 0;
	string sortq[MAX_Q_DEPTH];
	float sortlen[MAX_Q_DEPTH];
	
	bool tempvalid[MAX_Q_DEPTH];
	
	//populate temp vars
	for (i = 0; i < MAX_Q_DEPTH; i++)
	{
		tempvalid[i] = valid[i];
	}
	
	//do the sorting with my kewl VENZALGORITHM
	if (buildup)
	{
		int m;
		for (i = 0; i < MAX_Q_DEPTH; i++)
		{
			float maxval = 0.0f;
			int maxindex = -1;
			
			for (m = 0; m < MAX_Q_DEPTH; m++)
			{
				if (tempvalid[m] && qlen[m] > maxval)
				{
					maxval = qlen[m];
					//tempvalid[m] = false;
					maxindex = m;
				}
			}
			
			if (maxindex < 0)
				break;
			
			sortlen[sortnum] = qlen[maxindex];
			
			if (printtime)
			{
				char tempchar[1024];
				sprintf(tempchar, "%.1f) %s", sortlen[sortnum], q[maxindex].c_str());
				sortq[sortnum] = tempchar;
			}
			else
			{
				sortq[sortnum] = q[maxindex];
			}
			tempvalid[maxindex] = false;
			sortnum++;
		}
	}
	else
	{
		int m;
		for (i = 0; i < MAX_Q_DEPTH; i++)
		{
			float minval = 1000.0f;
			int minindex = -1;
			
			for (m = 0; m < MAX_Q_DEPTH; m++)
			{
				if (tempvalid[m] && qlen[m] < minval)
				{
					minval = qlen[m];
					//tempvalid[m] = false;
					minindex = m;
				}
			}
			
			if (minindex < 0)
				break;
			
			sortlen[sortnum] = qlen[minindex];
			
			if (printtime)
			{
				char tempchar[1024];
				sprintf(tempchar, "%.1f) %s", sortlen[sortnum], q[minindex].c_str());
				sortq[sortnum] = tempchar;
			}
			else
			{
				sortq[sortnum] = q[minindex];
			}
			tempvalid[minindex] = false;
			sortnum++;
		}
	}
	
	
	
	
	
	//***print sorted queue
	
	string strout;
	string finalstr;
	strout = "";
	
	for (i = 0; i < sortnum; i++)
	{
		finalstr = strout;
		
		float white;
		float redtime = 8.0;
		if (sortlen[i] > ((redtime-1.0)*qpersist)/redtime)
			white = 1.0-(sortlen[i]-((redtime-1.0)*qpersist/redtime))/(qpersist/redtime);
		else
			white = 1.0;
		
		float trans;
		if (sortlen[i] < qpersist/4.0)
			trans = sortlen[i]/(qpersist/4.0);
		else
			trans = 1.0;
		
		finalstr.append(sortq[i]);
		font.Print(posx,posy,finalstr.c_str(),fset,size,1,white,white,trans);
		strout.append("\n");
	}
}

void MESSAGEQ::SetTimePrint(bool newprinttime)
{
	printtime = newprinttime;
}
