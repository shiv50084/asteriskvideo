/* H324M library
 *
 * Copyright (C) 2006 Sergio Garcia Murillo
 *
 * sergio.garcia@fontventa.com
 * http://sip.fontventa.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "H324MAL2.h"
#include "crc8.h"

/****************** Receiver **************/
H223AL2Receiver::H223AL2Receiver(int useSequenceNumbers,H223SDUListener* listener)
{
	//Set sn parameter
	useSN = useSequenceNumbers;
	//Save listener
	sduListener = listener;
}

H223AL2Receiver::~H223AL2Receiver()
{

}

void H223AL2Receiver::Send(BYTE b)
{
	//Enque in sdu
	sdu.Push(b);
}

void H223AL2Receiver::SendClosingFlag()
{
	Debug("-AL2 Frame received\n");

	//Crc
	CRC8 crc;

	//Data
	BYTE *data;
	int dataLen;

	//Check minimum size
	if (sdu.Length()<2+useSN)
		goto clean;

	//Get data
	data = sdu.GetPointer();
	dataLen = sdu.Length();

	//Set data
	crc.Add(data,dataLen-1);

	Debug("-AL2 Frame [%x,%x,%x,%d]\n",data[dataLen-1],crc.Calc(),this,dataLen);

	{
		char name[256];
		sprintf(name,"/tmp/media_%x.raw",(unsigned int)this);
		int fd = open(name,O_CREAT|O_WRONLY|O_APPEND);
		write(fd,data+useSN,dataLen-useSN-1);
		close(fd);
	}
	
	//Calc
	if (data[dataLen-1]!=crc.Calc())
		goto clean;

	//Send to listener
	sduListener->OnSDU(data+useSN,dataLen-useSN-1);

//Clean SDU and exit
clean:
	sdu.Clean();
}

/****************** Sender **************/
H223AL2Sender::H223AL2Sender(int useSequenceNumbers)
{
	//Set sn parameter
	useSN = useSequenceNumbers;
	sn = 0;
}

H223AL2Sender::~H223AL2Sender()
{
}

H223MuxSDU* H223AL2Sender::GetNextPDU()
{
	//Check frame list
	if (frameList.size()==0)
		return NULL;

	//return first element
	return frameList.front();
}

void H223AL2Sender::OnPDUCompleted()
{
	//Get sdu
	H223MuxSDU *sdu = frameList.front();

	//Remove
	frameList.pop_front();

	//delete frame
	delete sdu;
}

int H223AL2Sender::SendPDU(BYTE *buffer,int len)
{
	//Crc
	CRC8 crc;

	//Build SDU
	H223MuxSDU *sdu = new H223MuxSDU();

	//If we have sn
	if (useSN)
		//Append
		sdu->Push(sn++);

	//Set buffer
	sdu->Push(buffer,len);

	//Calc crc
	crc.Add(sdu->GetPointer(),sdu->Length());

	//Append crc
	sdu->Push(crc.Calc());

	//Enque sdu
	frameList.push_back(sdu);

	//exit
	return true;
}

