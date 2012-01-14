/*  system_config.cc
 *      gameexe.ini �ե�������ɤ߹���
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <map>
#include <string>
#include "system_config.h"
#include "../system/file.h"

using namespace std;

// #define DEBUG_CONFIG
#ifdef DEBUG_CONFIG
# define dprintf(X) printf X
#else
# define dprintf(X)
#endif /* DEBUG_CONFIG */

#define MAXTOKEN 10 /* = �Ƕ��ڤ�줿�ΰ�κ���� */
#define MAXVARS 32 /* , �Ƕ��ڤ�줿���ͤκ���� */

// ������ե�������ɤ߹���
/* config �� ʸ���󡢿��󡢤���ʣ��ʤɡ�������ʷ��������ꤦ�� */
/* ʸ����ȿ���ϰ��̤� AyuSys_Config ���饹�˴ޤ�� */


/**********************************************************/
/* �Ȥꤢ�����ϥå�������դ�ʸ���� */
class HashStr {
	const char* str;
	unsigned int hash;
public:
	HashStr(const char*);
	HashStr(const HashStr& orig);
	~HashStr() {
		if (str) delete[] str;
	}
	const char* c_str(void) const { return str; }
	friend inline int operator<(const HashStr& a, const HashStr& b) {
		if (a.hash == b.hash) {
			if (a.str == 0) return 1;
			else if (b.str == 0) return 0;
			else return strcmp(a.str, b.str);
		}
		else return a.hash < b.hash;
	}
};
HashStr::HashStr(const char* s ) {
	if (s == 0 || s[0] == '\0') {
		str = 0; hash = 0; return; /* invalid string */
	}
	char* new_str = new char[strlen(s)+1];
	strcpy(new_str, s);
	str = new_str;
	/* calc hash... Ŭ�� */
	int h = strlen(s);
	while(*s != 0) {
		h = *s + ((h * (0x9449+*s))>>7);
		s++;
	}
	hash = (unsigned int)h;
}
HashStr::HashStr(const HashStr& orig) {
	if (orig.str == 0 || orig.str[0] == '\0') {
		str = 0; hash = 0; return; /* invalid */
	}
	char* new_str = new char[strlen(orig.str)+1];
	strcpy(new_str, orig.str);
	str = new_str;
	hash = orig.hash;
}

/**********************************************************
**AyuSys_Config_[String | Intlist] :
** ���������
**	original : ������
**	old_data : ���� ClearDiff() �����Ȥ�������
**	new_data : ClearDiff() �ʹߤ����ꤷ�����Ƥ���¸
**	�ǡ������ꡧ
**	Init() : ����������
**	Set() : ������ѹ�
**	Get() : �Ǥ⿷�������������
**
**	�ѹ��ε�Ͽ��
**	Diff() : �����ClearDiff() �����ѹ��������Ƥ�����
**	DiffLen() : Diff() ��ɬ�פ�ʸ����Ĺ������
**	ClearDiff() : �ѹ���Ͽ��ä�
**	PatchOld() : Diff() ��������Ͽ�˴�Ť����ѹ����ξ��֤��᤹
**	PatchNew() : Diff() ��������Ͽ�˴�Ť����ѹ���ξ��֤��᤹
**
**	�����꤫����ѹ��ε�Ͽ��
**	SetOriginal() : ��������᤹
**	DiffOriginal() : �����꤫�鸽�ߤ�������ѹ�������
**	DiffOriginalLen() : DiffOriginal() ��ɬ�פ�ʸ����Ĺ������
**	PatchOriginal() : DiffOriginal() ��������Ͽ�˴�Ť�����������줹��
*/

/************************************************
** AyuSysConfigStringItem
** ʸ�����ǡ����Ȥ��Ƥ���������
*/
class AyuSysConfigStringItem {
	char* original_data;
	char* old_data;
	char* new_data;
public:
	AyuSysConfigStringItem(void) {
		original_data = 0;
		old_data = 0;
		new_data = 0;
	}
	AyuSysConfigStringItem(const AyuSysConfigStringItem& o) {
		original_data = 0; old_data = 0; new_data = 0;
		if (o.original_data) {
			original_data = new char[strlen(o.original_data)+1];
			strcpy(original_data, o.original_data);
		}
		if (o.old_data) {
			old_data = new char[strlen(o.old_data)+1];
			strcpy(old_data, o.old_data);
		}
		if (o.new_data) {
			new_data = new char[strlen(o.new_data)+1];
			strcpy(new_data, o.new_data);
		}
	}
	/* ���ꡧInit �ǽ������Set ���ѹ���Get ���ѹ���ͥ�褷�Ƽ��Ф� */
	void Init(int deal, const char* str) { /* deal ��̵�� */
		if (original_data) delete[] original_data;
		int len = strlen(str);
		original_data = new char[len+1];
		strcpy(original_data, str);
		original_data[len] = '\0';
	}
	void Set(int deal, const char* str) { /* deal ��̵�� */
		if (new_data) delete[] new_data;
		int len = strlen(str);
		new_data = new char[len+1];
		strcpy(new_data, str);
		new_data[len] = '\0';
	}
	const char* Get(int deal) const {/* deal ��̵�� */
		if (new_data) return new_data;
		else if (old_data) return old_data;
		return original_data;
	}
	const char* GetOriginal(int deal) const {
		return original_data;
	}
	int Deal(void) const {
		return 1;
	}
	/* ���ꥸ�ʥ뤫����Ѳ���Ĵ�� :
	** DiffOriginal ���Ѳ���ʸ����Ǽ��Ф���PatchOriginal ��
	** �Ѳ���ȿ��
	*/
	int DiffOriginalLen(void) {
		if (new_data == 0) return 0;
		return strlen(new_data)+1;
	}
	void DiffOriginal(string& data) {
		if (new_data == 0) { /* �������ʤ� */
			fprintf(stderr,"AyuSysConfigStringItem::DiffOriginal : this method must not called if not required!\n");
			return;
		}
		char* out_data = new char[strlen(new_data)*2+1];
		char* buf = out_data;
		int i;
		for (i=0; new_data[i]!=0; i++) {
			switch(new_data[i]) {
				case '?': *buf++ = '?'; *buf++ = '0'; break;
				case '"': *buf++ = '?'; *buf++ = '1'; break;
				case '\'': *buf++ = '?'; *buf++ = '2'; break;
				case ',': *buf++ = '?'; *buf++ = '3'; break;
				case '.': *buf++ = '?'; *buf++ = '4'; break;
				case ':': *buf++ = '?'; *buf++ = '5'; break;
				case ';': *buf++ = '?'; *buf++ = '6'; break;
				case '=': *buf++ = '?'; *buf++ = '7'; break;
				case '<': *buf++ = '?'; *buf++ = '8'; break;
				case '>': *buf++ = '?'; *buf++ = '9'; break;
				default: *buf++ = new_data[i]; break;
			}
		}
		*buf++ = 0;
		data += out_data;
		delete[] out_data;
		return;
	}
	const char* PatchOriginal(const char* data) {
		static char* table = "?\"',.:;=<>";
		if (new_data) delete[] new_data;
		if (old_data) delete[] old_data;
		new_data = 0; old_data = 0;
		new_data = new char[1024];
		int i,j = 0;
		for (i=0; i<1020; i++) {
			switch(data[j]) {
			case '?':
				if (data[j+1] >= '0' && data[j+1] <= '9') {
					new_data[i] = table[ data[j+1] - '0'];
					j += 2;
					break;
				}
			case '"': case '\'': case ',': case '.': case ':':
			case ';': case '=':  case '<': case '>':
				goto for_end;
			default: new_data[i] = data[j++]; break;
			}
		}
	for_end:
		new_data[i] = 0;
		return data;
	}
	void SetOriginal(void) {
		if (new_data) delete[] new_data;
		if (old_data) delete[] old_data;
		new_data = 0; old_data = 0;
	}
	void Dump(FILE* f) const {
		if (original_data) fprintf(f, "original %s ",original_data);
		if (old_data) fprintf(f, "old_data %s ",old_data);
		if (new_data) fprintf(f, "new_data %s ",new_data);
		fprintf(f, "\n");
	}
};

