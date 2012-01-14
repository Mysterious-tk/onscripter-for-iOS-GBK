/*
 * Copyright (c) 2004  Kazunori "jagarl" Ueno
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#ifndef __FILE_IMPL_H__
#define __FILE_IMPL_H__

#include<vector>

struct ARCFILE_ATOM {
	char* filename;
	char* filename_lower;
	off_t offset;
	int arcsize;
	int filesize;
	int private_data;
	bool operator <(const ARCFILE_ATOM& to) const {
		return strcmp(filename_lower, to.filename_lower) < 0;
	}
	bool operator <(char* const to) const {
		return strcmp(filename_lower, to) < 0;
	}
};

class ARCFILE {
protected:
	char* arcname;
	char* filenames_orig;
	int list_point;
	std::vector<ARCFILE_ATOM> arc_atom;
	typedef std::vector<ARCFILE_ATOM>::iterator iterator;
	ARCFILE* next; /* FILESEARCH �ΰ�Ĥη���ʣ���� ARCFILE ����ĤȤ����ꥹ�Ȥ�Ĥ��� */
	/* arcname �˻��ꤵ�줿�ե����롿�ǥ��쥯�ȥ�����ƥ����å� */
	virtual int CheckFileDeal(void);
	virtual void ListupFiles(int fname_len);
	virtual ARCINFO* MakeARCINFO(ARCFILE_ATOM&);
	iterator SearchName(const char* f, const char* ext=0);
public:
	ARCFILE(char* fname);
	void SetNext(ARCFILE* _next) { next = _next;}
	ARCFILE* Next(void) { return next; }
	void Init(void);
	virtual ~ARCFILE();
	/* �ե����븡�� */
	class ARCINFO* Find(const char* fname, const char* ext);
	/* �ե�����ꥹ�Ȥν��� */
	int Deal(void) { Init(); return arc_atom.size(); }
	void ListFiles(FILE* out);
	void InitList(void);
	char* ListItem(void);
};

class SCN2kFILE : public ARCFILE {
protected:
	virtual int CheckFileDeal(void);
	virtual void ListupFiles(int fname_len);
	virtual ARCINFO* MakeARCINFO(ARCFILE_ATOM& atom);
public:
	SCN2kFILE(char* fname) : ARCFILE(fname) {}
	virtual ~SCN2kFILE() {}
};

class CattleyaFILE : public ARCFILE {
	bool is_compress;
	/* header �� Huffman �ڹ����� */
	char* bitbuf;
	char* bitbuf_end;
	int ltree[0x400];
	int rtree[0x400];
	int treecnt;
	int bitcnt;
	int GetBit(void);
	int GetCh(void);
	void SetBuf(char* buf, int len);
	int MakeTree(void);
	int Decode(int seed);

protected:
	virtual int CheckFileDeal(void);
	virtual void ListupFiles(int fname_len);
	virtual ARCINFO* MakeARCINFO(ARCFILE_ATOM& atom);
public:
	CattleyaFILE(char* fname) : ARCFILE(fname) {is_compress = false;}
	virtual ~CattleyaFILE() {}
};

class NULFILE : public ARCFILE {
protected:
	virtual int CheckFileDeal(void);
	virtual void ListupFiles(int fname_len);
	virtual ARCINFO* MakeARCINFO(ARCFILE_ATOM& atom);
public:
	NULFILE() : ARCFILE("") {}
	virtual ~NULFILE() {}
};
class DIRFILE : public ARCFILE {
protected:
	virtual int CheckFileDeal(void);
	virtual void ListupFiles(int fname_len);
	virtual ARCINFO* MakeARCINFO(ARCFILE_ATOM& atom);
public:
	DIRFILE(char* fname) : ARCFILE(fname) {}
	virtual ~DIRFILE() {}
	FILE* Open(const char* fname); /* FILE* �򳫤� */
	char* SearchFile(const char* dirname); /* �ե����븡�� */
};
class ARCINFO_AVG32 : public ARCINFO {
	ARCINFO_AVG32(const char* name, ARCFILE_ATOM& atom) : ARCINFO(name, atom) {
	}
	virtual bool ExecExtract(void);
	friend class ARCFILE;
};
class ARCINFO2k : public ARCINFO {
	static char decode_seed[256];
	static char decode_seed2[16];
protected:
	ARCINFO2k(const char* name, ARCFILE_ATOM& atom) : ARCINFO(name,atom) {
	}
	virtual bool ExecExtract(void);
	friend class SCN2kFILE;
};

class ARCINFOZ : public ARCINFO {
protected:
	ARCINFOZ(const char* name, ARCFILE_ATOM& atom) : ARCINFO(name, atom) {
	}
	virtual bool ExecExtract(void);
	friend class DaemonBaneFILE;
	friend class CattleyaFILE;
};

#endif /* __FILE_IMPL_H__ */
