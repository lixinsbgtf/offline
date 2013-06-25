#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "Offline.h"

//Offline::Offline( int verbosity, int maxlength, int uni_thresh, int mwt_thresh, int phrase_in, int root_in ){
Offline::Offline( int verbosity, int maxlength, int thresh, int phrase_in, int root_in ){
  mVerbosity = verbosity;
  mMaxLength = maxlength;
//  mUniThreshold = uni_thresh;
//  mMwtThreshold = mwt_thresh;
  mThreshold = thresh;
  mIncludePhrase = phrase_in;
  mIncludeSyn = root_in;
}	
Offline::~Offline(){
}

void
Offline::loadDict( ifstream & fin ) 
{
  // load dictionary

  string t_line;
  long t_root = 0, counter = 0;
  vector<posting> empty_posting; 

  while( getline(fin, t_line) ){
    vector<string> tokens = tokenize( t_line, " \t\n");

    if( tokens.size() > 2 ) {
      /* long fre = atol( tokens[tokens.size()-2].c_str() );
       * if( tokens.size() == 3 )     { if( fre < mUniThreshold ) continue; }
       * else if( tokens.size() > 3 ) { if( fre < mMwtThreshold ) continue;  }*/

      // reform the n-gram phrase after tokenization 
      string t_str = tokens[0] ;
      for( unsigned int i = 1; i < tokens.size()-2; i++ )
        t_str += " " + tokens[i] ; 
     
      if ( mIncludeSyn == 0 ) {
        // if mIncludeSyn is not evoked, all of the phrases will have to be stored in 
	// 	mDict and make posting for each of them.
	mDict.insert( pair<string, vector<posting> >( t_str, empty_posting ) );
      } 
      else if ( mIncludeSyn == 1 ) {
	// if mIncludeSyn is evoked, all of phrases that is their own root will be stored
	//	in mDict. All of the phrases will be stored in mSynMap.

        t_root = atol ( tokens.back().c_str() ) ;
	mSynMap.push_back( phrase_root( t_str, t_root ) );

	if( counter == t_root )
	  mDict.insert( pair<string, vector<posting> >( t_str, empty_posting ) );
	
	counter ++;
      }
    }
  }
  //showDict();
}

void
Offline::showDict()
{
  cout << "mDict size: " << mDict.size() << endl;
  map<string, vector<posting> >::iterator it = mDict.begin();
  for( ; it != mDict.end(); it ++ )
    cout << it->first << endl;

  cout << "\nmSynMap size: "  << mSynMap.size() << endl;
  for( long i = 0 ; i < mSynMap.size() ; i ++ )
    cout << mSynMap[i].phrase << "\t" << mSynMap[i].root << endl;
}
	
void
Offline::readingDoc( ifstream & fin )
{
  // load document

  vector< vector<string> > t_docu;
  long doc_size = 0, did = -1;
  string t_docid;

  string t_line;
  while( getline(fin, t_line) ){
    vector<string> tokens = tokenize( t_line, " \n\t");
    if( tokens.size() == 0 ) 
      // empty line
      continue;
    else if( tokens[0] == "$DOC" || tokens[0] == "$doc" ){  
       /* /FOR TEST/	
	* if ( did % 100 == 1 )	// for log to check what's going on during process
      	  cout << "Processing: " << did << " / 131896 " << endl;  */

      // sign of new doc
  
      if ( did != -1 ) {
	// indexing present document
        mappingDoc( did , t_docu );
	// record a Document's name and its size in text collection
        docEntry aEntry( t_docid, doc_size ); 
        mDocOffset.push_back( aEntry );
	//init of doc_size and t_docu
	doc_size = 0;
        t_docu.clear();
      }
      t_docid = tokens[1];
      did++;
    } else {						   
      // new non-document-header line
      t_docu.push_back( tokens );
      doc_size += tokens.size(); 
    }
  }

  // Last document handling
  if( did != -1 ) {
    mappingDoc( did, t_docu );
    docEntry aEntry( t_docid, doc_size ); 
    mDocOffset.push_back( aEntry );
    doc_size = 0;
    t_docu.clear();
  }
}