/************************************************
** AyuSysConfigIntlistItem
** �������ǡ����Ȥ��Ƥ���������
*/
class AyuSysConfigIntlistItem {
	int item_deal;
	int* original_data;
	int* old_data;
	int* new_data;
public:
	AyuSysConfigIntlistItem(void) {
		item_deal = 0;
		original_data = 0;
		old_data = 0;
		new_data = 0;
	}
	AyuSysConfigIntlistItem(const AyuSysConfigIntlistItem& o) {
		item_deal = o.item_deal;
		original_data = 0; old_data = 0; new_data = 0;
		if (o.original_data) {
			original_data = new int[item_deal];
			memcpy(original_data, o.original_data, sizeof(int)*item_deal);
		}
		if (o.old_data) {
			old_data = new int[item_deal];
			memcpy(old_data, o.old_data, sizeof(int)*item_deal);
		}
		if (o.new_data) {
			new_data = new int[item_deal];
			memcpy(new_data, o.new_data, sizeof(int)*item_deal);
		}
	}
	/* ���ꡧInit �ǽ������Set ���ѹ���Get ���ѹ���ͥ�褷�Ƽ��Ф� */
	void Init(int deal, const int* list) { /* deal ��̵�� */
		if (original_data) delete[] original_data;
		original_data = 0;
		if (deal <= 0) {
			item_deal = 0; return;
		}
		item_deal = deal;
		original_data = new int[item_deal];
		memcpy(original_data, list, sizeof(int)*deal);
	}
	void Set(int deal, const int* list) { /* deal ��̵�� */
		item_deal = deal;
		if (new_data) delete[] new_data;
		new_data = new int[item_deal];
		memcpy(new_data, list, sizeof(int)*item_deal);
	}
	const int* Get(int deal) const {/* deal ��̵�� */
		if (item_deal == 0) return 0;
		if (deal > item_deal) {
			fprintf(stderr,"AyuSysConfigIntlistItem::Get : invalid items deal %d (correct: %d)\n",deal,item_deal);
			return 0;
		}
		if (new_data) return new_data;
		else if (old_data) return old_data;
		return original_data;
	}
	const int* GetOriginal(int deal) const {/* deal ��̵�� */
		if (item_deal == 0) return 0;
		if (deal > item_deal) {
			fprintf(stderr,"AyuSysConfigIntlistItem::Get : invalid items deal %d (correct: %d)\n",deal,item_deal);
			return 0;
		}
		return original_data;
	}
	int Deal(void) const {
		return item_deal;
	}
	/* ���ꥸ�ʥ뤫����Ѳ���Ĵ�� :
	** DiffOriginal ���Ѳ���ʸ����Ǽ��Ф���PatchOriginal ��
	** �Ѳ���ȿ��
	*/
	int DiffOriginalLen(void) {
		if (new_data == 0) return 0;
		return  12 * item_deal + 1;
	}
	void DiffOriginal(string& data) {
		if (new_data == 0) { /* �������ʤ� */
			fprintf(stderr,"AyuSysConfigStringItem::DiffOriginal : this method must not called if not required!\n");
			return;
		}
		int i; char buf[1024];
		for (i=0; i<item_deal; i++) {
			sprintf(buf, "%d,",new_data[i]);
			data += buf;
		}
		return;
	}
	const char* PatchOriginal(const char* data) {
		if (old_data) delete[] old_data;
		if (new_data) delete[] new_data;
		old_data = 0; new_data = 0;
		new_data = new int[item_deal];
		int i;
		for (i=0; i<item_deal; i++) {
			new_data[i] = atoi(data);
			if (strchr(data, ',') == 0) break;
			data = strchr(data, ',') + 1;
		}
		return data;
	}
	void SetOriginal(void) {
		if (new_data) delete[] new_data;
		if (old_data) delete[] old_data;
		new_data = 0; old_data = 0;
	}
	void Dump(FILE* f) const {
		fprintf(f, "item deal %d, ",item_deal);
		if (original_data) {
			fprintf(f, "(%d", original_data[0]);
			int i;for (i=1; i<item_deal; i++) {
				fprintf(f, ",%d",original_data[i]);
			}
			fprintf(f, ") ");
		}
		if (old_data) {
			fprintf(f, "old %08x(%d", (unsigned int)(old_data), old_data[0]);
			int i;for (i=1; i<item_deal; i++) {
				fprintf(f, ",%d",old_data[i]);
			}
			fprintf(f, ") ");
		}
		if (new_data) {
			fprintf(f, "new %08x(%d", (unsigned int)(new_data), new_data[0]);
			int i;for (i=1; i<item_deal; i++) {
				fprintf(f, ",%d",new_data[i]);
			}
			fprintf(f, ") ");
		}
		fprintf(f, "\n");
	}
};

// template map<HashStr, AyuSysConfigStringItem>;
// template map<HashStr, AyuSysConfigIntlistItem>;

/************************************************
** AyuSysConfigItem
** �ǡ���̾ -> �ǡ������Τ� map �ȡ�map ���Τ�
** �͡�������Ԥ�����Υ᥽�å�
*/

