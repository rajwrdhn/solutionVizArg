#!/bin/bash
# bash script to run 'aspatrix' asgumentation solver 
# Sarah Gaggl, Norbert Manthey, 2015
#
# usage:
# ./aspatrix.sh ... show solver information
# ./aspatrix.sh --formats  ... show available framework formats
# ./aspatrix.sh --problems ... show available problem types (task and semantics)
# ./aspatrix.sh -f <filename> -fo <format> -p problemType -a additionalArgument [options]
#      options will be passed to the solver that is called to solve the actual problem
#
#

#
# example calls:
#
# ./aspatrix.sh -f examples/116134__200__18_19_87__57.apx -fo apx -p EE-PR
# ./aspatrix.sh -f examples/116134__200__18_19_87__57.apx -fo apx -p SE-ST
# ./aspatrix.sh -f examples/116134__200__18_19_87__57.apx -fo apx -p DC-PR -a a120
# ./aspatrix.sh -f examples/116134__200__18_19_87__57.apx -fo apx -p DS-ST -a a120

#
# specific constants, and temporary files
#
supportedProblems=( DC-CO DC-GR DC-PR DC-ST DS-CO DS-GR DS-PR DS-ST EE-CO EE-GR EE-PR EE-ST SE-CO SE-GR SE-PR SE-ST )
supportedProblemsString="[ DC-CO, DC-GR, DC-PR, DC-ST, DS-CO, DS-GR, DS-PR, DS-ST, EE-CO, EE-GR, EE-PR, EE-ST, SE-CO, SE-GR, SE-PR, SE-ST ]"
mkdir -p tmp                                                     # build a directory for temporary files
TMPFILE=tmp/additional-$$-`hostname`.pl                          # do not use mktemp, as the competition requires to write temporary files into the solver directory

# checkAdditional ( additionalArgument, problemType )
# check, whether an additional argument was specified
#
checkAdditional () {
	if [ $1 == "" ]
	then
	  echo "additional argument missing for problem format $2" 1>&2
	  exit 1
  fi
}

# containsElement ( stringToTest, arrayOf Elements )
# check whether the string is present in the array as element
# (pass array as ${array[@]}"
#
containsElement () {
  local e
  for e in "${@:2}"; 
  do 
  	[[ "$e" == "$1" ]] && return 0; 
  done
  return 1
}

# solve the current problem with the SAT solver
# all logic programs that should be passed to the conversion are given as arguments
# @exit exit code of the SAT solver
#
solveWithSat () {
	local d
  d=solvers/lp2sat/linux
  # convert all file from the given parameters, pipe them into translation, and run the SAT solver
#  echo "c use SAT solver: $solverCode" 1>&2
  solvers/clingo/linux/gringo_linux_64 $* | $d/lp2normal | $d/lp2atomic | $d/lp2sat | solvers/riss/linux/riss -config=Riss427 -quiet > /dev/null 2> /dev/null
  solverCode=$?
#  echo "c SAT solver exit code: $solverCode" 1>&2
  return $solverCode
}

#
# general script variables
#
solver="aspatrix"
fileinput=""
problem=""
format=""
additionalArgument=""
remainingParameters=""
exitCode=""
useSATsolver="0"
useMetasp="0"

# encodings to be used for model enumeration tasks (one or all)
stableEnumerationEncoding=Encodings/stable.lp
completeEnumerationEncoding=Encodings/comp_gringo.lp
preferredEnumerationEncoding=Encodings/prefex_gringo.lp

# use different default preferred encoding
preferredDefaultEncoding=Encodings/prefex_gringo.lp

#
# make sure that on exit the temporary file is removed (even if there was no file created)
#
trap 'rm -f "$TMPFILE"' EXIT

#
# print solver information
#
if [ "$#" = "0" ]
then
	echo " aspatrix "
	echo " Sarah Gaggl <sarah.gaggl@tu-dresden.de>"
	echo " Norbert Manthey "
	exit 0
fi

