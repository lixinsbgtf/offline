
#ifndef _DEFS_H_
#define _DEFS_H_

#include <string>

using namespace std;

#include <string>
#include <vector>

struct sequ { 
  string seq;
  int size;
};

struct docEntry {
  string docid;
  //long offset;
  //double magnitude;
  long size;
  //docEntry( const string& t_doc, const long& t_offset )
  docEntry( const string& t_doc, const long& t_size )
	:docid( t_doc ),
	 size( t_size ){};
};

struct posting { 
  long did;
  long tf;
  posting( const long& t_did, const long& t_tf)
	:did(t_did),
	 tf(t_tf){};
};

/*
struct posting_root {
  long root;
  vector<posting> post;
  posting_root( const long & t_root, const vector<posting> & t_post)
	:root(t_root),
	 post(t_post){};

};*/

struct wordRecord {
  string ngrm;
  long offset;
};

struct phrase_root {
  string phrase;
  long root;
  phrase_root( const string& t_ph, const long& t_root)
	:phrase( t_ph ),
	 root( t_root ){};
};

#endif
