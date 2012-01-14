/* nwatowav : Visual Arts �ϤΥ�����Υǥ�ǻȤ��� nwa ������
**            �ե������ wav �������Ѵ�����
**
**     compile : gcc -O2 -o nwatowav nwatowav.cc
**     usage : nwatowav [nwa-file [outfile]]
**	       nwatowav [nwk-file [outfile]]
**     example : nwatowav HM06.nwa HM06.wav	# BGM�ե����롣HM06.wav ��Ÿ�������
**		 nwatowav z2813.nwk z2813	# �����ե����롣 z2813-100.wav �ʤɤΥե�����̾��Ÿ�������
**		 nwatowav z0513.ovk z0513	# �����ե����롣 z0513-100.ogg �ʤɤΥե�����̾��Ÿ�������
**
**
** 2004.5.19 ��������<s1100089@u-aizu.ac.jp> ���� CLANNAD ��̵����nwa�������б�����
**           �ѥå��򤤤��������Τǡ�Ŭ�Ѥ��ޤ��������꤬�Ȥ��������ޤ���
** 2006.9.10 �����奢�ե����פβ����ե�������� (complevel = 5) �򥵥ݡ���
**	     .nwk �Ȥ�����ĥ�Ҥ���ĥե������������Ȳ����ե�����Ȥ���
**	     ��ᡢʬ�䤷��Ÿ������褦�ˤ���
** 2007.7.28 �֥�ȥ�Х����������פβ����ե�������� (*.ovk; ogg Ϣ�뷿)
**		�򥵥ݡ��ȡ�.ovk �Ȥ�����ĥ�Ҥ��ĥե�������������
**		�����ե�����Ȥ��Ʋ�ᡢʬ�䤷��Ÿ������褦�ˤ���
**	     �֥�ȥ�Х����������פ�BGM�ե������¿�̤ΥΥ�������������
**	     �����ʥ���󥰥����̤ν�������ɬ�פ��ä���
*/

/*
 * Copyright 2001-2007  jagarl / Kazunori Ueno <jagarl@creator.club.ne.jp>
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * ���Υץ����κ�Ԥ� jagarl �Ǥ���
 *
 * ���Υץ���ࡢ�ڤӥ���ѥ���ˤ�ä����������Х��ʥ��
 * �ץ������ѹ����롢���ʤ��ˤ�����餺�����۲�ǽ�Ǥ���
 * ���κݡ��嵭 Copyright ɽ�����ݻ�����ʤɤξ��ϲݤ���
 * �����б������ݤʤΤǥХ�����������᡼���Ϣ��򤹤�
 * �ʤɤ�ɬ�פ⤢��ޤ��󡣥������ΰ�����ή�Ѥ��뤳�Ȥ�ޤᡢ
 * ����ͳ�ˤ��Ȥ�����������
 *
 * THIS SOFTWARE IS PROVIDED BY KAZUNORI 'jagarl' UENO ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KAZUNORI UENO BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 */

