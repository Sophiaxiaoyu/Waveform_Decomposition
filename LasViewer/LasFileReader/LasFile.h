// LasFile.h: interface for the CLasFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LASFILE_H__76E5AF85_B727_4FE8_9ACB_A729A1C48046__INCLUDED_)
#define AFX_LASFILE_H__76E5AF85_B727_4FE8_9ACB_A729A1C48046__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lasdef.h"


class CLasFile  
{
private:
	BOOL m_bcolorflag;
	BOOL m_bgpstimeflag;
	BOOL m_bwavepacketflag;
	HANDLE m_hfile;
	HANDLE m_hfile_write;
	int m_num_of_waveform_descriptor;
	LAS_Waveform_Packect_Descriptor *m_pwaveform_packect_descriptor;
	LAS_Public_header *m_ppublic_header;
	
private:
	BOOL CheckLASPublicHeader(LAS_Public_header &public_header);
	void ReadVLRecord();
	//������ʹ�ú���//////////////////////////////////////////////////////////////
	// ��ȡ���¼
	BOOL ReadPt(LAS_PT* pPoint,const unsigned long& start,const unsigned long& num);
	// һ�ζ������еĲ��ΰ����ܴ��ڵ�����  
	BOOL ReadWavewavepackets(LAS_PT* ptemp,const unsigned long& num,BYTE*& wap, unsigned __int64& cp);
	// �ֽⲨ�ΰ���¼�������Ϣ��������ֵ
	BOOL ParseWavePackets(unsigned long*& amplitude,void *plaswavepacket,LAS_Waveform_Packect_Descriptor *pdescriptor,
		unsigned long& nos);   
	////////////////////////////////////////////////////////////////////////////////
public:
	CLasFile();
	void MyClear();
	virtual ~CLasFile();
	
	BOOL OpenLasfile(CString pathname);
	BOOL GetLASPublicHeaderPointer(LAS_Public_header *&ppublic_header);
	BOOL ReadLasPt(LAS_PT *plaspt,__int64 startpt,long num);
	BOOL ReadLasWavePacket(void *laswavepacket,LAS_PT &laspt);

	BOOL GetLASWaveformPackectDescriptor(
		LAS_Waveform_Packect_Descriptor* &pdescriptor,unsigned char index);
	BOOL ParseWavePacket(unsigned long *praw_waveform_amplitude,void *plaswavepacket,
		LAS_Waveform_Packect_Descriptor *pdescriptor);

	//������ʹ�ú���//////////////////////////////////////////////////////////////
	// ��ȡ������Ϣ
	BOOL ReadWaveInfo(const unsigned long & start, unsigned long & num,LAS_PT* laspt,BYTE*& wap,  unsigned __int64& cp); 
	// ��ò��ΰ������ֵ
	int GetAmplitude(LAS_PT* laspt,const unsigned __int64& ptnum,BYTE* waps,unsigned long*& amplitude,
		const unsigned __int64& cp,unsigned long& nos);
	// д�빫��ͷ��
	BOOL Write_Public_header(CString pathname,const int& ptnum);
};

#endif // !defined(AFX_LASFILE_H__76E5AF85_B727_4FE8_9ACB_A729A1C48046__INCLUDED_)
