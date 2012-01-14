/*  file.h  : KANON �ΰ��̥ե����롦PDT �ե�����ʲ����ե�����ˤ�Ÿ����
 *            ����Υ��饹
 *     class FILESEARCH : �ե�����δ�����Ԥ�
 *     class ARCINFO : ��˥ե��������Σ��ĤΥե�����򰷤����饹
 *     class PDTCONV : PDT �ե������Ÿ����Ԥ���
 */

/*
 *
 *  Copyright (C) 2000-   Kazunori Ueno(JAGARL) <jagarl@creator.club.ne.jp>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/

#ifndef __KANON_FILE_H__
#define __KANON_FILE_H__

#ifndef DIR_SPLIT
#define DIR_SPLIT '/'	/* UNIX */
#endif

// read 'KANON' compressed file

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if defined(__sparc) || defined(sparc)
#  if !defined(WORDS_BIGENDIAN)
#    define WORDS_BIGENDIAN 1
#  endif
#endif

#define INT_SIZE 4

static int read_little_endian_int(const char* buf) {
	const unsigned char *p = (const unsigned char *) buf;
	return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
}

static int read_little_endian_short(const char* buf) {
	const unsigned char *p = (const unsigned char *) buf;
	return (p[1] << 8) | p[0];
}

static int write_little_endian_int(char* buf, int number) {
	int c = read_little_endian_int(buf);
	unsigned char *p = (unsigned char *) buf;
	unsigned int unum = (unsigned int) number;
	p[0] = unum & 255;
	unum >>= 8;
	p[1] = unum & 255;
	unum >>= 8;
	p[2] = unum & 255;
	unum >>= 8;
	p[3] = unum & 255;
	return c;
}

static int write_little_endian_short(char* buf, int number) {
	int c = read_little_endian_short(buf);
	unsigned char *p = (unsigned char *) buf;
	unsigned int unum = (unsigned int) number;
	p[0] = unum & 255;
	unum >>= 8;
	p[1] = unum & 255;
	return c;
}


/*********************************************
**  FILESEARCH:
**	��˥ե����롿�ǥ��쥯�ȥ��ޤᡢ
**	���ե�����δ�����Ԥ���
**
**	�ǽ�ˡ�����ե����뤫��ե�����μ��ऴ�Ȥ�
**	�ºݤ����äƤ���ǥ��쥯�ȥꡢ��ˤ����ꤹ��
**
**	�ʹߤ�Find() �᥽�åɤǼºݤΥե���������Ƥ�����
**
*/