template<class ItemType, class DataType> class AyuSysConfigItem {
	typedef map<HashStr,ItemType> maptype;
	typedef typename maptype::iterator mapiterator;
	typedef typename maptype::const_iterator const_mapiterator;
	maptype data;
public:
	void SetOrig(HashStr& name, int deal, const DataType* str) {
		if (str == 0) return; /* ̵�� */
		data[name].Init(deal, str);
	}
	void Set(HashStr& name, int deal, const DataType* new_data) {
		if (new_data == 0) return; /* ̵�� */
		/* ����򸡺� */
		mapiterator it = data.find(name);
		/* ���꤬������˸��Ĥ���ʤ��ʤ鼺�� */
		if (it == data.end()) {
			fprintf(stderr,"AyuSysConfigItem::Set : there is no '%s' parameter\n",name.c_str());
			return;
		}
		/* ������ѹ� */
		it->second.Set(deal, new_data);
	}
	/* �����������ͥ�褷���֤� */
	const DataType* Get(int deal, HashStr& name) const {
		const_mapiterator it = data.find(name);
		if (it == data.end()) return 0;
		return it->second.Get(deal);
	}
	const DataType* GetOriginal(int deal, HashStr& name) const {
		const_mapiterator it = data.find(name);
		if (it == data.end()) return 0;
		return it->second.GetOriginal(deal);
	}
	int Deal(HashStr& name) const {
		const_mapiterator it = data.find(name);
		if (it == data.end()) return 0;
		return it->second.Deal();
	}
	/* ���ꥸ�ʥ뤫����Ѳ���Ĵ�� :
	** DiffOriginal ���Ѳ���ʸ����Ǽ��Ф���PatchOriginal ��
	** �Ѳ���ȿ��
	*/
	void DiffOriginal(string& ret_str) {
		mapiterator it = data.begin();
		for (; it != data.end(); it++) {
			int len = it->second.DiffOriginalLen();
			if (len) {
				ret_str += it->first.c_str();
				ret_str += "=";
				it->second.DiffOriginal(ret_str);
				ret_str += ";";
			}
		}
		ret_str += ";";
		return;
	}
	const char* PatchOriginal(const char* diff_data) {
		while(*diff_data != ';') {
			char name[1024];
			const char* data_start = strchr(diff_data, '=');
			if (data_start == 0) break;
			strncpy(name, diff_data, data_start-diff_data);
			name[data_start-diff_data] = 0;
			data_start++;
			mapiterator it = data.find(name);
			if (it != data.end()) {
				diff_data = data_start;
				it->second.PatchOriginal(diff_data);
			}
			diff_data = strchr(diff_data, ';');
			if (diff_data) diff_data++;
		}
		if (*diff_data == ';') {
			diff_data++;
		} else {
			fprintf(stderr,"AyusysConfigItem::PatchOriginal: invalid data %s\n",diff_data);
		}
		return diff_data;
	}
	void SetOriginal(void) {
		mapiterator it = data.begin();
		for (; it != data.end(); it++) {
			it->second.SetOriginal();
		}
	}
	void Dump(FILE* f) const {
		const_mapiterator it = data.begin();
		for (; it != data.end(); it++) {
			fprintf(f, "name %s: ",it->first.c_str());
			it->second.Dump(f);
		}
	}
};
// template AyuSysConfigItem<AyuSysConfigStringItem, char>;
// template AyuSysConfigItem<AyuSysConfigIntlistItem, int>;

/************************************************/
/* ��å� */
struct AyuSysConfigString {
	AyuSysConfigItem<AyuSysConfigStringItem,char> orig;
	void Dump(FILE* f) const {
		fprintf(f, "string config:\n");
		orig.Dump(f);
	}
};
struct AyuSysConfigIntlist {
	AyuSysConfigItem<AyuSysConfigIntlistItem, int> orig;
	void Dump(FILE* f) const {
		fprintf(f, "integer array config:\n");
		orig.Dump(f);
	}
};

/************************************************/
/* AyuSysConfig ���饹 */
int AyuSysConfig::SearchParam(const char* name) const{
	HashStr str(name);
	if (str_config->orig.Get(1, str)) return 1; /* char* �Υѥ�᡼�� */
	else if (int_config->orig.Get(1, str)) return 2; /* int �Υѥ�᡼�� */
	/* XXX.015.XXX ����Υ���̾�� XXX.000.XXX �η��˵��ʲ����ƺƸ��� */
	char name_copy[1024];
	strncpy(name_copy, name, 1000);
	name_copy[1000] = 0;
	char* s;
	for (s=name_copy; s != 0; s = strchr(s,'.')) {
		if (isdigit(s[1]) && isdigit(s[2]) && isdigit(s[3])) {
			s[1] = '0'; s[2] = '0'; s[3] = '0';
		}
		s++;
	}
	HashStr str2(name_copy);
	if (str_config->orig.Get(1, str2)) return 1; /* char* �Υѥ�᡼�� */
	else if (int_config->orig.Get(1, str2)) return 2; /* int �Υѥ�᡼�� */
	else return 0;
}
const char* AyuSysConfig::GetParaStr(const char* name) const{
	HashStr str(name);
	const char* ret = str_config->orig.Get(1,str);
	if (ret == 0) {
		// fprintf(stderr,"Cannot find config name '%s'\n",name);
	}
	return ret;
}
int AyuSysConfig::GetParam(const char* name, int deal, ...) const{
	HashStr str(name);
	va_list va; int i;
	const int* vars = int_config->orig.Get(deal, str);
	if (vars == 0) {
		// fprintf(stderr,"Cannot find config name '%s'\n",name);
		va_start(va, deal);
		for (i=0; i<deal; i++) {
			int* var = va_arg(va, int*);
			if (var) *var = 0;
		}
		return -1;
	} else {
		va_start(va, deal);
		for (i=0; i<deal; i++) {
			int* var = va_arg(va, int*);
			if (var) *var = vars[i];
		}
	}
	return 0;
}
int AyuSysConfig::GetOriginalParam(const char* name, int deal, ...) const{
	HashStr str(name);
	va_list va; int i;
	const int* vars = int_config->orig.GetOriginal(deal, str);
	if (vars == 0) {
		// fprintf(stderr,"Cannot find config name '%s'\n",name);
		va_start(va, deal);
		for (i=0; i<deal; i++) {
			int* var = va_arg(va, int*);
			if (var) *var = 0;
		}
		return -1;
	} else {
		va_start(va, deal);
		for (i=0; i<deal; i++) {
			int* var = va_arg(va, int*);
			if (var) *var = vars[i];
		}
	}
	return 0;
}
const int* AyuSysConfig::GetParamArray(const char* name, int& deal) const{
	HashStr str(name);
	if (int_config->orig.Deal(str) == 0) { deal = 0; return 0; }
	deal = int_config->orig.Deal(str);
	const int* vars = int_config->orig.Get(deal, str);
	if (vars == 0) { deal = 0; return 0; }
	return vars;
}
void AyuSysConfig::SetParaStr(const char* name, const char* var) {
	HashStr str(name);
	dirty_flag = 1; change_flag = 1;
	str_config->orig.Set(str, 1, var);
}
void AyuSysConfig::SetParam(const char* name, int deal, ...) {
	if (deal >= MAXVARS) return ;
	HashStr str(name);
	int vars[MAXVARS]; va_list va; int i;
	va_start(va, deal);
	for (i=0; i<deal; i++) vars[i] = va_arg(va, int);
	int_config->orig.Set(str, deal, vars);
	dirty_flag = 1; change_flag = 1;
	return;
}
void AyuSysConfig::SetOrigParaStr(const char* name, const char* var) {
	HashStr str(name);
	str_config->orig.SetOrig(str, 1, var);
	change_flag = 1;
}
void AyuSysConfig::SetOrigParam(const char* name, int deal, ...) {
	if (deal >= MAXVARS) return;
	HashStr str(name);
	int vars[MAXVARS]; va_list va; int i;
	va_start(va, deal);
	for(i=0; i<deal; i++) vars[i] = va_arg(va, int);
	int_config->orig.SetOrig(str, deal, vars);
	change_flag = 1;
}
void AyuSysConfig::SetOrigParamArray(const char* name, int deal, int* array) {
	HashStr str(name);
	int_config->orig.SetOrig(str, deal, array);
}
void AyuSysConfig::SetOriginal(void) {
	/* ���Ƥ�����򸵤��᤹ */
	str_config->orig.SetOriginal();
	int_config->orig.SetOriginal();
	change_flag = 1;
}
void AyuSysConfig::DiffOriginal(string& data) {
	str_config->orig.DiffOriginal(data);
	int_config->orig.DiffOriginal(data);
	return;
}
const char* AyuSysConfig::PatchOriginal(const char* data) {
	data = str_config->orig.PatchOriginal(data);
	data = int_config->orig.PatchOriginal(data);
	return data;
}

