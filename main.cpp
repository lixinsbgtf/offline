#include <stdlib.h>
#include <iostream>
#include "Offline.h"

#define VERBOSITY 0
#define THRESHOLD 2
#define MAX_NGRAM 6
#define GAPLIMIT 0
//#define CONFIDENTCE 0.1
using namespace std;

int main(int argc, char * argv[]) {

  //if( argc != 7 ) 	{
  if( argc != 5 ) 	{
     //cerr << "Usage: " << argv[0] << " [option1] [option2] <documents.txt> <SynMap.txt> <uni_thresh> <mwt_thresh>" << endl;
     cerr << "Usage: " << argv[0] << " [option1] [option2] <documents.txt> <SynMap.txt>" << endl;
     cerr << "Option1:\n" ;
     cerr << "  1 			include unigram and multi word phrases\n";
     cerr << "  0			include only unigram\n"; 
     cerr << "Option2:\n" ;
     cerr << "  1 			with synonym group \n";
     cerr << "  0			without synonym \n";
     cerr << "For example: " << argv[0] << " 1 1 original_documents.txt SynMap.txt" << endl;
     return -1;		}

  char * pEnd;
  int phrase_in;
  phrase_in = strtol( argv[1], &pEnd, 10);
  if ( phrase_in > 1 || phrase_in < 0 ) {
    cerr << "Invalid option in argument 2: " << argv[1] << endl;
    exit(-1);
  }

  int root_in;
  root_in = strtol( argv[2], &pEnd, 10);
  if ( root_in > 1 || root_in < 0 ){
    cerr << "invalid option in argument 3: " << argv[2] << endl;
    exit(-1);
  }  

  ifstream fin( argv[3] );
  if ( !fin ){
    cerr << "no documents input." << endl;
    exit(-1);
  }

  ifstream fin2( argv[4] );
  if ( !fin2 ){
    cerr << "no synonym group input." << endl;
    exit(-1);
  }

  //int uni_thresh = atoi( argv[5] ); 
  //int mwt_thresh = atoi( argv[6] ); 

  Offline aOffline( VERBOSITY, MAX_NGRAM, THRESHOLD, phrase_in, root_in );
  //Offline aOffline( VERBOSITY, MAX_NGRAM, uni_thresh, mwt_thresh, phrase_in, root_in );
  aOffline.loadDict( fin2 );
  aOffline.readingDoc( fin );
  aOffline.output();
  return 0;
}
