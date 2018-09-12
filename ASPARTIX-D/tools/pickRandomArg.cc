/** read ASP output, return a randomly selected argument on stdout -- Norbert Manthey, 2015
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

bool isSpace( const char& c ) {
	return c == '\n' || c == ' ' || c == '\r' || c == '\t';
}

/**
 *  main method
 */
int main(int argc, char *argv[])
{  
	std::cerr << "c parse ASP input in apx format, return a randomly selected argument on stdout" << endl;
  std::cerr << "c parameters: -seed RANDOMSEED" << endl;
  
  int seed = -1;
  
  /*
   *  check whether there is a parameter
   */
  for (int i = 1; i < argc; ++i){
 		if( string(argv[i]) == "-seed" ) {
 			++i;
 			if( i < argc ) {
 				stringstream tmp (string(argv[i])); // parse seed number
 				tmp >> seed;
 			}
 		}
 	} 
 	if( seed == -1 ) seed = time(NULL); // initialize based on time, if not specified differently
 	srandom( seed );
  
	int linecount = 0;
  string line;
  vector<string> arguments;
  while( getline( cin, line, '.'  ) ) // parse element by element. ASP delimiter is "."
  {

		line.erase( std::remove_if( line.begin(), line.end(), isSpace ), line.end() );
		size_t position = line.find( "arg(" );
		if( position != string::npos ) {
			size_t end      = line.find( ")", position + 5 );
			string element = line.substr(position + 4, end - position - 4);
			if( line == "arg(" + element + ")" ) { // use element only, if there are no more other symbols in the current token
				arguments.push_back( element );
			}
		}
  }
  cerr << "c found " << arguments.size() << " arguments" << endl;

	// if there have been arguments, pick one, and print it
	if( arguments.size() > 0 ) {
		int pick = random() % arguments.size();
		cout << arguments[pick] << endl;
		return 0;
	} else
		return 1;
}