void AyuSysConfig::Dump(FILE* f) const {
	str_config->Dump(f);
	int_config->Dump(f);
}

/************************************************
** AyuSysConfig �Υ��󥹥ȥ饯����
** ���Ƥ� config ���ܤ���������
*/
AyuSysConfig::AyuSysConfig(void) {
	int i;

	change_flag = 1; dirty_flag = 0;
	str_config = new AyuSysConfigString;
	int_config = new AyuSysConfigIntlist;

	/****** ʸ���� *******/
	SetOrigParaStr("#WAKUPDT", "GRDAT");	/* �ȡ��ޥ�����������ʤɤβ����ե����� */
	SetOrigParaStr("#REGNAME", "xclannad");	/* �쥸���ȥ�̾�������֥ե�����κ����˻Ȥ� */
	SetOrigParaStr("#CAPTION", "xclannad");	/* ������ɥ��Υ����ȥ� */
	SetOrigParaStr("#SAVENAME","SAVE.INI");	/* �����֥ե������̾�� */
	SetOrigParaStr("#SAVETITLE", "This is save file"); /* �����֥ե��������Ƭ��ʸ���� */
	SetOrigParaStr("#SAVENOTITLE", "-----------------"); /* �Ȥ��Ƥʤ������֥ǡ�����̾�� */
	SetOrigParaStr("#CGM_FILE", "MODE.CGM");/* CG mode �����꤬��¸���줿�ե�����̾ */
	SetOrigParaStr("#CGTABLE_FILE", "MODE.CGM");/* CG mode �����꤬��¸���줿�ե�����̾ */

	SetOrigParaStr("#WAKU.000.000.NAME", ""); // �ƥ����ȥ�����ɥ�����������̾
	SetOrigParaStr("#WAKU.000.000.BACK", ""); // �ƥ����ȥ�����ɥ��Υƥ������طʲ���̾
	SetOrigParaStr("#WAKU.000.000.BTN", ""); // �ƥ����ȥ�����ɥ��Υܥ������̾

	SetOrigParaStr("#MOUSE_CURSOR.000.NAME", ""); // �ޥ�����������Υե�����̾
	SetOrigParaStr("#CURSOR.000.NAME", ""); // �꥿���󥫡�����Υե�����̾
	SetOrigParaStr("#SELBTN.000.NAME", ""); // ������ط�
	SetOrigParaStr("#SELBTN.000.BACK", ""); // ������ط�

	char name_str[8] = "#NAME.A";
	for (i='A'; i<='Z'; i++) {
		name_str[6] = i;
		SetOrigParaStr(name_str, "");
	}

	/****** ������ *******/
	SetOrigParam("#CANCELCALL", 2, 0,0); /* ����󥻥�ܥ���(������å�)�����Ȥ��˸ƤӽФ���륵�֥롼�����ֹ�(��˥塼) */
	SetOrigParam("#COM2_TITLE", 1, 1); /* �� */
	SetOrigParam("#COM2_TITLE_COLOR", 1, 2); /* ����西���ȥ�ο� */
	SetOrigParam("#COM2_TITLE_INDENT", 1, 2); /* �� */
	SetOrigParam("#SAVEFILETIME", 1, 24); /* �����֤�����ο� */
	SetOrigParam("#SEEN_START", 1, 0); /* ������򳫻Ϥ��륷�ʥꥪ�ֹ� */
	SetOrigParam("#SEEN_SRT", 1, 0); /* ������򳫻Ϥ��륷�ʥꥪ�ֹ�(���������繥��) */
	SetOrigParam("#SEEN_MENU",  1, 0); /* ��˥塼�Υ��ʥꥪ�ֹ� */
	SetOrigParam("#SEEN_TEXT_CURRENT",  1, 0); /* seen.txt �� root directory ���֤��� */
	SetOrigParam("#FADE_TIME", 1, 40); /* ���̤Υե����ɡ������Ȥ�®�� */
	SetOrigParam("#NVL_SYSTEM",1, 0); /* �ƥ����ȥ�����ɥ��������̤��ݤ� */
	SetOrigParam("#WINDOW_ATTR", 5, -1, 128,128, 190, 0); /* �ƥ����ȥ�����ɥ��ο� */
	SetOrigParam("#WINDOW_ATTR_AREA", 4, 4,4,4,4); /* �ƥ����ȥ�����ɥ����ϰ� */
	SetOrigParam("#WINDOW_ATTR_TYPE", 1, 0); /* �ƥ����ȥ�����ɥ���ȾƩ���ˤ��뤫 */
	SetOrigParam("#WINDOW_MSG_POS", 2, 22, 350); /* �ƥ����ȥ�����ɥ��ΰ��� */
	SetOrigParam("#WINDOW_COM_POS", 2,450, 250); /* ���򥦥���ɥ��ΰ��� */
	SetOrigParam("#WINDOW_GRP_POS", 2, 16, 100); /* �ʤˤ��Υ�����ɥ��ΰ��� */
	SetOrigParam("#WINDOW_SUB_POS", 2, 48, 100); /* �ʤˤ��Υ�����ɥ��ΰ��� */
	SetOrigParam("#WINDOW_SYS_POS", 2, 32, 100); /* �ʤˤ��Υ�����ɥ��ΰ��� */
	SetOrigParam("#WINDOW_WAKU_TYPE", 1, 0); /* �ƥ����ȥ�����ɥ����Ȥμ��ࡣxkanon �ȼ����� */
	SetOrigParam("#RETN_CONT", 1, 16); /* �꥿���󥫡�����ο� */
	SetOrigParam("#RETN_SPEED",1,100); /* �꥿���󥫡������ư��®�� */
	SetOrigParam("#RETN_XSIZE", 1, 16); /* �꥿���󥫡�������礭�� */
	SetOrigParam("#RETN_YSIZE", 1, 16); /* �꥿���󥫡�������礭�� */
	SetOrigParam("#FONT_SIZE", 1, 26); /* �ե���Ȥ��礭�� */
	SetOrigParam("#FONT_WEIGHT", 1, 100); /* �ե���Ȥ� weight */
	SetOrigParam("#MSG_MOJI_SIZE", 2, 12, 29); /* ʸ�����礭��(Ⱦ��) */
	SetOrigParam("#MESSAGE_SIZE", 2, 23, 3); /* ��å�����������ɥ���ʸ���� */
	SetOrigParam("#COM_MESSAGE_SIZE", 2, 23, 3); /* ��å�����������ɥ���ʸ���� */
	SetOrigParam("#INIT_MESSAGE_SPEED", 1, 30); /* �ƥ�����ɽ��®�� */
	SetOrigParam("#INIT_MESSAGE_SPEED_MOD", 1, 0); /* �ƥ�����ɽ�� no wait */
	SetOrigParam("#MESSAGE_KEY_WAIT_USE", 1, 0); /* �ƥ����ȿʹԥ����ȥ⡼�� */
	SetOrigParam("#MESSAGE_KEY_WAIT_TIME", 1, 1500); /* �����ȥ⡼�ɤǤΥ����Ԥ����� */

	SetOrigParam("#GRP_DC_TIMES", 1, 4); /* ΢���̤ο� */
	SetOrigParam("#MUSIC_LINEAR_PAC",1,0); /* PCM �ǡ����� 8bit -> 16bit �Ѵ���Ԥ��� */
	SetOrigParam("#MUSIC_TYPE",1,0); /* PCM �ǡ����μ��� */
	SetOrigParam("#WINDOW_MSGBK_BOX",1,0); /* �Хå����ѤΥܥ��� */
	SetOrigParam("#WINDOW_MSGBK_LBOX_POS",4,15,7,8,0); /* �Хå����ѤΥܥ���(��)�ΰ��� */
	SetOrigParam("#WINDOW_MSGBK_RBOX_POS",4,7,7,0,0); /* �Хå����ѤΥܥ���(��)�ΰ��� */
	SetOrigParam("#MSGBK_MOD",1,0); /* �Хå����ѤΥܥ������Ѥ��뤫 */

	SetOrigParam("#WAKU.000.000.TYPE", 1, 5);
	SetOrigParam("#WAKU.000.000.MOVE_BOX", 5, 0, 0, 0, 0, 0); // �ƥ����ȥ�����ɥ��ΰ�ư�ѥܥ������
	SetOrigParam("#WAKU.000.000.CLEAR_BOX", 5, 0, 0, 0, 0, 0); //    ����õ��ѥܥ������
	SetOrigParam("#WAKU.000.000.READJUMP_BOX", 5, 0, 0, 0, 0, 0); // �����å��ѥܥ������
	SetOrigParam("#WAKU.000.000.AUTOMODE_BOX", 5, 0, 0, 0, 0, 0); // �������ѥܥ������
	SetOrigParam("#WAKU.000.000.MSGBK_BOX", 5, 0, 0, 0, 0, 0); // �Хå����ܥ������
	SetOrigParam("#WAKU.000.000.MSGBKLEFT_BOX", 5, 0, 0, 0, 0, 0); // �Хå����ʿʤ��˥ܥ������
	SetOrigParam("#WAKU.000.000.MSGBKRIGHT_BOX", 5, 0, 0, 0, 0, 0); // �Хå��������˥ܥ������
	SetOrigParam("#WAKU.000.000.EXBTN_000_BOX", 5, 0, 0, 0, 0, 0); // ����¾�ܥ��󣰰���
	SetOrigParam("#WAKU.000.000.EXBTN_001_BOX", 5, 0, 0, 0, 0, 0); // ����¾�ܥ��󣱰���
	SetOrigParam("#WAKU.000.000.EXBTN_002_BOX", 5, 0, 0, 0, 0, 0); // ����¾�ܥ��󣲰���

	SetOrigParam("#WINDOW.000.MOJI_SIZE", 1, 21); // ʸ��������
	SetOrigParam("#WINDOW.000.MOJI_REP", 2, -1, 2); // ʸ����;͵
	SetOrigParam("#WINDOW.000.MOJI_CNT", 2, 20, 3); // ������ɥ����ʸ����
	SetOrigParam("#WINDOW.000.MOJI_POS", 4, 100, 0, 180, 40); // �ƥ����Ȱ��֡�3���ܤ�x ��1���ܤ�y�餷��
	SetOrigParam("#WINDOW.000.MOJI_SHADOW", 1, 0); // ʸ���˱Ƥ��դ��뤫
	SetOrigParam("#WINDOW.000.LUBY_SIZE", 1, 8); // ��Ӥ�ʸ��������
	SetOrigParam("#WINDOW.000.MOJI_MIN", 2, 8, 1); // ʸ��Ʊ�Τη�֡�
	SetOrigParam("#WINDOW.000.SELCOM_USE", 1, 0); // �����μ�����ˡ
	SetOrigParam("#WINDOW.000.POS", 4, 100, 0, 0, 260); // ������ɥ�����
	SetOrigParam("#WINDOW.000.ATTR_MOD", 1, 0); // ������ɥ���
	SetOrigParam("#WINDOW.000.ATTR", 5, -1, -1, -1, -1, -1); // ������ɥ���
	/* SELCOM �Ϥ褯�狼���Τ�̵�� */
	SetOrigParam("#WINDOW.000.OPEN_ANM_MOD", 1, 0); // ������ɥ��򳫤��Ȥ��θ��̤餷��
	SetOrigParam("#WINDOW.000.OPEN_ANM_TIME", 1, 500);
	SetOrigParam("#WINDOW.000.CLOSE_ANM_MOD", 1, 0); // ������ɥ����Ĥ���Ȥ��θ��̤餷��
	SetOrigParam("#WINDOW.000.CLOSE_ANM_TIME", 1, 500);
	SetOrigParam("#WINDOW.000.WAKU_SETNO", 1, 0); // �Ȥμ���
	SetOrigParam("#WINDOW.000.MOVE_USE", 1, 0); // ������ɥ��Ȱ�ư�ܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.CLEAR_USE", 1, 0); // ������ɥ��Ⱦõ�ܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.READJUMP_USE", 1, 0); // �����åץܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.AUTOMODE_USE", 1, 0); // �����åץܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.MSGBK_USE", 1, 0); // �Хå����ܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.MSGBKLEFT_USE", 1, 0); // �Хå����ʿʤ�˥ܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.MSGBKRIGHT_USE", 1, 0); // �Хå��������˥ܥ�����Ѥβ���
	SetOrigParam("#WINDOW.000.EXBTN_000_USE", 1, 0); // ����¾�ܥ��󣰻��Ѥβ���
	SetOrigParam("#WINDOW.000.EXBTN_001_USE", 1, 0); // ����¾�ܥ��󣱻��Ѥβ���
	SetOrigParam("#WINDOW.000.EXBTN_002_USE", 1, 0); // ����¾�ܥ��󣲻��Ѥβ���
	SetOrigParam("#WINDOW.000.NAME_MOD", 1, 0); // ̾��������ɥ������ӻ��Ѥ��뤫
	SetOrigParam("#WINDOW.000.NAME_MOJI_SIZE", 1, 20); // ̾���ե���ȤΥ�����
	SetOrigParam("#WINDOW.000.NAME_MOJI_POS", 2, 0, 0); // ̾��������ɥ���ʸ���ΰ���
	SetOrigParam("#WINDOW.000.NAME_MOJI_MIN", 1, 0); // ̾��������ɥ�����
	SetOrigParam("#WINDOW.000.NAME_CENTERING", 1, 1); // ̾���Υ��󥿥�󥰤�̵ͭ
	SetOrigParam("#WINDOW.000.NAME_POS", 2, 159, 78); // ̾��������ɥ����֡ʺ������֤餷����
	SetOrigParam("#WINDOW.000.NAME_WAKU_SETNO", 1, -1); // ̾��������ɥ����֡ʺ������֤餷����
	SetOrigParam("#WINDOW.000.FACE.000", 5, 0, 0, 1, 1, 1); // �饦����ɥ�����(�Ϥᣲ�Ĥ�x,y��MOJI_POS��������а��֤ʤΤ����)
	SetOrigParam("#WINDOW.000.KEYCUR_MOD", 3, 0, 0, 0); // �꥿���󥫡�����ΰ���


	SetOrigParam("#CURSOR.000.SIZE", 2, 0, 0); // �꥿���󥫡�������礭��
	SetOrigParam("#CURSOR.000.CONT", 1, 50); // �꥿���󥫡�����η����֤���
	SetOrigParam("#CURSOR.000.SPEED", 1, 1000); // �֥�󥯤���®��

	SetOrigParam("#SELBTN.000.BASEPOS", 2, 0, 0); // ����襦����ɥ��ΰ���
	SetOrigParam("#SELBTN.000.REPPOS", 2, 0, 50); // ����襦����ɥ��μ��ΰ��֡����С�
	SetOrigParam("#SELBTN.000.MOJISIZE", 4, 26, 0,0,0); // ʸ�����礭��
	SetOrigParam("#SELBTN.000.MOJIDEFAULTCOL", 1, 0); // ���������ʸ����
	SetOrigParam("#SELBTN.000.MOJISELECTCOL", 1, 0); // �������ʸ����

	SetOrigParam("#COLOR_TABLE.000", 3, 255,255,255);
	SetOrigParam("#SHAKE.000", 3, 0,0,0);

	SetOrigParam("#SELR.000",16,0,0,640,480,0,0,500,50,0,0,0,0,0,0,255,0);
	SetOrigParam("#SEL.000", 15,0,0,639,479,0,0, 32, 4,0,0,0,0,0,0,0); 

	SetOrigParam("#SCREENSIZE_MOD", 1, 0); /* 0 = 640x480; 1 = 800x600 */
}

