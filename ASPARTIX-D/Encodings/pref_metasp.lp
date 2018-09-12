%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encoding for preferred extensions
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Guess a set S \subseteq A
%{in(X):arg(X)}.

in(X) :- not out(X), arg(X).
out(X) :- not in(X), arg(X).

%% S has to be conflict-free
:- in(X), in(Y), att(X,Y).

%% All arguments x \in S need to be defended by S (admissibility)
defeated(X) :- in(Y), att(Y,X).
:- in(X), att(Y,X), not defeated(Y).

%% Minimize out to get the subset-maximal admissible sets (needs metasp)
#minimize [ out(X) ].