/********************************************
**
**	nwa �ե����ޥåȤˤĤ���
**
**		���ΤȤ��Ƥϰʲ��ι�¤�����
**		NWA Header
**		data offset index
**		data block<0>
**		data block<1>
**		...
**		data block<N>
**
**	NWA Header: �ե�������Ƭ���� 44 bytes
**		magic number �ʤɤϤʤ��Τ�nwa �ե����뤫��
**		�ǡ���������������Ƚ�Ǥ���ɬ�פ�����
**		�ǡ��������� little endian �ǡ�
**		short(signed 2byte)�ޤ��� int(signed 4byte) �Ǥ��롣
**
**		+00 short   channel ��(1/2)
**		+02 short   sample ��Ĥ������ bit ��(16)
**		+04 int     ���ȿ�(���ä�����Υǡ�����)
**		+08 int     ���̥�٥롧-1~5.2�ǺǾ��Υǡ�����0�Ǻ����������(-1��̵����raw�ǡ����Ȥߤʤ����)
**		+12 int     ?
**		+16 int     �֥�å���
**		+20 int     Ÿ����Υǡ������礭��(�Х���ñ��)
**		+24 int     ���̻��Υǡ������礭��(nwa �ե�������礭�����Х���ñ��)
**		+28 int     ����ץ����Ÿ����Υǡ�����(16bit data�ʤ� short ñ��==����ץ�ñ�̤Υǡ������礭��)
**		+32 int     �ǡ������֥�å���Ÿ���������Υ���ץ�ñ�̤Υǡ�����
**		+36 int     �ǽ��֥�å���Ÿ���������Υ���ץ�ñ�̤Υǡ�����
**		+40 int     ?
**		
**	data offset index
**		���֥�å��� x 4 byte �Υǡ���
**		���줾�� int �Υǡ��������֥�å���³���Ƥ���
**
**		�ǡ����֥�å�����Ƭ��ؤ��ե�������Ƭ����ΰ���(���ե��å�)
**		����Ǽ����Ƥ���
**
**	data block
**		Ĺ���ϲ��ѡ�Ÿ�����뤳�Ȥǰ�����礭�����ĥǡ�����Ÿ������롣
**		�ǡ�����DPCM�������� PCM �ǡ����� a,b,c �ʤ�� (a),b-a, c-b ��
**		���ä���ʬ�ǡ�����������3-5bit,�ؿ�3bit�η�������¸����Ƥ��롣
**		���Ū�ˡ�16bit �Υǡ�����¿���ξ�� 6-8bit �ǳ�Ǽ����롣
**		�����Υӥåȿ��ϰ��̥�٥�0��5bit�����̥�٥�2��3bit�Ȥʤ롣
**		�ʲ������̥�٥�2�ξ��ˤĤ����ä�ʤ�롣
**		��Υ��ξ�硧
**			+00 short  �֥�å���κǽ�Υǡ���
**			+02- bit stream
**		���ƥ쥪�ξ�硧
**			+00 short  ��(?)�����ͥ�κǽ�Υǡ���
**			+02 short  ��(?)�����ͥ�κǽ�Υǡ���
**			+04- bit stream
**
**		��ʬ�ǡ��������٤��⤯�ʤ��Τǳƥ֥�å�����Ƭ��
**		���Τʥǡ����ˤ�����������(��)
**
**	bit stream
**		little endian
**		+0 - +2 : �ؿ�
**		+3 - +5 : ����
**		�η������㤨�� a,b,c �Ȥ���8bit�ǡ���������С�
**		a&0x07 : �ǡ������λؿ�
**		(a>>3)&0x07 : �ǡ������β���(signed ; 
**		((b<<2)|(a>>6))&0x07 : �ǡ������λؿ�
**		(b>>1)&0x07 : �ǡ������β���
**		�Ȥʤ롣
**		���������ؿ����ͤˤ�겾����bit�����Ѳ����뤳�Ȥ����롣
**		�ؿ� = 1 - 6 �ξ�硧
**			a=�ؿ���b=������p=���Υǡ����Ȥ��ơ�����Υǡ���d ��
**			b��2bit�ܤ�Ω�äƤ����硧
**				d = p - (b&3)<<(4+a)
**			Ω�äƤʤ���硧
**				d = p + (b&3)<<(4+a)
**		�ؿ� = 0 �ξ�硧������¸�ߤ��ʤ�(�ǡ�����3bit�Ȥʤ�)
**			d = p
**			�����奢�ե����פβ����ե����� (complevel == 5) �Ǥϥ���󥰥������Ѥ˻Ȥ��Ƥ��롣
**		�ؿ� = 7
**			���� bit ��Ω�äƤ����硧
**				d = 0 (����̤����)
**				(�ǡ�����4bit�Ȥʤ�)
**			���� bit ��Ω�äƤʤ���硧
**				complevel = 0,1,2:
**				   ���� b = 6bit
**				   b �� 5bit �ܤ�Ω�äƤ����硧
**					d = p - (b&0x1f)<<(4+7)
**				   Ω�äƤʤ���硧
**					d = p + (b&0x1f)<<(4+7)
**				   (�ǡ�����10bit�Ȥʤ�)
**				complevel = 3,4,5:
**				   ���� b = 8bit
**				   b �� 7bit �ܤ�Ω�äƤ����硧
**					d = p - (b&0x7f)<<9
**				   Ω�äƤʤ���硧
**					d = p + (b&0x1f)<<9
**				   (�ǡ�����10bit�Ȥʤ�)
**
**		���̥�٥뤬�ۤʤ��硢���Ȥ��а��̥�٥�==0��
**			�ؿ�==1~6��d�κǾ��bit��Ω�äƤ�����
**				d = p - (b&0x0f)<<(2+a)
**			�ؿ�==7��d�κǾ��bit��Ω�äƤ�����
**				d = p - (b&0x7f)<<(2+7)
**				(b : 8bit�ʤΤǥǡ�����12bit�Ȥʤ�)
**		�Τ褦�ˡ����٤������Ѳ�����褦�ˤʤäƤ��롣
**
**	�إå��ɤ߹��ߤˤĤ��Ƥ�NWAData::ReadHeader()����
**	bit stream ����Υǡ���Ÿ���ˤĤ��Ƥ� NWADecode()����
**************************************************************
*/