static int SplitVar(const char* str, int* ret_var, int ret_size) {
	/* , ���뤤�� ),:( �򥻥ѥ졼���Ȥ��ơ�-?[0-9]+ ��
	** �ե����ޥåȤο�������ɤ߹��ࡣ��Ƭ�� (�������� ) ���դ����롣
	** (),-[0-9] �ʳ���ʸ�������ä��餽���ǽ�λ
	** ����줿�ǡ��������֤�
	*/
	if (*str == '(') str++;
	int i; for (i=0; i<ret_size; i++) {
		int c; int is_positive = 1;
		/* ���ѥ졼�����ɤ����Ф� */
		c = *str;
		if (c == ',' || c == ':') {
			str++;
		} else if (c == ')' && str[1] == '(') {
			str += 2;
		}
		/* - �� parse */
		c = *str;
		if (c == '-' && isdigit(str[1])) {
			is_positive = -1; str++;
		} else if (! isdigit(c)) {
			return i; /* �۾��ʸ���򸫤Ĥ�������λ */
		}
		int number = 0;
		/* �����ɤ߹��� */
		while(isdigit( (c=*str) )) {
			number *= 10;
			number += c-'0';
			str++;
		}
		ret_var[i] = is_positive * number;
	}
	return i;
}
/* ����줿���ΰ��������롣-1 �ʤ饨�顼�������� */
static inline int SplitVar(const char* str, int& var1) {
	if (SplitVar(str, &var1, 1) != 1) return -1;
	return 0;
}
static inline int SplitVar(const char* str, int& var1, int& var2) {
	int vars[2];
	if (SplitVar(str, vars, 2) != 2) return -1;
	var1 = vars[0]; var2 = vars[1];
	return 0;
}
static inline int SplitVar(const char* str, int& var1, int& var2, int& var3) {
	int vars[3];
	if (SplitVar(str, vars, 3) != 3) return -1;
	var1 = vars[0]; var2 = vars[1]; var3 = vars[2];
	return 0;
}
static inline int SplitVar(const char* str, int& var1, int& var2, int& var3, int& var4) {
	int vars[4];
	if (SplitVar(str, vars, 4) != 4) return -1;
	var1 = vars[0]; var2 = vars[1]; var3 = vars[2]; var4 = vars[3];
	return 0;
}

