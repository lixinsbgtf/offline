#ifndef _OFFLINE_H_
#define _OFFLINE_H_

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "defs.h"
#include "util.h"
using namespace std;

class Offline {

  public:
	//Offline( int verbosity, int maxlength, int uni_thresh, int mwt_thresh, int phrase_in, int root_in );
	Offline( int verbosity, int maxlength, int thresh, int phrase_in, int root_in );

	void readingDoc( ifstream & fin );

	void mappingDoc( long t_docName, vector< vector<string> > & t_doc );

	void loadDict( ifstream & fin );
	 
	void output();

	~Offline();
  private:
  // function members
	long binary_search( vector<phrase_root> & A, string & key, long imin, long imax);

	long findRoot( long position );

  	void showDict();

  // container members
	//map<string, vector<posting> > mDict;
	map< string, vector<posting> > mDict;
	vector< phrase_root > mSynMap;

	vector<docEntry> mDocOffset;  
     
	//vector<sequ> mSequences;
	//vector<posting> mPosting;
	int mIncludePhrase;
    	int mIncludeSyn;
	int mMaxLength;
	//int mUniThreshold;
	//int mMwtThreshold;
	int mThreshold;
	int mVerbosity;
};

#endif