// #define NDEBUG /* �ʤ��� assert�����ä�����®�������� */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>	// for isatty() function
#include<sys/stat.h>
#include<string.h>


#ifdef WORDS_BIGENDIAN
#error Sorry, This program does not support BIG-ENDIAN system yet.
/* �⤷ big endian �Υ����ƥ���б���������
** �ʲ��� *_little_endian_* �ڤ�
** getbits() �ؿ����ѹ�����ɬ�פ�����
*/
#endif

inline int read_little_endian_int(const char* buf) {
	return *(int*)buf;
}

inline int read_little_endian_short(const char* buf) {
	return *(short*)buf;
}

inline int write_little_endian_int(char* buf, int number) {
	int c = *(int*)buf; *(int*)buf = number; return c;
}

inline int write_little_endian_short(char* buf, int number) {
	int c = *(short*)buf; *(short*)buf = number; return c;
}
inline int getbits(const char*& data, int& shift, int bits) {
	if (shift > 8) { data++; shift-=8;}
	int ret = read_little_endian_short(data)>>shift;
	shift += bits;
	return ret & ((1<<bits)-1); /* mask */
}

/* ���ꤵ�줿�����Υإå���Ĥ��� */
const char* make_wavheader(int size, int channels, int bps, int freq) {
	static char wavheader[0x2c] = {
		'R','I','F','F',
		0,0,0,0, /* +0x04: riff size*/
		'W','A','V','E',
		'f','m','t',' ',
		16,0,0,0, /* +0x10 : fmt size=0x10 */
		1, 0,    /* +0x14 : tag : pcm = 1 */
		2, 0,    /* +0x16 : channels */
		0,0,0,0, /* +0x18 : samples per second */
		0,0,0,0, /* +0x1c : average bytes per second */
		0,0,     /* +0x20 : block alignment */
		0,0,     /* +0x22 : bits per sample */
		'd','a','t','a',
		0,0,0,0};/* +0x28 : data size */
	write_little_endian_int(wavheader+0x04, size+0x24);
	write_little_endian_int(wavheader+0x28, size);
	write_little_endian_short(wavheader+0x16, channels);
	write_little_endian_short(wavheader+0x22, bps);
	write_little_endian_int(wavheader+0x18, freq);
	int byps = (bps+7)>>3;
	write_little_endian_int(wavheader+0x1c, freq*byps*channels);
	write_little_endian_short(wavheader+0x20, byps*channels);
	return wavheader;
}

/* NWA �� bitstreamŸ����ɬ�פȤʤ���� */
class NWAInfo {
	int channels;
	int bps;
	int complevel;
	bool use_runlength;
public:
	NWAInfo(int c,int b,int cl) {
		channels=c;
		bps=b;
		complevel=cl;
		use_runlength = false;
		if (cl == 5) {
			use_runlength = true; // Tomoyo After (.nwk koe file)
			if (channels == 2) use_runlength = false; // BGM*.nwa in Little Busters!
		}
	}
	int Channels(void) const{return channels;}
	int Bps(void) const { return bps;}
	int CompLevel(void) const { return complevel;}
	int UseRunLength(void) const { return use_runlength; }
};

