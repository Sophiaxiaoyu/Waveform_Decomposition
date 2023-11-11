// LasFile.cpp: implementation of the CLasFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LasFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CLasFile::CheckLASPublicHeader(LAS_Public_header &public_header)
{
	if (strncmp(public_header.file_signature, "LASF", 4) != 0)
	{
		AfxMessageBox("not las file!");
		return FALSE;
	}
	if ((public_header.version_major != 1) || (public_header.version_minor != 3))
	{
		AfxMessageBox("version is not las1.3!");
		return FALSE;
	}
	if (public_header.header_size != 235)
	{
		AfxMessageBox("header size should be 235!");
		return FALSE;
	}
	if (public_header.offset_to_point_data < public_header.header_size)
	{
		return FALSE;
	}
	if (public_header.number_of_point_records <= 0)
	{
		return FALSE;
	}

	if (public_header.point_data_format>5 )
	{
		AfxMessageBox("point data format ID is wrong!");	
		return FALSE;
	}
	if (public_header.x_scale_factor == 0 || public_header.y_scale_factor == 0 || public_header.z_scale_factor == 0)
	{
		AfxMessageBox("some scale factors are zero!");
		return FALSE;
	}
	if (public_header.max_x < public_header.min_x 
		|| public_header.max_y < public_header.min_y || public_header.max_z < public_header.min_z)
	{
		AfxMessageBox("invalid bounding box!");
		return FALSE;
	}
	return TRUE;
}