bool AyuSysConfig::LoadInitFile(void)
{
	char buf[1024]; int i;
	char* tokens[MAXTOKEN]; int token_deal; int buf_ptr;
	int numbers[MAXVARS];

	ARCINFO* info = file_searcher.Find(FILESEARCH::ROOT, "gameexe.ini");
	if (info == NULL) return false;
	int size = info->Size();
	unsigned char* buf_orig = (unsigned char*)info->Read();
	if (size <= 0 || buf_orig == 0) {
		delete info; return false;
	}
	unsigned char* buf_end = buf_orig + size;
	int line_count = 0;
	while(buf_orig < buf_end) {
		/* buf_orig �������ɤ߹��� */
		/* ���κݤˡ�
		** ��Ƭ�� # �ʳ��ʤ鼡�ιԤޤǤȤФ�
		** ��"" ���Υ��ڡ�����TAB����Ȥ�������ʸ�� (0x20 �ʲ���ASCII������)����
		** ��= �Ƕ��ڤ롣���ڤ�Ϻ����10�Ĥǡ�tokens �����������
		** �ʤɤ�����Ԥ�
		*/

		/* # �����å� */
		if (*buf_orig != '#') {
			/* ���� '\n' �ޤ��ɤ����Ф� */
			while(buf_orig < buf_end &&
				*buf_orig != '\n' && *buf_orig != '\r') buf_orig++;
			if (buf_orig < buf_end-1 && *buf_orig == '\r' && buf_orig[1] == '\n') buf_orig += 2;
			else if (*buf_orig == '\r' || *buf_orig == '\n') buf_orig++;
			line_count++;
			continue;
		}
		/* ����� */
		token_deal = 1; tokens[0] = buf; buf_ptr = 0;
		int in_quote = 0;

		while(buf_orig < buf_end && buf_ptr < 1023) {
			if (in_quote) {
				/* "" ���� */
				int c = *buf_orig;
				if (c == '\n' || c == '\r') {
					break;
				} else if (c == '\"') {
					in_quote = 0;
				} else {
					buf[buf_ptr++] = c;
				}
				buf_orig++;
			} else { /* quote ����Ƥʤ� */
				/* ����ʸ�����ɤ����Ф� */
				while(*buf_orig <= 0x20 && buf_orig < buf_end &&
					*buf_orig != '\n' && *buf_orig != '\r') buf_orig++;
				int c = *buf_orig;
				if (c == '\n' || c == '\r') break;
				/* = �ʤ鼡�� token */
				if (c == '=') {
					c = 0; tokens[token_deal++] = buf+buf_ptr+1;
					if (token_deal >= MAXTOKEN) break;
				} else if (c == '\"') {
					in_quote = 1; buf_orig++; continue;
				}
				buf[buf_ptr++] = c;
				buf_orig++;
			}
		}
		buf[buf_ptr] = '\0';
		/* ������ \r\n ��õ� */
		if (buf_orig < buf_end-1 && buf_orig[0] == '\r' && buf_orig[1] == '\n') buf_orig += 2;
		else if (buf_orig < buf_end && (buf_orig[0] == '\r' || buf_orig[0] == '\n')) buf_orig++;
		/* ɬ�פʤ� parse ���Ƥ���� */
		dprintf(("line %3d ",line_count));
		for (i=0; i<token_deal; i++) {
			dprintf(("%d:\"%s\", ",i,tokens[i]));
		}
		dprintf(("\n"));
		if (in_quote) {
			fprintf(stderr, "Warning : open quote is found while parsing gameexe.ini, line %d\n",line_count);
		}


		/* ����줿���Ƥ� parse */

		/* #NAME=<ʸ����> */
		int type = SearchParam(tokens[0]);
		if (type == 1) { /* #NAME=<ʸ����> */
			if (token_deal != 2) {
				dprintf(("Parse error, line %d, %s\n",line_count, tokens[0]));
				goto parse_error;
			}
			SetOrigParaStr(tokens[0], tokens[1]);
			goto parse_end;
		} else if (type == 2) { /* #NAME=<������> */
			if (token_deal != 2) {
				dprintf(("Parse error, line %d, %s\n",line_count, tokens[0]));
				goto parse_error;
			}
			int number_deal = SplitVar(tokens[1], numbers, MAXVARS);
			SetOrigParamArray(tokens[0], number_deal, numbers);
			goto parse_end;
		}
		/* ����Ū������ʳ� : cdrom track �ʤ� */
		if (strncmp(tokens[0],"#NAME.", 6) == 0) {
			if (token_deal != 2) goto parse_error;
			SetOrigParaStr(tokens[0], tokens[1]);
			goto parse_end;
		} else if (strncmp(tokens[0],"#DIRC.",6) == 0) {
			if (token_deal != 3) goto parse_error;
			/* �ե���������λ��� */
			FILESEARCH::FILETYPE type;
			char* name = tokens[0]+6;
			if (strcmp(name, "PDT") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "G00") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "GRP") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "TXT") == 0) type = FILESEARCH::SCN;
			else if (strcmp(name, "ANM") == 0) type = FILESEARCH::ANM; 
			else if (strcmp(name, "ARD") == 0) type = FILESEARCH::ARD;
			else if (strcmp(name, "CUR") == 0) type = FILESEARCH::CUR;
			else if (strcmp(name, "WAV") == 0) type = FILESEARCH::WAV;
			else if (strcmp(name, "KOE") == 0) type = FILESEARCH::KOE;
			else if (strcmp(name, "GAN") == 0) type = FILESEARCH::GAN;
			else goto parse_error; /* ¾�� ALL,ROOT,MID,KOE,BGM�����֤�¸�ߤ��ʤ� */
			if (tokens[2][0] == 'N') { /* directory */
				file_searcher.SetFileInformation(type, FILESEARCH::ATYPE_DIR, tokens[1]);
				dprintf(("set file directory; type %s, directory %s\n",name,tokens[1]));
			} else if (tokens[2][0] == 'P' && tokens[2][1] == ':') { /* ���������� */
				file_searcher.SetFileInformation(type, FILESEARCH::ATYPE_ARC, tokens[2]+2);
				dprintf(("set file archive; type %s, file %s\n",name,tokens[2]+2));
			} else goto parse_error;
			goto parse_end;
		}
		if (strncmp(tokens[0],"#ADRC.",6) == 0) {
			if (token_deal != 3) goto parse_error;
			/* �ե���������λ��� */
			FILESEARCH::FILETYPE type;
			char* name = tokens[0]+6;
			if (strcmp(name, "PDT") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "G00") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "GRP") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "TXT") == 0) type = FILESEARCH::SCN;
			else if (strcmp(name, "ANM") == 0) type = FILESEARCH::ANM; 
			else if (strcmp(name, "ARD") == 0) type = FILESEARCH::ARD;
			else if (strcmp(name, "CUR") == 0) type = FILESEARCH::CUR;
			else if (strcmp(name, "WAV") == 0) type = FILESEARCH::WAV;
			else if (strcmp(name, "KOE") == 0) type = FILESEARCH::KOE;
			else if (strcmp(name, "GAN") == 0) type = FILESEARCH::GAN;
			else goto parse_error; /* ¾�� ALL,ROOT,MID,KOE,BGM�����֤�¸�ߤ��ʤ� */
			if (tokens[2][0] == 'N') { /* directory */
				file_searcher.AppendFileInformation(type, FILESEARCH::ATYPE_DIR, tokens[1]);
				dprintf(("set file directory; type %s, directory %s\n",name,tokens[1]));
			} else if (tokens[2][0] == 'P' && tokens[2][1] == ':') { /* ���������� */
				file_searcher.AppendFileInformation(type, FILESEARCH::ATYPE_ARC, tokens[2]+2);
				dprintf(("set file archive; type %s, file %s\n",name,tokens[2]+2));
			} else if (tokens[2][0] == 'R' && tokens[2][1] == ':') { /* ���컶�륢�������� */
				file_searcher.AppendFileInformation(type, FILESEARCH::ATYPE_ARC, tokens[2]+2);
				dprintf(("set file archive; type %s, file %s\n",name,tokens[2]+2));
			} else goto parse_error;
			goto parse_end;
		}
		if (strncmp(tokens[0],"#FOLDNAME.",10) == 0) {
			if (token_deal != 3) goto parse_error;
			/* �ե���������λ��� */
			FILESEARCH::FILETYPE type;
			char* name = tokens[0]+10;
			if (strcmp(name, "PDT") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "G00") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "GRP") == 0) type = FILESEARCH::PDT;
			else if (strcmp(name, "TXT") == 0) type = FILESEARCH::SCN;
			else if (strcmp(name, "ANM") == 0) type = FILESEARCH::ANM; 
			else if (strcmp(name, "ARD") == 0) type = FILESEARCH::ARD;
			else if (strcmp(name, "CUR") == 0) type = FILESEARCH::CUR;
			else if (strcmp(name, "WAV") == 0) type = FILESEARCH::WAV;
			else if (strcmp(name, "BGM") == 0) type = FILESEARCH::BGM;
			else if (strcmp(name, "GAN") == 0) type = FILESEARCH::GAN;
			else goto parse_error; /* ¾�� ALL,ROOT,MID,KOE,BGM�����֤�¸�ߤ��ʤ� */
			if (tokens[2][0] == '0') { /* directory */
				file_searcher.AppendFileInformation(type, FILESEARCH::ATYPE_DIR, tokens[1]);
				dprintf(("set file directory; type %s, directory %s\n",name,tokens[1]));
			} else if (tokens[2][0] == '1' && tokens[2][1] == ':') { /* ���������� */
				file_searcher.AppendFileInformation(type, FILESEARCH::ATYPE_SCN2k, tokens[2]+2);
				dprintf(("set file archive; type %s, file %s\n",name,tokens[2]+2));
			} else goto parse_error;
			goto parse_end;
		}
		if (strcmp(tokens[0], "#CDTRACK") == 0) {
			if (token_deal != 3) goto parse_error;
			track_name.AddCDROM(tokens[2], atoi(tokens[1]));
			dprintf(("Set CDTRACK, name %s, track %d\n",tokens[2], atoi(tokens[1])));
			goto parse_end;
		}
		if (strcmp(tokens[0], "#DSTRACK") == 0) {
			/* #DSTRACK=00000000-99999000-00782556="filename"   ="name"       */
			/* #DSTRACK=00000000-99999000-00782556="name"       */
			/* ����ȡ�����Σ��Ĥ�Υѥ�᡼��������ʷ����֤��λ��κ������ϰ��֡� */
			int start_pt = 0;
			const char* tk1 = strchr(tokens[1], '-');
			const char* tk2 = 0;
			if (tk1 && *tk1) tk2 = strchr(tk1+1, '-');
			if (tk2 && *tk2) start_pt = atoi(tk2+1);
			if (token_deal == 3) {
				track_name.AddWave(tokens[2], tokens[2], start_pt);
				dprintf(("Set Wave track, name %s\n",tokens[2]));
			} else if (token_deal == 4) {
				track_name.AddWave(tokens[3], tokens[2], start_pt);
				dprintf(("Set Wave track, name %s, file %s\n",tokens[3], tokens[2]));
			} else goto parse_error;
			goto parse_end;
		}
		if (strncmp(tokens[0], "#SE.", 4) == 0) {
			/* SE.XXX="XXX"=X */
			if (token_deal == 2) {
				track_name.AddSE(atoi(tokens[0]+4), tokens[1]);
			} else if (token_deal == 3) {
				if (atoi(tokens[2]) != 0) {
					track_name.AddSE(atoi(tokens[0]+4), tokens[1]);
				}
			}
			dprintf(("Set SE %d, name %s\n",atoi(tokens[0]+4), tokens[1]));
			goto parse_end;
		}
		/* ������ܤ����Ĥ���ʤ��ä� */
		dprintf(("Cannot find configuration name: %s\n",tokens[0]));
	parse_error:
	parse_end:
		line_count++;
	}
	delete info;
	/* �ǥե���ȤΥ��ץ�������ꤹ�� */
	// set_game(GetParaStr("#REGNAME"), *this);
	return true;
}