template<class NWAI> void NWADecode(const NWAI& info,const char* data, char* outdata, int datasize, int outdatasize) {
	int d[2];
	int i,j;
	int shift = 0;
	const char* dataend = data+datasize;
	/* �ǽ�Υǡ������ɤ߹��� */
	if (info.Bps() == 8) {d[0] = *data++; datasize--;}
	else /* info.Bps() == 16 */ {d[0] = read_little_endian_short(data); data+=2; datasize-=2;}
	if (info.Channels() == 2) {
		if (info.Bps() == 8) {d[1] = *data++; datasize--;}
		else /* info.Bps() == 16 */ {d[1] = read_little_endian_short(data); data+=2; datasize-=2;}
	}
	int dsize = outdatasize / (info.Bps()/8);
	int flip_flag = 0; /* stereo �� */
	int runlength = 0;
	for (i=0; i<dsize; i++) {
		if (data >= dataend) break;
		if (runlength == 0) { // ���ԡ��롼����Ǥʤ��ʤ�ǡ����ɤ߹���
			int type = getbits(data, shift, 3);
			/* type �ˤ��ʬ����0, 1-6, 7 */
			if (type == 7) {
				/* 7 : �礭�ʺ�ʬ */
				/* RunLength() ͭ������CompLevel==5, �����ե�����) �Ǥ�̵�� */
				if (getbits(data, shift, 1) == 1) {
					d[flip_flag] = 0; /* ̤���� */
				} else {
					int BITS, SHIFT;
					if (info.CompLevel() >= 3) {
						BITS = 8;
						SHIFT = 9;
					} else {
						BITS = 8-info.CompLevel();
						SHIFT = 2+7+info.CompLevel();
					}
					const int MASK1 = (1<<(BITS-1));
					const int MASK2 = (1<<(BITS-1))-1;
					int b = getbits(data, shift, BITS);
					if (b&MASK1)
						d[flip_flag] -= (b&MASK2)<<SHIFT;
					else
						d[flip_flag] += (b&MASK2)<<SHIFT;
				}
			} else if (type != 0) {
				/* 1-6 : �̾�κ�ʬ */
				int BITS, SHIFT;
				if (info.CompLevel() >= 3) {
					BITS = info.CompLevel()+3;
					SHIFT = 1+type;
				} else {
					BITS = 5-info.CompLevel();
					SHIFT = 2+type+info.CompLevel();
				}
				const int MASK1 = (1<<(BITS-1));
				const int MASK2 = (1<<(BITS-1))-1;
				int b = getbits(data, shift, BITS);
				if (b&MASK1)
					d[flip_flag] -= (b&MASK2)<<SHIFT;
				else
					d[flip_flag] += (b&MASK2)<<SHIFT;
			} else { /* type == 0 */
				/* ����󥰥����̤ʤ��ξ��Ϥʤˤ⤷�ʤ� */
				if (info.UseRunLength() == true) {
					/* ����󥰥����̤���ξ�� */
					runlength = getbits(data,shift,1);
					if (runlength==1) {
						runlength = getbits(data,shift,2);
						if (runlength == 3) {
							runlength = getbits(data, shift, 8);
						}
					}
				}
			}
		} else {
			runlength--;
		}
		if (info.Bps() == 8) {
			*outdata++ = d[flip_flag];
		} else {
			write_little_endian_short(outdata, d[flip_flag]);
			outdata += 2;
		}
		if (info.Channels() == 2) flip_flag ^= 1; /* channel �ڤ��ؤ� */
	}
	return;
};

class NWAData {
public:
	int channels;
	int bps; /* bits per sample */
	int freq; /* samples per second */
private:
	int complevel; /* compression level */
	int dummy; /* ? : 0x00 */
public:
	int blocks; /* block count */
	int datasize; /* all data size */
private:
	int compdatasize; /* compressed data size */
	int samplecount; /* all samples */
	int blocksize; /* samples per block */
	int restsize; /* samples of the last block */
	int dummy2; /* ? : 0x89 */
	int curblock;
	int* offsets;
	int offset_start;
	int filesize;
	char* tmpdata;
public:
	void ReadHeader(FILE* in, int file_size=-1);
	int CheckHeader(void); /* false: invalid true: valid */
	NWAData(void) {
		offsets = 0;
		tmpdata = 0;
	}
	~NWAData(void) {
		if (offsets) delete[] offsets;
		if (tmpdata) delete[] tmpdata;
	}
	int BlockLength(void) {
		if (complevel != -1) {
			if (offsets == 0) return false;
			if (tmpdata == 0) return false;
		}
		return blocksize * (bps/8);
	}
	/* data �� BlockLength �ʾ��Ĺ������Ĥ���
	** �֤��ͤϺ��������ǡ�����Ĺ������λ���� 0��
	** ���顼���� -1
	*/
	int Decode(FILE* in, char* data, int& skip_count);
	void Rewind(FILE* in);
};