#
# parse command line
#
while [ "$1" != "" ]; do
	case $1 in
		# print supported formats, yet only apx
		"--formats" )
			echo "[ apx ]"
			exit 0
			;;
		# print supported problem types
		"--problems" )
			echo "$supportedProblemsString"
			exit 0
			;;
		# receive name of the problem file
		 "-p" )
			shift
			problem=$1
			;;
		# receive name of the input file
		"-f" )
			shift
			fileinput=$1
			;;
		# receive name of the input format
		"-fo" )
			shift
			format=$1
			;;
		"-a" )
			shift
			additionalArgument=$1
			;;
		# change enumeration encodings 
		"-EE" )
			stableEnumerationEncoding=Encodings/stable_EE.lp
			completeEnumerationEncoding=Encodings/comp_gringo_EE.lp
			preferredEnumerationEncoding=Encodings/prefex_gringo_EE.lp
			;;
		# use SAT approach instead of solving ASP with clasp directly (so far, works only DC-ST, DS-ST, DC-CO, DS-CO)
		"-SAT" )
			useSATsolver="1"
			;;
       # use Metasp for EE-PR, EE-GR, SE-PR, SE-GR
        "-metasp" )
        useMetasp="1"
        ;;
		# use encodings used in vienna variant of the tool, currently only for preferred semantics
		"-VIENNA" )
			preferredDefaultEncoding=Encodings/prefex_vienna.lp
			preferredEnumerationEncoding=Encodings/prefex_vienna.lp
			;;
		# handle remaining parameter
		*)
			remainingParameters="$remainingParameters $1"
			;; 
	esac
	shift
done

#
# handle case where no data is given
#
if [ "$format" = "" -o "$problem" = "" -o "$fileinput" = "" ]
then
	echo "No Input, Format or Problem given, exit" 1>&2
	exit 0
fi

if [ "$remainingParameters" != "" ]
then
	echo "use additional parameter: $remainingParameters"  1>&2
fi

# set path to clingo binary correctly
clingo=solvers/clingo/linux/clingo_linux_64
gringo305=solvers/clingo/linux/gringo305_linux
clasp=solvers/clingo/linux/clasp_linux_64
if [[ $OSTYPE == darwin* ]];
then
	clingo=solvers/clingo/mac/clingo_macos_64
    gringo305=solvers/clingo/mac/gringo305
    clasp=clasp
fi

#
# handle wrong format and wrong problem type
#
if [ "$format" != "apx" ]
then
	echo "wrong format given: $format - ABORT"  1>&2
	exit 1
fi
containsElement $problem ${supportedProblems[@]}
containCode=$?
if [ "$containCode" == "1" ]
then
	echo "unsupported problem type given: $problem - ABORT" 1>&2
	exit 1
fi
# check if input file exists
if [ ! -f $fileinput ]
then
	echo "input file does not exist: $fileInput - ABORT" 1>&2
	exit 1
fi

#
# execute solver according to problem type and format
#
# ground
#
if [ "$problem" = "EE-GR" -o "$problem" = "SE-GR" ]; # ground, enumeration, there exists only one extension, but not additional information, just run
then
#   if [ "$useMetasp" -eq "1" ]; then # we should use Metasp
    output=$( $gringo305 --reify Encodings/ground_metasp.lp $fileinput | $gringo305 - {Encodings/meta.lp,Encodings/metaO.lp,Encodings/metaD.lp} <(echo "optimize(1,1,incl).") | $clasp 1 $remainingParameters )
    exitCode=$?
#   else
#	output=$( $clingo Encodings/ground.lp $fileinput 1 $remainingParameters )             # search for all models
#	exitCode=$?
#   fi
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then     # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		if [ "$problem" = "EE-GR" ]; then 
			echo "$output" | tools/parseOutput          # convert output to actual competition output for multiple answers
		else	
			echo "$output" | tools/parseOutput -single  # convert output to actual competition output for single answers
		fi		
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi

elif [ "$problem" = "DC-GR" -o "$problem" = "DS-GR" ]; 
then
	# add the additioal element to the graph, does there exist one solutions -> answer is yes, otherwise no
	checkAdditional  $additionalArgument  $problem ; # check whether there has been an additional argument
#   if [ "$useMetasp" -eq "1" ]; then # we should use Metasp
    output=$( $gringo305 --reify Encodings/ground_metasp.lp $fileinput | $gringo305 - {Encodings/meta.lp,Encodings/metaO.lp,Encodings/metaD.lp} <(echo "optimize(1,1,incl).") | $clasp 1 $remainingParameters )
    exitCode=$?
#   else
#   output=$( $clingo Encodings/ground.lp $fileinput 1 $remainingParameters )             # search for one model
#	exitCode=$?
#   fi
    if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then     # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput -yesnofor $additionalArgument                    # convert output to actual competition output YES/NO
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi
	