TrackName::TrackName(void) {
	deal = 1;
	track = new char*[deal];
	track_wave = new char*[deal];
	track_num = new int[deal];
	track_start = new int[deal];
	int i; for (i=0; i<deal; i++) track[i] = 0;
	for (i=0; i<deal; i++) track_wave[i] = 0;
	se_deal = 10;
	se_track = new char*[se_deal];
	for (i=0; i<se_deal; i++) se_track[i] = 0;
}

TrackName::~TrackName() {
	int i; for (i=0; i<deal; i++) {
		if (track[i] != 0) delete[] track[i];
		if (track_wave[i] != 0) delete[] track_wave[i];
	}
	for (i=0; i<se_deal; i++) {
		if (se_track[i]) delete[] se_track[i];
	}
	delete[] track;
	delete[] track_wave;
	delete[] track_num;
	delete[] track_start;
	delete[] se_track;
}
void TrackName::Expand(void) {
	int new_deal = deal * 2;
	int* new_track_num = new int[new_deal];
	int* new_track_start = new int[new_deal];
	char** new_track = new char*[new_deal];
	char** new_track_wave = new char*[new_deal];
	int i; for (i=0; i<deal; i++) {
		new_track_num[i] = track_num[i];
		new_track_start[i] = track_start[i];
		new_track[i] = track[i];
		new_track_wave[i] = track_wave[i];
	}
	for (; i<new_deal; i++) { 
		new_track_num[i] = 0;
		new_track_start[i] = 0;
		new_track[i] = 0;
		new_track_wave[i] = 0;
	}
	deal = new_deal;
	delete[] track; track = new_track;
	delete[] track_num; track_num= new_track_num;
	delete[] track_start; track_start= new_track_start;
	delete[] track_wave; track_wave = new_track_wave;
}
void TrackName::ExpandSE(int n) {
	if (n < 0) return;
	n += 10;
	if (se_deal >= n) return;
	char** new_se = new char*[n];
	int i; for (i=0; i<se_deal; i++) new_se[i] = se_track[i];
	for (; i<n; i++) new_se[i] = 0;
	delete[] se_track;
	se_deal = n; se_track = new_se;
}
void TrackName::AddCDROM(char* name, int tk) {
	if (CDTrack(name) != -1) return;
	int i; for (i=0; i<deal; i++) {
		if (track[i] == 0) break;
	}
	int num = i;
	if (i == deal) Expand();
	track[num] = new char[strlen(name)+1];
	for (i=0; name[i] != 0; i++) track[num][i] = tolower(name[i]);
	track[num][i] = 0;
	track_num[num] = tk;
}
void TrackName::AddWave(char* name, char* file, int pt) {
	if (CDTrack(name) != -1) return;
	int i; for (i=0; i<deal; i++) {
		if (track[i] == 0) break;
	}
	int num = i;
	if (i == deal) Expand();
	track_num[num] = 0;
	track_start[num] = pt;
	track[num] = new char[strlen(name)+1];
	for (i=0; name[i] != 0; i++) track[num][i] = tolower(name[i]);
	track[num][i] = 0;
	track_wave[num] = new char[strlen(file)+1]; strcpy(track_wave[num], file);
}
int TrackName::CDTrack(char* name) {
	char buf[1024];
	int i;
	for (i=0; name[i]!=0; i++) buf[i]=tolower(name[i]);
	buf[i]=0;
	for (i=0; i<deal; i++) {
		if (track[i] == 0) return -1;
		if (strcmp(track[i],  buf) == 0) {
			return track_num[i];
		}
	}
	return -1;
}
int TrackName::TrackStart(char* name) {
	char buf[1024];
	int i;
	for (i=0; name[i]!=0; i++) buf[i]=tolower(name[i]);
	buf[i]=0;
	for (i=0; i<deal; i++) {
		if (track[i] == 0) return -1;
		if (strcmp(track[i],  buf) == 0) {
			return track_start[i];
		}
	}
	return 0;
}
const char* TrackName::WaveTrack(char* name) {
	char buf[1024];
	int i;
	for (i=0; name[i]!=0; i++) buf[i]=tolower(name[i]);
	buf[i]=0;
	for (i=0; i<deal; i++) {
		if (track[i] == 0) return 0;
		if (strcmp(track[i],  buf) == 0) {
			return track_wave[i];
		}
	}
	return 0;
}
const char* TrackName::SETrack(int n) {
	if (n < 0 || n >= se_deal) return 0;
	return se_track[n];
}
void TrackName::AddSE(int n, char* file) {
	if (se_deal <= n) ExpandSE(n);
	if (se_track[n]) delete[] se_track[n];
	se_track[n] = new char[strlen(file)+1];
	strcpy(se_track[n], file);
}