void NWAData::ReadHeader(FILE* in, int _file_size) {
	char header[0x2c];
	struct stat sb;
	int i;
	if (offsets) delete[] offsets;
	if (tmpdata) delete[] tmpdata;
	offsets = 0;
	tmpdata = 0;
	filesize = 0;
	offset_start = ftell(in);
	if (offset_start == -1) offset_start = 0;
	if (_file_size != -1) filesize = _file_size;
	curblock = -1;
	/* header �ɤ߹��� */
	if (in == 0 || feof(in) || ferror(in)) {
		fprintf(stderr,"invalid stream\n");
		return;
	}
	fread(header, 0x2c, 1, in);
	if (feof(in) || ferror(in)) {
		fprintf(stderr,"invalid stream\n");
		return;
	}
	channels = read_little_endian_short(header+0x00);
	bps = read_little_endian_short(header+0x02);
	freq = read_little_endian_int(header+0x04);
	complevel = read_little_endian_int(header+0x08);
	dummy = read_little_endian_int(header+0x0c);
	blocks = read_little_endian_int(header+0x10);
	datasize = read_little_endian_int(header+0x14);
	compdatasize = read_little_endian_int(header+0x18);
	samplecount = read_little_endian_int(header+0x1c);
	blocksize = read_little_endian_int(header+0x20);
	restsize = read_little_endian_int(header+0x24);
	dummy2 = read_little_endian_int(header+0x28);
	if (complevel == -1) {	/* ̵����raw�ǡ��� */
		/* Ŭ���˷���Ǥ����� */
		blocksize = 65536;
		restsize = (datasize % (blocksize * (bps/8))) / (bps/8);
		blocks = datasize / (blocksize * (bps/8)) + (restsize > 0 ? 1 : 0);
	}
	if (blocks <= 0 || blocks > 1000000) {
		/* �����֤�Ķ����ʤäƤΤϤʤ��Ǥ���*/
		fprintf(stderr,"too large blocks : %d\n",blocks);
		return;
	}
	/* regular file �ʤ� filesize �ɤ߹��� */
	if (filesize == 0 && fstat(fileno(in), &sb)==0 && (sb.st_mode&S_IFMT) == S_IFREG) {
		int pos = ftell(in);
		fseek(in, 0, 2);
		filesize = ftell(in);
		fseek(in, pos, 0);
		if (pos+blocks*4 >= filesize) {
			fprintf(stderr,"offset block is not exist\n");
			return;
		}
	}
	if (complevel == -1) return;
	/* offset index �ɤ߹��� */
	offsets = new int[blocks];
	fread(offsets, blocks, 4, in);
	for (i=0; i<blocks; i++) {
		offsets[i] = read_little_endian_int((char*)(offsets+i));
	}
	if (feof(in) || ferror(in)) {
		fprintf(stderr,"invalid stream\n");
		delete[] offsets;
		offsets = 0;
		return;
	}
	return;
}
void NWAData::Rewind(FILE* in) {
	curblock = -1;
	fseek(in, 0x2c, 0);
	if (offsets) fseek(in, blocks*4, 1);
}
int NWAData::CheckHeader(void) {
	if (complevel != -1 && offsets == 0) return false;
	/* �ǡ������Τ�ΤΥ����å� */
	if (channels != 1 && channels != 2) {
		fprintf(stderr,"This program only supports mono / stereo data : data have %d channels.\n",channels);
		return false;
	}
	if (bps != 8 && bps != 16) {
		fprintf(stderr,"This program only supports 8 / 16bit data : data is %d bits\n",bps);
		return false;
	}
	if (complevel == -1) {
		int byps = bps/8; /* bytes per sample */
		if (datasize != samplecount*byps) {
			fprintf(stderr,"invalid datasize : datasize %d != samplecount %d * samplesize %d\n",datasize,samplecount,byps);
			return false;
		}
		if (samplecount != (blocks-1)*blocksize+restsize ) {
			fprintf(stderr,"total sample count is invalid : samplecount %d != %d*%d+%d(block*blocksize+lastblocksize).\n",samplecount,blocks-1,blocksize,restsize);
			return false;
		}
		else
			return true;
	}
	//if (complevel < 0 || complevel > 2) {
	if (complevel < 0 || complevel > 5) {
		fprintf(stderr,"This program only supports -1,0,1,2 compression level : the level of data is %d\n",complevel);
		return false;
	}
	/* �����������å� */
	if (filesize != 0 && filesize != compdatasize) {
		fprintf(stderr,"file size is invalid : %d != %d\n",filesize,compdatasize);
		return false;
	}
	if (offsets[blocks-1] >= compdatasize) {
		fprintf(stderr,"the last offset overruns the file.\n");
		return false;
	}
	int byps = bps/8; /* bytes per sample */
	if (datasize != samplecount*byps) {
		fprintf(stderr,"invalid datasize : datasize %d != samplecount %d * samplesize %d\n",datasize,samplecount,byps);
		return false;
	}
	if (samplecount != (blocks-1)*blocksize+restsize ) {
		fprintf(stderr,"total sample count is invalid : samplecount %d != %d*%d+%d(block*blocksize+lastblocksize).\n",samplecount,blocks-1,blocksize,restsize);
		return false;
	}
	tmpdata = new char[blocksize*byps*2]; /* ����ʾ���礭���Ϥʤ����������� */
	return true;
}