void Offline::mappingDoc( long t_docId, vector< vector<string> > & t_doc )
{
  map<string, int> t_dict;  // to store present document's ngram information (ngram, freq)
  map<string, int>::iterator it_dict;
  map<string, int> t_pos;
  map<string, int>::iterator it_pos;
  map<long, int> syn;
  map<long, int>::iterator it_syn;

  map<string, vector<posting> >::iterator it;
  int phrase_restart_point = 0;

  unsigned int i;
  for( i = 0; i < t_doc.size(); i++) {
    vector<string> * tokens = & t_doc[i];

    for( unsigned int j = 0; j < tokens->size() ; j++ ) {

      // Phrase only 
      if ( mIncludePhrase == 1 && j >= phrase_restart_point ) {
      //if ( mIncludePhrase == 1 ) {
	// find longest n-gram
        string t_str = (*tokens)[j] ;
        for ( int n = 1 ; n < mMaxLength && j+n < tokens->size() ; n++ )
          t_str += " " + (*tokens)[j+n];
	vector<string> t_tokens = tokenize( t_str, " ");

        bool first = true;
        while ( t_tokens.size() > 1 ) {
	  // check from the temp longest n-gram to the shortest n-gram
	  string t_stri = t_tokens[0] ;
          if( first ){
	    first = false;
	    t_stri = t_str;
	  }else
	    for ( unsigned int m = 1; m < t_tokens.size(); m++ )
	      t_stri += " " + t_tokens[m];
	  
	  if ( mIncludeSyn == 0 ){
	    // all n-gram's information is stored in mDict
  	    it = mDict.find( t_stri );
            if ( it != mDict.end() ) {
	      it_dict = t_dict.find( t_stri );
	      if ( it_dict != t_dict.end() )
	        it_dict->second ++;
	      else
	        t_dict[ t_stri ] = 1;

	      phrase_restart_point = j + t_tokens.size();
	      break;
	    }
	  }
	  else if ( mIncludeSyn == 1 ) {
	    // all n-gram's information is stored in mSynMap
	    long position = binary_search( mSynMap, t_stri, 0, mSynMap.size()-1 );
	    if ( position > -1 ) {
	      it_dict = t_dict.find( t_stri );
	      if ( it_dict != t_dict.end() )
	        it_dict->second ++;
	      else
	        t_dict[ t_stri ] = 1;

	      t_pos[ t_stri ] = position;

	      phrase_restart_point = j + t_tokens.size();
	      break;
	    }
	  }
	  //t_tokens.erase( t_tokens.begin() + t_tokens.size() - 1 );
	  t_tokens.pop_back();
        }
      }

      if( i >= phrase_restart_point ){
        // Word only
        if ( mIncludeSyn == 0 ) {
          it_dict = t_dict.find( (*tokens)[j] );
          if( it_dict != t_dict.end() )
            it_dict->second += 1;
          else
	    t_dict[ (*tokens)[j] ] = 1;
        }
        else if ( mIncludeSyn == 1 ){
          long position = binary_search( mSynMap, (*tokens)[j], 0, mSynMap.size()-1 );
	  if ( position > -1 ) {
	    it_dict = t_dict.find( (*tokens)[j] );
	    if ( it_dict != t_dict.end() )
	      it_dict->second ++;
	    else
	      t_dict[ (*tokens)[j] ] = 1;
	    t_pos[ (*tokens)[j] ] = position;  
	  }
        }
      }
    }
  }

  // insert the word and phrases found in present document to mDict.
  if ( mIncludeSyn == 0 ) {
    it_dict = t_dict.begin();
    for ( ; it_dict != t_dict.end(); it_dict ++ ) {
      it = mDict.find( it_dict->first );
      if ( it != mDict.end() ) {
        posting tPosting( t_docId, it_dict->second );
        it->second.push_back( tPosting );
      } //else { cout << "ERROR with mDict ... " << endl; exit(1); }
    }
  } else if ( mIncludeSyn == 1 ) {
    it_pos = t_pos.begin();
    for( ; it_pos != t_pos.end(); it_pos ++ ) {  
      it_dict = t_dict.find( it_pos->first );
      if( it_dict == t_dict.end() ) { cout << "1Error with " << it_pos->first << endl; exit(1); }

      long t_root = findRoot( it_pos->second );
      //if ( t_root == -1 ) { cout << "2Error with " << it_pos->first << endl; exit(1); }

      it_syn = syn.find( t_root );
      if ( it_syn != syn.end() )
	it_syn->second = max( it_syn->second, it_dict->second );
      else
	syn[ t_root ] = it_dict->second;
    }

    it_syn = syn.begin();
    for ( ; it_syn != syn.end(); it_syn++ ) {  
      it = mDict.find( mSynMap[it_syn->first].phrase );
      if( it != mDict.end() )
	it->second.push_back( posting( t_docId, it_syn->second ) );
      else {
	cout << "There is sth wrong with : " << mSynMap[it_syn->first].phrase << endl;
	exit(1);
      }
    }
  }
}
  