#
# FIXed: is this case necessary, or can it be treated as the above case?
#	NO, we can treat it as above.
#elif [ "$problem" = "DS-GR" ]; 
#then
#	# add negated additinonal element to the graph. does there exist a solution -> answer is no
#	# otherwise, add the positive element to the graph. does there exist a solution -> answer is yes, otherwise no
#	checkAdditional  $additionalArgument  $problem ; # check whether there has been an additional argument
#	echo "not yet implemented - ABORT" 1>&2 ; exit 1
#    $clingo Encodings/ground.lp $fileinput 1
##todo: check if additional element -a a1 "in(a1)" contained in answer set (check exit code), return YES or NO
#	exitCode=$?
#

#
# stable
#
elif [ "$problem" = "DC-ST" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
	echo "in($additionalArgument)." > $TMPFILE
	
	if [ "$useSATsolver" -eq "1" ]; then # we should use a SAT solver
		solveWithSat Encodings/stable.lp $fileinput $TMPFILE
		exitCode=$?
	else
    output=$( $clingo Encodings/stable.lp $fileinput $TMPFILE 1 $remainingParameters)
#$clingo Encodings/stable.lp $fileinput $TMPFILE 1 $remainingParameters 1>&2           # pipe stdout of clingo to stderr, so that its not in the actual output
  	exitCode=$?
  fi
  if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then 
  	echo "YES"
  elif [ "$exitCode" -eq "20" ]; then 
  	echo "NO"
  else
  	echo "UNKNOWN"
  fi

elif [ "$problem" = "DS-ST" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
  echo "out($additionalArgument)." > $TMPFILE
	if [ "$useSATsolver" -eq "1" ]; then # we should use a SAT solver
		solveWithSat Encodings/stable.lp $fileinput $TMPFILE
		exitCode=$?
	else
    output=$( $clingo Encodings/stable.lp $fileinput $TMPFILE 1 $remainingParameters)
#  $clingo Encodings/stable.lp $fileinput $TMPFILE 1 $remainingParameters 1>&2           # FIXME: check final decision of ICCMA commitee (this is the correct handling), pipe stdout of clingo to stderr, so that its not in the actual output
		exitCode=$?
	fi
	
  if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then 
  	echo "NO"
  elif [ "$exitCode" -eq "20" ]; then 
  	echo "YES"
  else
  	echo "UNKNOWN"
  fi

elif [ "$problem" = "EE-ST" ];
then
	output=$( $clingo $stableEnumerationEncoding $fileinput 0 --project $remainingParameters)        # search for all models
	exitCode=$?
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then                               # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput  # convert output to actual competition output
		parseExitCode=$?
	elif [ "$exitCode" -eq "20" ]; then   # handle unsatisfiable problems
		echo "[]"                           # FIXed: should 'NO' be printed? check final decision of ICCMA commitee -> no, [] is correct!
	else
		echo "UNKNOWN"
	fi

elif [ "$problem" = "SE-ST" ];
then
	output=$( $clingo $stableEnumerationEncoding $fileinput 1 --project $remainingParameters)         # search for one model
	exitCode=$?
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then                               # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput  -single # convert output to actual competition output
		parseExitCode=$?
	elif [ "$exitCode" -eq "20" ]; then   # handle unsatisfiable problems
		echo "NO"                           # FIXed: should '[]' be printed as well? check final decision of ICCMA commitee -> no, here NO is correct
	else
		echo "UNKNOWN"
	fi

	
#
# complete
#
elif [ "$problem" = "DC-CO" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
  echo "in($additionalArgument)." > $TMPFILE
#	if [ "$useSATsolver" -eq "1" ]; then # we should use a SAT solver
		solveWithSat Encodings/comp_gringo.lp $fileinput $TMPFILE
		exitCode=$?
#	else
 # 	$clingo Encodings/comp_gringo.lp $fileinput $TMPFILE 1 --configuration=frumpy $remainingParameters 1>&2      # pipe stdout of clingo to stderr, so that its not in the actual output
#	  exitCode=$?
#	fi
  if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then 
  	echo "YES"
  elif [ "$exitCode" -eq "20" ]; then 
   	echo "NO"
  else
	 	echo "UNKNOWN"
  fi

elif [ "$problem" = "DS-CO" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
  echo "out($additionalArgument)." > $TMPFILE
#	if [ "$useSATsolver" -eq "1" ]; then # we should use a SAT solver
		solveWithSat Encodings/comp_gringo.lp $fileinput $TMPFILE
		exitCode=$?
#	else
 # 	$clingo Encodings/comp_gringo.lp $fileinput $TMPFILE 1 $remainingParameters 1>&2      # pipe stdout of clingo to stderr, so that its not in the actual output
  #	exitCode=$?
 # fi
  if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then 
  	echo "NO"
  elif [ "$exitCode" -eq "20" ]; then 
  	echo "YES"
  else
  	echo "UNKNOWN"
  fi

elif [ "$problem" = "EE-CO" ];
then
	output=$( $clingo $completeEnumerationEncoding $fileinput 0 --project $remainingParameters )      # search for all models
	exitCode=$?
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then       # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput           # convert output to actual competition output
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi
	
elif [ "$problem" = "SE-CO" ];
then
	output=$( $clingo $completeEnumerationEncoding $fileinput 1 --project $remainingParameters )      # search for one model
	exitCode=$?
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then       # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput  -single  # convert output to actual competition output
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi


#
# preferred
#
elif [ "$problem" = "DC-PR" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
  echo "in($additionalArgument)." > $TMPFILE
#    $clingo Encodings/prefex_gringo.lp $fileinput $TMPFILE 1 $remainingParameters 1>&2      # pipe stdout of clingo to stderr, so that its not in the actual output
    output=$($clingo $preferredDefaultEncoding $fileinput $TMPFILE 1 --configuration=auto $remainingParameters)
 #$clingo $preferredDefaultEncoding $fileinput $TMPFILE 1 --configuration=auto $remainingParameters 1>&2      # pipe stdout of clingo to stderr, so that its not in the actual output
	exitCode=$?
    if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then
  	echo "YES"
  elif [ "$exitCode" -eq "20" ]; then 
  	echo "NO"
  else
  	echo "UNKNOWN"
  fi
	
elif [ "$problem" = "DS-PR" ];
then
	checkAdditional  $additionalArgument   $problem ;                                       # check whether there has been an additional argument
  echo "out($additionalArgument)." > $TMPFILE
# $clingo Encodings/prefex_gringo.lp $fileinput $TMPFILE 1 $remainingParameters 1>&2
#    exitCode=$?
#   if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then
    output=$($clingo $preferredDefaultEncoding $fileinput $TMPFILE 1 --configuration=auto $remainingParameters)
# $clingo $preferredDefaultEncoding $fileinput $TMPFILE 1 $remainingParameters 1>&2
  exitCode=$?
  if [ "$exitCode" -eq "10" -o "$exitCode" -eq "30" ]; then
  	echo "NO"
  elif [ "$exitCode" -eq "20" ]; then 
  	echo "YES"
  else
  	echo "UNKNOWN"
  fi
	
elif [ "$problem" = "EE-PR" ];
then
# if [ "$useMetasp" -eq "1" ]; then # we should use Metasp
    output=$($gringo305 --reify Encodings/pref_metasp.lp $fileinput | $gringo305 - {Encodings/meta.lp,Encodings/metaO.lp,Encodings/metaD.lp} <(echo "optimize(1,1,incl).") | $clasp 0 $remainingParameters)
    exitCode=$?
#   else
#	output=$( $clingo $preferredEnumerationEncoding $fileinput 0 $remainingParameters )        # search for all models
#	exitCode=$?
#   fi
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then       # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
		echo "$output" | tools/parseOutput           # convert output to actual competition output
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi
	
elif [ "$problem" = "SE-PR" ];
then
#  if [ "$useMetasp" -eq "1" ]; then # we should use Metasp
    output=$($gringo305 --reify Encodings/pref_metasp.lp $fileinput | $gringo305 - {Encodings/meta.lp,Encodings/metaO.lp,Encodings/metaD.lp} <(echo "optimize(1,1,incl).") | $clasp 1 $remainingParameters)
    exitCode=$?
#  else
#	output=$( $clingo $preferredEnumerationEncoding $fileinput 1 $remainingParameters )        # search for one model
#   exitCode=$?
#   fi
	if [ "$exitCode" -eq "10" -o "$exitCode" -eq "20" -o "$exitCode" -eq "30" ]; then       # handle output only if solver terminated
#	echo "analyze output $output" 1>&2
      echo "$output" | tools/parseOutput  -single   # convert output to actual competition output
		parseExitCode=$?
	else
		echo "UNKNOWN"
	fi
	
#
# fall back
#
else
	echo "unsupported problem type: $problem - ABORT" 1>&2
	exit 1
fi

#
# clean up
#
rm -f $TMPFILE

#
# everything went fine
#
#echo "solver exit code: $exitCode parseOutput exit code: $parseExitCode" 1>&2
exit 0