class NWAInfo_sw2 {
public:
	int Channels(void) const{return 2;}
	int Bps(void) const { return 16;}
	int CompLevel(void) const { return 2;}
	int UseRunLength(void) const { return false; }
};
int NWAData::Decode(FILE* in, char* data, int& skip_count) {
	if (complevel == -1) {		/* ̵���̻��ν��� */
		if (feof(in) || ferror(in)) return -1;
		if (curblock == -1) {
			/* �ǽ�Υ֥�å��ʤ顢wave header ���� */
			memcpy(data, make_wavheader(datasize, channels, bps, freq), 0x2c);
			curblock++;
			fseek(in, offset_start + 0x2c, SEEK_SET);
			return 0x2c;
		}
		if (skip_count > blocksize/channels) {
			skip_count -= blocksize/channels;
			fseek(in, blocksize*(bps/8), SEEK_CUR);
			curblock++;
			return -2;
		}
		if (curblock < blocks) {
			int readsize = blocksize;
			if (skip_count) {
				fseek(in, skip_count*channels*(bps/8), SEEK_CUR);
				readsize -= skip_count * channels;
				skip_count = 0;
			}
			int err = fread(data, 1, readsize * (bps/8), in);
			curblock++;
			return err;
		}
		return -1;
	}
	if (offsets == 0 || tmpdata == 0) return -1;
	if (blocks == curblock) return 0;
	if (feof(in) || ferror(in)) return -1;
	if (curblock == -1) {
		/* �ǽ�Υ֥�å��ʤ顢wave header ���� */
		memcpy(data, make_wavheader(datasize, channels, bps, freq), 0x2c);
		curblock++;
		return 0x2c;
	}
	/* �����ɤ߹��ࡿ�ǥ����ɤ���ǡ������礭�������� */
	int curblocksize, curcompsize;
	if (curblock != blocks-1) {
		curblocksize = blocksize * (bps/8);
		curcompsize = offsets[curblock+1] - offsets[curblock];
		if (curblocksize >= blocksize*(bps/8)*2) return -1; // Fatal error
	} else {
		curblocksize = restsize * (bps/8);
		curcompsize = blocksize*(bps/8)*2;
	}
	if (skip_count > blocksize/channels) {
		skip_count -= blocksize/channels;
		fseek(in, curcompsize, SEEK_CUR);
		curblock++;
		return -2;
	}
	/* �ǡ����ɤ߹��� */
	fread(tmpdata, 1, curcompsize, in);
	/* Ÿ�� */
	if (channels == 2 && bps == 16 && complevel == 2) {
		NWAInfo_sw2 info;
		NWADecode(info, tmpdata, data, curcompsize, curblocksize);
	} else {
		NWAInfo info(channels, bps, complevel);
		NWADecode(info, tmpdata, data, curcompsize, curblocksize);
	}
	int retsize = curblocksize;
	if (skip_count) {
		int skip_c = skip_count * channels * (bps/8);
		retsize -= skip_c;
		memmove(data, data+skip_c, skip_c);
		skip_count = 0;
	}
	curblock++;
	return retsize;
}

#ifdef USE_MAIN

