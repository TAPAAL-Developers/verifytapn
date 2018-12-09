The following provides a brief introduction to the DistanceMeasure implementation in Verifydtapn. For more information see [1] and [2].

Program arguments example: -k 4 src/DistanceMeasure/Data/tawfn_export.xml src/DistanceMeasure/Data/universal.q
Home directory: /home/gblegmand/git/sw9_kode

Current naming convention for transitions and places (Remember to give unique identifiers always i.e. increment identifiers):
Places: P0, P23 etc. This follows the same ordinary naming convention
Transitions: Tau transitions are named as "T40_Tau_" or "T23_Tau_", and "T2_RoL_" or "T3_RoH_"


Suggestions for future code improvements:
1) Decide on a naming scheme for transitions that adheres to TAPAAL naming possibilities and enables simple and robust string comparisons in the code.
2) Introduce new distance such as penalising for "inproper completion" i.e. tokens remain in other places than out when trace has been replayed.
3) Proper integration of the distance feature into the existing Verifydtapn project
  -This includes properly configuring the project with CMake as well as creating a new flag to run the distance measure on some given input

Documentation is found in the following papers:
[1]"Adaptive Storage Rearrangement and Process Discovery in a
Warehouse", June 2018, C. Stephansen and G. Blegmand, https://projekter.aau.dk/projekter/files/281080339/deis1011_thesis.pdf
[2]"Conformance Checking Extended Timed-Arc Workflow Nets", January 2018, C. Stephansen and G. Blegmand