/*		
   c. Create dictionary.txt, posting.txt and docOffset.txt
*/
  
 
void
Offline::output()
{
  //cout << "mDict size: " << mDict.size() << endl; 
  ofstream ofsDict("dictionary.txt");
  ofstream ofsPos("posting.txt");
  ofstream ofsDocOff("docOffset.txt"); // actually, it is doc size.
  
  // dictionary.txt and posting.txt
  unsigned long pos = 0;
  map<string, vector<posting> >::iterator it = mDict.begin();
  for ( ; it != mDict.end(); it++ ) {
    //if( it->second.size() >= mThreshold ) {

      ofsDict << it->first << "\t" << pos << endl;
      pos += it->second.size();
      //if( it->second.size() <= 0 ){cout << "ERROR WITH ZERO POSTING " << it->first << endl;
	//exit(1);}

      for( unsigned int i = 0; i < it->second.size() ; i++)
        ofsPos << it->second[i].did << "\t" << it->second[i].tf << endl;

    //}else {
      //if ( mVerbosity > 1 ) {
        //cout << "ERROR with: | " << it->first << " | with size " << it->second.post.size() << endl;
        //uselessPhrase ++;
      //}
      //exit(-1);
    //}
  }

  //if ( mVerbosity > 1 )  cout << "The useless phrases's number is: "  << uselessPhrase  << endl;

  // docOffset.txt
  for( unsigned long i = 0 ; i < mDocOffset.size() ; i++ )
    ofsDocOff << mDocOffset[i].docid << " " << mDocOffset[i].size << endl;
} 

long Offline::binary_search( vector<phrase_root> & A, string & key, long imin, long imax)
{
  // continue searching while [imin,imax] is not empty
  while (imax >= imin) {
    // calculate the midpoint for roughly equal partition 
    //int imid = midpoint(imin, imax);
    long imid = (imin + imax)/2 ;

    // determine which subarray to search
    if ( A[imid].phrase < key )
      // change min index to search upper subarray
      imin = imid + 1;
    else if ( A[imid].phrase > key )
      // change max index to search lower subarray
      imax = imid - 1;
    else
      // key found at index imid
      return imid;
  }
  // key not found
  return -1;
}

long Offline::findRoot( long position )
{
  if( mSynMap[position].root == position )
    return position;
  else {
    long t_root = mSynMap[position].root;
    while( mSynMap[t_root].root != t_root ) {
      t_root = mSynMap[t_root].root;
    }
    return t_root;
  }
}