void conv(FILE* in, FILE* out, int skip_count, int in_size = -1) {
	NWAData h;
	h.ReadHeader(in, in_size);
	h.CheckHeader();
	int bs = h.BlockLength();
	char* d = new char[bs];
	int err;
	while( (err=h.Decode(in, d, skip_count)) != 0) {
		if (err == -1) break;
		if (err == -2) continue;
		fwrite(d, err, 1, out);
	}
	return;
}
int main(int argc, char** argv) {
	int skip_count = 0;

	if (argc > 2 && strcmp(argv[1], "--skip") == 0) {
		skip_count = atoi(argv[2]);
		argc -= 2;
		argv[1] = argv[3];
		argv[2] = argv[4];
	}
	if (argc != 2 && argc != 3) {
		fprintf(stderr,"usage : nwatowav [inputfile [outputfile]]\n");
		return -1;
	}
	if (strstr(argv[1], ".nwk") != 0 || strstr(argv[1], ".ovk") != 0) {
		bool is_ovk;
		int headblk_sz;
		char* out_ext;

		char* outpath = new char[strlen(argv[1])+10];
		char buf[1024];
		memset(buf, 0, 1024);
		FILE* in = fopen(argv[1], "rb");
		if (in == 0) {
			fprintf(stderr,"Cannot open file : %s\n",argv[1]);
			return -1;
		}
		if (strstr(argv[1], ".ovk") != 0) {
			is_ovk = true;
			headblk_sz = 16; 
			out_ext = "ogg";
		} else {
			is_ovk = false;
			headblk_sz = 12;
			out_ext = "wav";
		}
		fread(buf, 1, 4, in);
		int index = read_little_endian_int(buf);
		if (index <= 0) { 
			if (is_ovk)
				fprintf(stderr,"Invalid Ogg-ovk file : %s : index = %d\n",argv[1],index);
			else
				fprintf(stderr,"Invalid Koe-nwk file : %s : index = %d\n",argv[1],index);
			return -1;
		}
		int* tbl_off = new int[index];
		int* tbl_siz = new int[index];
		int* tbl_cnt = new int[index];
		int* tbl_origsiz = new int[index];
		int i;
		for (i=0; i<index; i++) {
			fread(buf, 1, headblk_sz, in);
			tbl_siz[i] = read_little_endian_int(buf);
			tbl_off[i] = read_little_endian_int(buf+4);
			tbl_cnt[i] = read_little_endian_int(buf+8);
			tbl_origsiz[i] = read_little_endian_int(buf+12);
		}
		fseek(in, 0, 2);
		int fsize = ftell(in);
		for (i=0; i<index; i++) {
			if (tbl_off[i] <= 0 || tbl_siz[i] <= 0 || tbl_off[i]+tbl_siz[i] > fsize) {
				fprintf(stderr,"Invalid table[%d] : cnt %d off %d size %d / %d\n",i,tbl_cnt[i],tbl_off[i],tbl_siz[i],fsize);
				continue;
			}
			if (argc == 2)
				sprintf(outpath, "%s-%d.%s", argv[1], tbl_cnt[i],out_ext);
			else
				sprintf(outpath, "%s-%d.%s", argv[2], tbl_cnt[i],out_ext);
			FILE* out = fopen(outpath, "wb");
			if (out == 0) {
				fprintf(stderr,"Cannot open output file %s\n",outpath);
				continue;
			}
			fprintf(stderr,"Writing file %s...\n",outpath);
			fseek(in, tbl_off[i], 0);
			if (is_ovk) { // copy file
				int sz = tbl_siz[i];
				char buf[32*1024];
				while(sz > 32*1024) {
					fread(buf, 32*1024, 1, in);
					fwrite(buf, 32*1024, 1, out);
					sz -= 1024*32;
				}
				if (sz > 0) {
					fread(buf, sz, 1, in);
					fwrite(buf, sz, 1, out);
				}
			} else { // .nwk
				conv(in, out, 0, tbl_siz[i]);
			}
			fclose(out);
		}
		fclose(in);
		return 0;
	}
	FILE* in = fopen(argv[1],"rb");
	if (in == 0) {
		fprintf(stderr,"Cannot open file : %s\n",argv[1]);
		return -1;
	}
	FILE* out;
	if (argc != 3 && (!isatty(fileno(stdout)))) {	// wave file is written to stdout if stdout is redirected to a file
		out = stdout;
	} else {					// make a new file or use argv[2] for output file name
		char* outpath = new char[strlen(argv[1])+10];
		sprintf(outpath, "%s.wav",argv[1]);
		if (argc == 3) outpath = argv[2];
		out = fopen(outpath, "wb");
		if (out == 0) {
			fprintf(stderr,"Cannot open file : %s\n",outpath);
			return -1;
		}
	}
	conv(in, out, skip_count);
	fclose(in);
	if (out != stdout) fclose(out);
	return 0;
}
#else