void CLasFile::ReadVLRecord()
{
	if (m_ppublic_header==NULL||m_ppublic_header->number_of_variable_length_records<=0)
	{
		return;
	}
	unsigned long i=0,j=0;
	HANDLE hfile=m_hfile;
	unsigned long num_of_waveform_descriptor=0;
	unsigned long number_of_variable_length_records=m_ppublic_header->number_of_variable_length_records;
	SetFilePointer(hfile,sizeof(LAS_Public_header),0,FILE_BEGIN);
	LAS_VLR_header variable_header;
	unsigned long readbytes;
	for (i=0;i<number_of_variable_length_records;i++)
	{
		readbytes = 0;
		ReadFile(hfile,&variable_header,sizeof(LAS_VLR_header),&readbytes,NULL);
		ASSERT(readbytes==sizeof(LAS_VLR_header));
		if(strcmp(variable_header.user_id, "LASF_Spec") == 0 && 
			(variable_header.record_id >=100||variable_header.record_id <356))
		{
			ReadFile(hfile,&m_pwaveform_packect_descriptor[num_of_waveform_descriptor],
				sizeof(LAS_Waveform_Packect_Descriptor),&readbytes,NULL);
			num_of_waveform_descriptor++;
		}
		else
		{
			SetFilePointer(hfile,variable_header.record_length_after_header,0,FILE_CURRENT);
		}
	}
	m_num_of_waveform_descriptor=num_of_waveform_descriptor;
	return;
}
CLasFile::CLasFile()
{
	m_bcolorflag=FALSE;
	m_bgpstimeflag=FALSE;
	m_bwavepacketflag=FALSE;
	m_hfile=INVALID_HANDLE_VALUE;
	m_ppublic_header=NULL;
	m_num_of_waveform_descriptor=0;
	m_pwaveform_packect_descriptor=new LAS_Waveform_Packect_Descriptor[256];
}
void CLasFile::MyClear()
{
	m_bcolorflag=FALSE;
	m_bgpstimeflag=FALSE;
	m_bwavepacketflag=FALSE;
	if(m_hfile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hfile);
		m_hfile= INVALID_HANDLE_VALUE;
	}
	if(m_ppublic_header!=NULL)
	{
		delete m_ppublic_header;
		m_ppublic_header=NULL;
	}
	m_num_of_waveform_descriptor=0;
	memset(m_pwaveform_packect_descriptor,0,sizeof(LAS_Waveform_Packect_Descriptor)*256);
}
CLasFile::~CLasFile()
{
	MyClear();
	if(m_pwaveform_packect_descriptor!=NULL)
	{
		delete[] m_pwaveform_packect_descriptor;
		m_pwaveform_packect_descriptor = NULL;
	}
}
BOOL CLasFile::OpenLasfile(CString pathname)
{
	HANDLE hfile=CreateFile(pathname,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(hfile==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("fail to open the file!");
		return FALSE;
	}

	LAS_Public_header public_header;
	DWORD readbytes;
	if(!(ReadFile(hfile,&public_header,sizeof(LAS_Public_header),&readbytes,NULL))
		||readbytes!=sizeof(LAS_Public_header))
	{
		AfxMessageBox("fail to open the file public header!");
		CloseHandle(hfile);
		return FALSE;
	}

	 // check header contents
	if(!CheckLASPublicHeader(public_header))
	{
		CloseHandle(hfile);
		return FALSE;
	}

	MyClear();	
	m_hfile = hfile;
	m_ppublic_header  = new LAS_Public_header;
	*m_ppublic_header = public_header;

	if(public_header.point_data_format==0) {m_bcolorflag=FALSE;m_bgpstimeflag=FALSE;m_bwavepacketflag=FALSE;}
	if(public_header.point_data_format==1) {m_bcolorflag=FALSE;m_bgpstimeflag=TRUE; m_bwavepacketflag=FALSE;}
	if(public_header.point_data_format==2) {m_bcolorflag=TRUE; m_bgpstimeflag=FALSE;m_bwavepacketflag=FALSE;}
	if(public_header.point_data_format==3) {m_bcolorflag=TRUE; m_bgpstimeflag=TRUE; m_bwavepacketflag=FALSE;}
	if(public_header.point_data_format==4) {m_bcolorflag=FALSE;m_bgpstimeflag=TRUE; m_bwavepacketflag=TRUE;}
	if(public_header.point_data_format==5) {m_bcolorflag=TRUE; m_bgpstimeflag=TRUE; m_bwavepacketflag=TRUE;}

	ReadVLRecord();	
	return TRUE;
}
BOOL CLasFile::GetLASPublicHeaderPointer(LAS_Public_header *&ppublic_header)
{
	if (m_ppublic_header==NULL)
		return FALSE;
	else
	{
		ppublic_header=m_ppublic_header;
		return TRUE;
	}
		
}
BOOL CLasFile::ReadLasPt(LAS_PT *plaspt,__int64 startpt,long num)
{

	HANDLE hfile=m_hfile;
	if(hfile==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	LAS_Public_header *ppublic_header = m_ppublic_header;
	if (startpt<0||startpt+num>m_ppublic_header->number_of_point_records)
	{
		return FALSE;
	}
	__int64 offset=0;
	DWORD readbytes=0;
	offset=ppublic_header->offset_to_point_data+startpt*ppublic_header->point_data_record_length;
	long lDistanceToMove=long(offset&0x00000000ffffffff);
	long lDistanceToMoveHigh=long(offset>>32);
	SetFilePointer(hfile,lDistanceToMove,&lDistanceToMoveHigh,FILE_BEGIN);
	void *pdata=new BYTE[ppublic_header->point_data_record_length*num];
	ReadFile(hfile,pdata,ppublic_header->point_data_record_length*num,&readbytes,NULL);
	memset(plaspt,0,sizeof(LAS_PT)*num);
	if(ppublic_header->point_data_format==0)
	{
		LAS_Point_Format0 *laspt0=(LAS_Point_Format0 *)pdata;
		for (int i=0;i<num;i++)
		{
			memcpy(&(plaspt[i]),laspt0+i,sizeof(LAS_Point_Format0));
		}
	}
	else if (ppublic_header->point_data_format==1)
	{
		LAS_Point_Format1 *laspt1=(LAS_Point_Format1 *)pdata;
		for (int i=0;i<num;i++)
		{
			memcpy(&(plaspt[i]),laspt1+i,sizeof(LAS_Point_Format1));
		}
	}
	else if (ppublic_header->point_data_format==2)
	{
		LAS_Point_Format2 *laspt2=(LAS_Point_Format2 *)pdata;
		for (int i=0;i<num;i++)
		{
			memcpy(&(plaspt[i]),laspt2+i,sizeof(LAS_Point_Format0));
			plaspt[i].red=laspt2[i].red;
			plaspt[i].green=laspt2[i].green;
			plaspt[i].blue=laspt2[i].blue;
		}

	}
	else if (ppublic_header->point_data_format==3)
	{
		LAS_Point_Format3 *laspt3=(LAS_Point_Format3 *)pdata;
		for (int i=0;i<num;i++)
		{
			memcpy(&(plaspt[i]),laspt3+i,sizeof(LAS_Point_Format3));
		}
	}
	else if (ppublic_header->point_data_format==4)
	{
		LAS_Point_Format4 *laspt4=(LAS_Point_Format4 *)pdata;
		for (int i=0;i<num;i++)
		{
			memcpy(&(plaspt[i]),laspt4+i,sizeof(LAS_Point_Format1));
			memcpy(&(plaspt[i].wave_packet_descriptor_index),&(laspt4[i].wave_packet_descriptor_index),29);
		}
	}
	else if (ppublic_header->point_data_format==5)
	{
		LAS_Point_Format5 *laspt5=(LAS_Point_Format5 *)pdata;
		memcpy(plaspt,laspt5,ppublic_header->point_data_record_length*num);
	}
	else
	{
		delete[] pdata;
		pdata = NULL;
		return FALSE;
	}
	delete[] pdata;
	pdata = NULL;
	return TRUE;
}
BOOL CLasFile::ReadLasWavePacket(void *laswavepacket,LAS_PT &laspt)
{
	if(laspt.wave_packet_descriptor_index==0)
	{
		return FALSE;
	}
	HANDLE hfile=m_hfile;
	if(hfile==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BOOL bwavepacketflag=m_bwavepacketflag;
	if(!bwavepacketflag)
	{
		AfxMessageBox("no waveform info!");
		return FALSE;
	}
	LAS_Public_header *ppublic_header=m_ppublic_header;
	if(ppublic_header->global_encoding&0x02)
	{
		DWORD readbytes=0;
		__int64 offset=0;
		__int64 byte_offset_to_waveform_data=laspt.byte_offset_to_waveform_data;
		unsigned long waveform_packet_size_in_bytes=laspt.waveform_packet_size_in_bytes;
		offset=ppublic_header->start_of_waveform_data_packect_record+byte_offset_to_waveform_data;
		long lDistanceToMove=long(offset&0x00000000ffffffff);
		long lDistanceToMoveHigh=long(offset>>32);
		SetFilePointer(hfile,lDistanceToMove,&lDistanceToMoveHigh,FILE_BEGIN);
		ReadFile(hfile,laswavepacket,waveform_packet_size_in_bytes,&readbytes,NULL);
		ASSERT(waveform_packet_size_in_bytes==readbytes);
	}
	else
	{
		AfxMessageBox("now .wdp data is not supported!");
		return FALSE;
	}
	return TRUE;
}
BOOL CLasFile::GetLASWaveformPackectDescriptor(
	 LAS_Waveform_Packect_Descriptor* &pwaveform_packect_descriptor,unsigned char index)
{
	pwaveform_packect_descriptor = NULL;
	int num_of_waveform_descriptor=m_num_of_waveform_descriptor;
	if (index ==0||index>num_of_waveform_descriptor)
	{
		return FALSE;
	}
	pwaveform_packect_descriptor=m_pwaveform_packect_descriptor+(index-1);
	return TRUE;
}
BOOL CLasFile::ParseWavePacket(unsigned long *praw_waveform_amplitude,void *plaswavepacket,
		LAS_Waveform_Packect_Descriptor *pdescriptor)
{
	unsigned char bits_per_sample=pdescriptor->bits_per_sample;
	unsigned char waveform_compression_type=pdescriptor->waveform_compression_type;
	ASSERT(waveform_compression_type==0);
	unsigned long number_of_samples=pdescriptor->number_of_samples;
	unsigned long i=0;
	//just support 8 , 16 , 32bits per sample
	if (8==bits_per_sample)
	{
		BYTE *pdata=(BYTE *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			praw_waveform_amplitude[i]=pdata[i];	
		}
	}
	else if (16==bits_per_sample)
	{
		WORD *pdata=(WORD *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			praw_waveform_amplitude[i]=pdata[i];	
		}
	}
	else if (32==bits_per_sample)
	{
		DWORD *pdata=(DWORD *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			praw_waveform_amplitude[i]=pdata[i];	
		}
	}
	else
	{
		CString outinfo;
		outinfo.Format("bits_per_sample=%d is not supported now!",bits_per_sample);
		AfxMessageBox(outinfo);
		return FALSE;
	}
	return TRUE;
}
/*****************************************************************************************
* CLasFile类成员函数 ReadWaveInfo: 读取点记录及对应波形包记录信息
* start-开始读取的起始点号，num-需要一次读取的信息条数，laspt-点记录指针，wap-波形信息指针，
  cp-存放从波形包在文件中的偏移量与读到内存中后偏移量的转换参数
*****************************************************************************************/
BOOL CLasFile::ReadWaveInfo(const unsigned long & start,unsigned long & num,LAS_PT* laspt,BYTE*& wap,  unsigned __int64& cp)
{
	if(m_ppublic_header->number_of_point_records-start<=0)                                              //控制文件最后正好被读完
	{
	/*	AfxMessageBox("The LASFile has finished reading,ok!");*/
		return FALSE;
	}
	if (m_ppublic_header->number_of_point_records-start<num )
	{
		num=m_ppublic_header->number_of_point_records-start;
	}
	if (!ReadPt(laspt,start,num))                                                                                             //读取点记录
	{
		AfxMessageBox("Fail to read pointrecord !");                                                             //如果读取点记录失败，提示
		return FALSE;
	}
    if(!ReadWavewavepackets(laspt,num,wap,cp))                                                               //读取波形信息
	{
		AfxMessageBox("Fail to read Wavewavepackets !");                                                  //如果读取波形信息失败，提示
		return FALSE;
	}                                                                                            
    return TRUE;
}
/*****************************************************************************************
* CLasFile类成员函数 ReadPt: 读取点记录信息
* pPoint-点记录指针，start-开始读取的起始点号，num-需要一次读取的信息条数
*****************************************************************************************/
BOOL CLasFile::ReadPt(LAS_PT* pPoint,const unsigned long& start,const unsigned long& num )
{
	HANDLE hfile=m_hfile;
	if (start<0||start+num>m_ppublic_header->number_of_point_records)
	{
		return FALSE;
	}
	__int64 offset=0;
	DWORD readbytes=0;
	offset=m_ppublic_header->offset_to_point_data+start*m_ppublic_header->point_data_record_length;   //计算开始读取起始点的偏移量
	long lDistanceToMove=long(offset&0x00000000ffffffff);                                                                        // 分出低32位
	long lDistanceToMoveHigh=long(offset>>32);                                                                                        // 分出高32位  
	SetFilePointer(hfile,lDistanceToMove,&lDistanceToMoveHigh,FILE_BEGIN);                                              // 将指针调到需要读取的点记录处
	void *pdata=new BYTE[m_ppublic_header->point_data_record_length*num];                                          // 动态分配内存
 	ReadFile(hfile,pdata,m_ppublic_header->point_data_record_length*num,&readbytes,NULL);
	if(readbytes!=(m_ppublic_header->point_data_record_length*num))                                                       //读取num条点记录信息
	{
		delete[] pdata;
		pdata=NULL;
		return FALSE;
	}
	// 进一步解析点记录
    if (m_ppublic_header->point_data_format==4)                                      
	{
		LAS_Point_Format4 *pPoint4=(LAS_Point_Format4 *)pdata;
		for (unsigned long i=0;i<num;i++)
		{
			memcpy(&(pPoint[i]),pPoint4+i,sizeof(LAS_Point_Format1));
			pPoint[i].wave_packet_descriptor_index=pPoint4[i].wave_packet_descriptor_index;
			pPoint[i].byte_offset_to_waveform_data=pPoint4[i].byte_offset_to_waveform_data;
			pPoint[i].waveform_packet_size_in_bytes=pPoint4[i].waveform_packet_size_in_bytes;
            pPoint[i].return_point_waveform_location=pPoint4[i].return_point_waveform_location;
            pPoint[i].xt=pPoint4[i].xt;
            pPoint[i].yt=pPoint4[i].yt;
			pPoint[i].zt=pPoint4[i].zt;
		}
	}
	else if (m_ppublic_header->point_data_format==5)
	{
		LAS_Point_Format5 *pPoint5=(LAS_Point_Format5 *)pdata;
		memcpy(pPoint,pPoint5,m_ppublic_header->point_data_record_length*num);
	}
	delete[] pdata;                                                                   //释放内存
	pdata = NULL;
	return TRUE;
}
/*****************************************************************************************
* CLasFile类成员函数 ReadWavewavepackets: 一次读完点记录对应波形包区域
* ptemp-点记录指针，wap-波形信息指针，
  cp-存放从波形包在文件中的偏移量与读到内存中后偏移量的转换参数 
*****************************************************************************************/
BOOL CLasFile::ReadWavewavepackets(LAS_PT* ptemp,const unsigned long& num,BYTE*& wap,  unsigned __int64& cp)
{
	HANDLE hfile= m_hfile;
	unsigned __int64 offsetstart=0;
    unsigned __int64 offsetend=0;

    for(unsigned long i=0;i<num;i++)
	{
	   if(ptemp[i].wave_packet_descriptor_index!=0)                                            // 求出num条点记录中记录波形信息的点记录中
 	   {
 		  offsetstart= ptemp[i].byte_offset_to_waveform_data;                               // 所记录波形包偏移量最小值                                   
 		  break;                                                                
	   }
	}

    cp= offsetstart;                                                                                             // 这个最小值就是对应的转换参数
    for(unsigned long j=0;j<num;j++)
    {
	   if(ptemp[num-1-j].wave_packet_descriptor_index!=0)                               // 求出num条点记录中记录波形信息的点记录中
	   {                                                                                                                  // 所记录波形包偏移量最大值并加上对应波形包的尺寸 
 		   offsetend=ptemp[num-1-j].byte_offset_to_waveform_data+ptemp[num-1-j].waveform_packet_size_in_bytes;
		   break;
 	   }
    }
    unsigned int utsize=offsetend-offsetstart;                                                   // 偏移最大最小做差求出波形信息存在区域尺寸
    BYTE* wavepacket=new BYTE[utsize];                                                          // 动态分配内存
	memset(wavepacket,0,utsize);
	if(m_ppublic_header->global_encoding & 0x02)
	{
		DWORD readbytes=0;
		__int64 offset=0;
		offset=m_ppublic_header->start_of_waveform_data_packect_record+offsetstart;   //波形包区域的起始位置
		long lDistanceToMove=long(offset&0x00000000ffffffff);
		long lDistanceToMoveHigh=long(offset>>32);
		SetFilePointer(hfile,lDistanceToMove,&lDistanceToMoveHigh,FILE_BEGIN);
		ReadFile(hfile,wavepacket,utsize,&readbytes,NULL);                            // 读出波形区域
		if(readbytes!=utsize)                     
		{
			delete[] wavepacket;
			wavepacket=NULL;
			return FALSE;
		} 
	}
	else
	{
		AfxMessageBox("now .wdp data is not supported!");
		return FALSE;
	}	
	wap=wavepacket;                                                                   //将动态分配的内存指针传递出去
	return TRUE;	
}
/*****************************************************************************************
* CLasFile类成员函数 GetAmplitude: 获得波形包内振幅值
* laspt-点记录指针，ptnum-点号，waps-波形包区域指针，amplitude-振幅值，cp-转换参数
*****************************************************************************************/
int CLasFile::GetAmplitude(LAS_PT* laspt,const unsigned __int64& ptnum,BYTE* waps,unsigned long*& amplitude,
						 const unsigned __int64& cp,unsigned long& nos)
{
	if(laspt[ptnum].wave_packet_descriptor_index > m_num_of_waveform_descriptor)
	{
		return -1;
	}	
	if(0==laspt[ptnum].wave_packet_descriptor_index )
	{
		return  0;
	}
	// 获得波形包描述记录
	LAS_Waveform_Packect_Descriptor* descriptor=m_pwaveform_packect_descriptor+(laspt[ptnum].wave_packet_descriptor_index-1);
    unsigned long Wapsize=laspt[ptnum].waveform_packet_size_in_bytes;                // 获得波形包的尺寸
    void* sngWap=new BYTE[Wapsize];                                                  // 动态分配内存
	unsigned __int64 offset=laspt[ptnum].byte_offset_to_waveform_data-cp;            // 转换波形包的偏移量
	memcpy(sngWap,waps+offset,Wapsize);                                              // 截取一个波形包
	if(!ParseWavePackets(amplitude,sngWap,descriptor,nos))                                // 从波形包里解析出振幅值
	{
		AfxMessageBox("ParseWavePacket wrong!");
		delete[] sngWap;                                                              // 释放内存
        sngWap=NULL;
		return -1;
	}
	delete[] sngWap;                                                                 // 释放内存
    sngWap=NULL;
	return 1;
}
/*****************************************************************************************
* CLasFile类成员函数 ParseWavePacket: 分解波形包记录里面的信息，获得振幅值
* amplitude-振幅值指针，plaswavepacket-单个波形包指针，pdescriptor-波形包描述器指针
*****************************************************************************************/
BOOL CLasFile::ParseWavePackets(unsigned long*& amplitude,void *plaswavepacket,LAS_Waveform_Packect_Descriptor *pdescriptor,
							   unsigned long& nos)
{
	unsigned char waveform_compression_type=pdescriptor->waveform_compression_type;
	if(waveform_compression_type!=0)
	{
		AfxMessageBox("Only support waveform_compression_type=0 !");
		return FALSE;
	}
	unsigned char bits_per_sample=pdescriptor->bits_per_sample;
	unsigned long number_of_samples=pdescriptor->number_of_samples;
	nos=number_of_samples;
    amplitude=new unsigned long[number_of_samples];
	unsigned long i=0;
	//仅支持 8 , 16 , 32bits per sample
	if (8==bits_per_sample)
	{
		BYTE *pdata=(BYTE *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			amplitude[i]=pdata[i];	
		}
	}
	else if (16==bits_per_sample)
	{
		WORD *pdata=(WORD *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			amplitude[i]=pdata[i];	
		}
	}
	else if (32==bits_per_sample)
	{
		DWORD *pdata=(DWORD *)plaswavepacket;
		for (i=0;i<number_of_samples;i++)
		{
			amplitude[i]=pdata[i];	
		}
	}
	else
	{
		CString outinfo;
		outinfo.Format("bits_per_sample=%d is not supported now!",bits_per_sample);
		AfxMessageBox(outinfo);
		return FALSE;
	}
	return TRUE;
}
BOOL CLasFile::Write_Public_header(CString pathname,const int& ptnum)
{
	HANDLE hfile=CreateFile(pathname,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
	if(hfile==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("fail to open the file!");
		return FALSE;
	}
    m_hfile_write=hfile;
	LAS_Public_header public_header;

	return TRUE;
}