/* ARCFILE �� DIRFILE �ϥե�������ऴ�Ȥξ��� */
class ARCFILE;
class DIRFILE;
class SCN2kFILE;
/* ARCINFO �ϥե�������ɤ߹��ि���ɬ�� */
class ARCINFO;
class ARCFILE_ATOM;
class FILESEARCH {
public:
#define TYPEMAX 14
	enum FILETYPE {
		/* �����0 - 15 �ޤ� reserved */
		ALL = 1, /* dat/ �ʲ��Υե�����(�ǥե���Ȥθ�����) */
		ROOT= 2, /* ������Υ��󥹥ȡ���ǥ��쥯�ȥ� */
		PDT = 3, /* default: PDT/ */
		SCN = 4, /* default: DAT/SEEN.TXT */
		ANM = 5, /* default: DAT/ALLANM.ANL */
		ARD = 6, /* default: DAT/ALLARD.ARD */
		CUR = 7, /* default: DAT/ALLCUR.CUR */
		MID = 8, /* default: ALL */
		WAV = 9, /* default: ALL */
		KOE = 10, /* default: KOE/ */
		BGM = 11, /* default: BGM */
		MOV = 12, /* default : MOV */
		GAN = 13  /* default : MOV */
	};
	enum ARCTYPE {ATYPE_DIR, ATYPE_ARC, ATYPE_SCN2k};
private:
	/* InitRoot() �λ����ǽ����������ѿ� */
	DIRFILE* root_dir;
	DIRFILE* dat_dir;
	ARCFILE* searcher[TYPEMAX];
	/* �ե������¸�߰��֤� information */
	ARCTYPE is_archived[TYPEMAX];
	char* filenames[TYPEMAX];
	/* �ǥե���Ȥ� information */
	static ARCTYPE default_is_archived[TYPEMAX];
	static char* default_dirnames[TYPEMAX];
public:
	FILESEARCH(void);
	~FILESEARCH();
	/* ���˥�����Υǡ���������ǥ��쥯�ȥ�����ꤹ��ɬ�פ����� */
	int InitRoot(char* root);
	/* �ե�����η����Ȥξ���򥻥åȤ��� */
	void SetFileInformation(FILETYPE type, ARCTYPE is_arc,
		char* filename);
	/* ʣ���Υե�������Ĥη��˴�Ϣ�Ť��� */
	void AppendFileInformation(FILETYPE type, ARCTYPE is_arc,
		char* filename);
	ARCFILE* MakeARCFILE(ARCTYPE tp, char* filename);
	/* fname �ǻ��ꤵ�줿̾���Υե�����򸡺� */
	class ARCINFO* Find(FILETYPE type, const char* fname, const char* ext=0);
	/* �������Υե�����򤹤٤ƥꥹ�ȥ��å�
	** ������ NULL pointer
	*/
	char** ListAll(FILETYPE type);
};

class ARCINFO {
protected:
	/* �ե����뤽�Τ�Τξ��� */
	ARCFILE_ATOM& info;
	char* arcfile;
	/* mmap ���Ƥ����硢���ξ��� */
	bool use_mmap;
	char* mmapped_memory;
	int fd;
	/* �ե��������Ƥ����äƤ���Хåե� */
	const char* data;

protected:
	ARCINFO(const char* arcfile, ARCFILE_ATOM& from); // only from ARCFILE
	friend class ARCFILE;
	friend class DIRFILE;

	virtual bool ExecExtract(void);
public:
	/* dest ��256byte ���٤�;͵�����뤳�� */
	static void Extract(char*& dest, char*& src, char* destend, char* srcend);
	static void Extract2k(char*& dest, char*& src, char* destend, char* srcend);
	virtual ~ARCINFO();
	/* ɬ�פʤ� Read ���˸Ƥ֤��Ȥǽ�����ʬ��Ǥ��� */
	int Size(void) const;
	char* CopyRead(void); /* Read() �������ƤΥ��ԡ����֤� */
	const char* Read(void);
	/* �ե����뤬 regular file �ξ�硢�ե�����̾�򵢤� */
	/* �����Ǥʤ��ʤ� 0 �򵢤� */
	const char* Path(void) const;
	FILE* OpenFile(int* length=0) const; /* �ߴ����Τ��ᡧraw file �ξ�硢�ե�����򳫤� */
};

class GRPCONV {
public:
	int width;
	int height;
	bool is_mask;

	const char* filename;
	const char* data;
	int datalen;

	int Width(void) { return width;}
	int Height(void) { return height;}
	bool IsMask(void) { return is_mask;}

	GRPCONV(void);
	virtual ~GRPCONV();
	void Init(const char* fname, const char* data, int dlen, int width, int height, bool is_mask);

	virtual bool Read(char* image) = 0;
	static GRPCONV* AssignConverter(const char* inbuf, int inlen, const char* fname);
	static GRPCONV* AssignConverter(ARCINFO* info) {
		const char* dat = info->Read();
		if (dat == 0) return 0;
		return AssignConverter(dat, info->Size(), "???");
	}
	void CopyRGBA(char* image, const char* from);
	void CopyRGB(char* image, const char* from);
	void CopyRGBA_rev(char* image, const char* from);
	void CopyRGB_rev(char* image, const char* from);
};

extern FILESEARCH file_searcher;

#endif // !defined(__KANON_FILE_H__)