#include"wavfile.h"

void NWAFILE::Seek(int count) {
	if (data == 0) data = new char[block_size];
	nwa->Rewind(stream);
	int dmy = 0;
	nwa->Decode(stream, data, dmy); // skip wav header
	data_len = 0;
	skip_count = count;
}
NWAFILE::NWAFILE(FILE* _stream) {
	skip_count = 0;
	data = 0;
	stream = _stream;
	nwa = new NWAData;
	nwa->ReadHeader(stream);
	if (!nwa->CheckHeader()) {
		return;
	}
	block_size = nwa->BlockLength();
	data = new char[block_size];
	data_len = 0;

	wavinfo.SamplingRate = nwa->freq;
	wavinfo.Channels = nwa->channels;
	wavinfo.DataBits = nwa->bps;

	int dmy = 0;
	data_len = nwa->Decode(stream, data, dmy); // skip wav header

	return;
}
NWAFILE::~NWAFILE() {
	if (stream) fclose(stream);
	if (data) delete[] data;
	if (nwa) delete nwa;
}
int NWAFILE::Read(char* buf, int blksize, int blklen) {
	if (data == 0) return -1; // end of file

	if (data_len > blksize * blklen) {
		int len = blksize * blklen;
		memcpy(buf, data, len);
		memmove(data, data+len, data_len-len);
		data_len -= len;
		return blklen;
	}
	memcpy(buf, data, data_len);
	int copied_length = data_len;
	data_len = 0;

	if (stream == 0) {
		delete[] data;
		data = 0;
		return copied_length / blksize;
	}

	// read
	do {
		int err;
retry:
		err = nwa->Decode(stream, data, skip_count);
		if (err == 0 || err == -1) { // eof or error
			delete[] data;
			data = 0;
			return copied_length / blksize;
		}
		if (err == -2) goto retry; // EAGAIN
		data_len = err;
		if (copied_length + data_len < blklen*blksize) {
			memcpy(buf+copied_length, data, data_len);
			copied_length += data_len;
			goto retry;
		}
	} while(0);

	// determine return length
	int datablks = (data_len+copied_length)/blksize;
	if (datablks <= 0) return 0;
	if (datablks > blklen) datablks = blklen;
	int rest_len = datablks * blksize - copied_length;
	if (rest_len) {
		memcpy(buf+copied_length, data, rest_len);
		memmove(data, data+rest_len, data_len-rest_len);
		data_len -= rest_len;
	}
	return datablks;
}

char* NWAFILE::ReadAll(FILE* in, int& total_size) {
	NWAData h;
	if (in == 0) return 0;
	h.ReadHeader(in);
	h.CheckHeader();
	int bs = h.BlockLength();
	total_size = h.datasize+0x2c;
	char* d = new char[total_size + bs*2];
	int dcur = 0;
	int err;
	int skip = 0;
	while(dcur < total_size+bs && (err=h.Decode(in, d+dcur, skip)) != 0) {
		if (err == -1) break;
		if (err == -2) continue;
		dcur += err;
	}
	return d;
}

#include"music.h"

char* decode_koe_nwa(AvgKoeInfo info, int* data_len) {
	NWAData h;
	if (info.stream == 0) return 0;
	fseek(info.stream, info.offset, 0);
	h.ReadHeader(info.stream, info.length);
	if (h.CheckHeader() == false) return 0;
	int bs = h.BlockLength();
	int total = h.datasize + 0x2c;
	char* d = new char[total + bs*2];
	int dcur = 0;
	int err;
	int skip = 0;
	while(dcur < total+bs && (err=h.Decode(info.stream, d+dcur, skip)) != 0) {
		if (err == -1) break;
		if (err == -2) continue;
		dcur += err;
	}
	if (data_len) {
		*data_len = dcur;
		if (*data_len > total) *data_len = total;
	}
	return d;
}

#endif
