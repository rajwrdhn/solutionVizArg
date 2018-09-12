/** read probo input, convert it to ASP input -- Norbert Manthey, 2015
*/

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool convertFile( char* filename )
{
  fstream infile ( filename );
  if( !infile ) return false;
  
  string line, argument1, argument2;
  bool foundAllArguments = false;
  int linecount = 0, arguments = 0, attacks = 0;
  while( getline( infile, line  ) ) // parse file line by line
  {
    linecount ++;
    if( line == "#" ) {
    	if( foundAllArguments ) {
    		cerr << "c WARNING: found '#' multiple times (this time on line " << linecount << ")" << endl;
    	}
	    foundAllArguments = true;
	    continue;
    }
    
    if( !foundAllArguments ) { // parse only one string from the line
    	stringstream tmp( line );
    	tmp >> argument1;
    	if( argument1 != "" ) {
		  	cout << "arg( " << argument1 << ")." << endl;
		  	arguments ++;
    	}
    } else {
    	stringstream tmp( line );
    	tmp >> argument1 >> argument2;
    	if( argument2 == "" && argument1 != "") {
    		cerr << "c WARNING: found attack with one empty argument in line " << linecount << endl;
    	} else if ( argument1 != "" ) {
    		cout << "att( " << argument1 << " , " << argument2 << ")." << endl;
    		attacks ++;
    	}
    }
    
  }
  cerr << "c converted " << arguments << " arguments and " << attacks << " attacks" << endl;
  return true;
}


/** print usage
*/

void usage( const char* binaryName ) {
	cerr << "c " << binaryName << " proboInputFile" << endl;
	cerr << "c" << endl
			 << "c will print ASP input to stdout" << endl;
}


/**
 *  main method
 */
int main(int argc, char *argv[])
{  

	std::cerr << "c ProboToASP" << endl;
  if( argc != 2 ) { usage(argv[0]); return 1; }

	std::cerr << "c use file " << argv[ argc-1] << endl;

  if( !convertFile( argv[ argc-1] )) return 1;
  else return 0;
}
