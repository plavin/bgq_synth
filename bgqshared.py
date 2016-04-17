#!/usr/bin/env python
import sys
import ast
from collections import defaultdict

Dim = [4,4,4,4,2] # torus dimensions

# take a node, and return a five-tuple representing the coordinates
def convertNodeToCoords(nodeId):
  eVal = nodeId / 1024
  remainder = nodeId % 1024
  dVal = remainder / 64
  remainder = remainder % 64
  cVal = remainder / 16
  remainder = remainder % 16
  bVal = remainder / 4
  aVal =  remainder % 4
  return (aVal, bVal, cVal, dVal, eVal)

#reads nodes (as 5-tuples) from file passed in as command line arg                
def readNodeSet(file):
  list = []
  with open(file) as f:
    for line in f.readlines():
      list.append(ast.literal_eval(line))
  return list


def addLinksForward(source, index, numHops, linkSet):
  last = source
  for i in range(1, numHops+1):
    oneHopForward = source[0:index] + (source[index]+i,) + source[index+1:len(source)]
    print "routing from", last, "to", oneHopForward
    linkSet[(last,oneHopForward)] += 1
    last = oneHopForward


def addLinksBackward(source, index, numHops, linkSet):
  last = source
  for i in range(1, numHops+1):
    oneHopBackward = source[0:index] + (source[index]-i,) + source[index+1:len(source)]
    linkSet[(last,oneHopBackward)] += 1
    last = oneHopBackward
    
#index is which of the five dimensions (A, B, C, D, E) we are moving in
#source and dest are the source and dest nodes (a five-tuple; A, B, C, D, E)
#linkSet is a dictionary; we going to count all links encountered on this move
def moveDirection(index, source, dest, linkSet):
  global Dim

  sI = source[index]
  dI = dest[index]

  dist = abs(sI-dI)
  if dist <= Dim[index]/2 and sI <= dI:  # do not use wraparound; already going forward
    addLinksForward(source, index, dist, linkSet)
  elif dist <= Dim[index]/2 and dI < sI:  # do not use wraparound; must reverse direction
    addLinksBackward(source, index, dist, linkSet)
  else:   # need to use wraparound
    if sI < dI:  
      # two recursive calls to make two backward moves, then add wraparound link
      start = source
      end = source[0:index] + (dest[index],) + source[index+1:len(source)]
      smallEnd = source[0:index] + (0,) + source[index+1:len(source)]
      moveDirection(index, start, smallEnd, linkSet)
      largeEnd = source[0:index] + (Dim[index]-1,) + source[index+1:len(source)]
      moveDirection(index, largeEnd, end, linkSet)
      linkSet[(smallEnd, largeEnd)] += 1  # this adds the wraparound link

    else:
      # two recursive calls to make two forward moves, then add wraparound link
      start = source[0:index] + (dest[index],) + source[index+1:len(source)]
      end = source
      smallEnd = source[0:index] + (0,) + source[index+1:len(source)]
      moveDirection(index, smallEnd, start, linkSet)
      largeEnd = source[0:index] + (Dim[index]-1,) + source[index+1:len(source)]
      moveDirection(index, end, largeEnd, linkSet)
      linkSet[(largeEnd, smallEnd)] += 1  # this adds the wraparound link

  # return progress up to this point
  return source[0:index] + (dest[index],) + source[index+1:len(source)]

#route from all nodes in a node list to all other nodes in that node list
#assumes all-to-all communication exists
def doRouting(nodeList, linkSet):

  for source in nodeList:
    for dest in nodeList:
      if source == dest:
        continue
      #move in each of the five directions, one at a time.  After a move in dimension i, source matches dest in dimension i
      #order is sorted by decreasing dimension; ties broken lexicographically
      sortedDim = sorted(list(enumerate(Dim)),key=lambda x: x[1],reverse=True)

      for i in range(len(Dim)):
        newSource = moveDirection(sortedDim[i][0], source, dest, linkSet)
        

def doRoutingPatrick(source, dest, linkSet):
  #move in each of the five directions, one at a time.  After a move in dimension i, source matches dest in dimension i
  #order is sorted by decreasing dimension; ties broken lexicographically
  sortedDim = sorted(list(enumerate(Dim)),key=lambda x: x[1],reverse=True)
  
  for i in range(len(Dim)):
    newSource = moveDirection(sortedDim[i][0], source, dest, linkSet)
    source = newSource


#compute set of links that the first job uses
def determineLinkSet(nodeListJobOne):
  linksJobOne = defaultdict(lambda: 0)  #all entries initialized to zero
  doRoutingPatrick(nodeListJobOne[0], nodeListJobOne[1], linksJobOne)
  return linksJobOne

#compute set of links that the second job causes
def determineLinkConflicts(nodeListJobTwo, linksJobOne):
  linksJobTwo = defaultdict(lambda: 0)  #all entries initialized to zero
  conflictedLinks = []   #list holding every conflicted link
  doRouting(nodeListJobTwo, linksJobTwo)
  for i in linksJobOne:
    if i in linksJobTwo:
      conflictedLinks.append((i,linksJobOne[i]+linksJobTwo[i]))
  return linksJobTwo, conflictedLinks

def traversals(tups):
  sum = 0
  for tup in tups:
    sum+=tup[1]
  return sum

def main(argv1,argv2,ret):
  nodeListJobOne = readNodeSet(argv1)
  nodeListJobTwo = readNodeSet(argv2)
  linksJobOne = determineLinkSet(nodeListJobOne)
  linksJobTwo, conflictLinks = determineLinkConflicts(nodeListJobTwo, linksJobOne)
  
  print ""
  sum = 0
  for i in conflictLinks:
    sum += 1
    print i
  print "total number of conflicted links", sum
  print "Nodes Job 1: ", len(nodeListJobOne), " Nodes Job 2:  ", len(nodeListJobTwo), " Shared Nodes (should be 0): ", len(set(nodeListJobOne) & set(nodeListJobTwo))
  print "traversals: {}".format(traversals(conflictLinks))
  if(ret == 0):
    return len(conflictLinks)
  else:
    return conflictLinks

if __name__ == "__main__":
  if(len(sys.argv) == 3):
    sys.exit(main(sys.argv[1], sys.argv[2]),0)
  else:
    sys.exit(main(sys.argv[1], sys.argv[2]),1)




