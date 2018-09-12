/** read ASP output, convert it to proboanswer -- Norbert Manthey, 2015
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

/**
 *  main method
 */
int main(int argc, char *argv[])
{  
	//std::cerr << "c parse ASP output from stdin and convert it into normal form output on stdout ... " << endl;
  //std::cerr << "c parameters: -yesnofor ATOM, -single, -oneline/-no-oneline, -space/-no-space" << endl;
  string toBeFound = "";
  bool singleSolution = false;
  bool oneline = true;
  string usespace = "";
  
  /*
   *  check whether there is a parameter for a string to be found in the answer to have YES/NO
   */
  for (int i = 1; i < argc; ++i){
 		if( string(argv[i]) == "-yesnofor" ) {
 			++i;
 			if( i < argc ) toBeFound = string(argv[i]);
 		} else if( string(argv[i]) == string("-single")  ) {
 		  singleSolution = true;
 		} else if( string(argv[i]) == string("-oneline") ) {
 		  oneline = true;
 		} else if( string(argv[i]) == string("-no-oneline") ) {
 		  oneline = false;
 		} else if( string(argv[i]) == string("-space") ) {
 		  usespace = " ";
 		} else if( string(argv[i]) == string("-no-space") ) {
 		  usespace = "";
 		}
  }
  
  bool isSatisfiable = false, wasInterrupted = false;
  int answers = 0;
	int linecount = 0;
  string argument1, line;
  stringstream normalFormAnswer;
  
  if( ! singleSolution )     // if there might be multiple solutions
	  normalFormAnswer << "["; //    print opening bracket for set of answers
  
  bool isFirstAnswer = true;
  while( getline( cin, line  ) ) // parse file line by line
  {
    linecount ++;
   	stringstream tmp( line );
   	tmp >> argument1;

		if( argument1 == "SATISFIABLE" ) { 
			isSatisfiable = true;
			continue;
		} else if ( argument1 == "INTERRUPTED" ) {
			wasInterrupted = true;
			break;
		}
		
		if( argument1 == "Answer:" ) { // on the next line there is another answer
			linecount ++;
			if( !getline( cin, line  ) ) {
				cerr << "c not able to parse the actual answer on solver output line " << linecount << " - ABORT" << endl;
				return 1; // no usual exit
			}
			answers ++;
			if( answers > 1 ) normalFormAnswer << ",";
			normalFormAnswer << usespace << "["; // opening bracket for current answer
			size_t position = line.find( "in(" );
			bool isFirstAtom = true;
			while( position != string::npos ) {
				line = line.substr( position ); // get next line
				
				// read name of argument within "in"
				const int end =  line.find( ")", 4 );
				if( !isFirstAtom ) normalFormAnswer << ",";                // add comma, if there is is another atom
				normalFormAnswer << usespace << line.substr(3, end - 3 );  // print atom with space
				
				isFirstAtom = false;
				
				// check whether we found the element in the answer -- if yes, abort immediately!
				if( ! toBeFound.empty() && toBeFound == line.substr(3, end - 3 ) ) {
					cout << "YES " << endl;
					return 10;
				}				
				
				// continue after current pattern
				position = line.find( "in(", end ); 
			}
			normalFormAnswer << usespace << "]"; // closing bracket for current answer
			if(!oneline) normalFormAnswer << endl;
			isFirstAnswer = false;
		}

  }
  if( ! singleSolution )      // if there might be multiple solutions
  	normalFormAnswer << "]";  // print closing bracket for set of answers
  normalFormAnswer << endl;
  
 // cerr << "found answers: " << answers << " found satisfiable: " << isSatisfiable << endl;
  
	// check whether we found the element in the answer -- if not, produce result and exit!
	if( ! toBeFound.empty() ) {
		cout << "NO " << endl;
		return 20;
	}
  
  if( answers > 0 && !isSatisfiable || wasInterrupted ) {
	  cerr << "found multiple answers but not all -- search incomplete" << endl;
	  return 0;
  }
  
  // print result and return with exit code
  cout << normalFormAnswer.str();
  return isSatisfiable ? (answers > 1 ? 30 : 10) : 20; // exit code based on exit codes of SAT competition
}
