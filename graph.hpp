///////////////////////////////////////////////////////////////
// REFERENCES:
// singleSourceLazy DEVELOPED USING WEEK 10 TUTORIAL SOLUTION
///////////////////////////////////////////////////////////////

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <iostream>
#include <fstream>
#include <utility>
#include <functional>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <unordered_map>
#include <limits>
#include "my_integer.hpp"
#include "index_pq.hpp"

template <typename T>
class Graph {
 private:
  std::vector<std::unordered_map<int, T> > adjList {};
  int numVertices {};

 public:
  // empty graph with N vertices
  explicit Graph(int N);

  // construct graph from edge list in filename
  explicit Graph(const std::string& filename);

  // add an edge directed from vertex i to vertex j with given weight
  void addEdge(int i, int j, T weight);

  // removes edge from vertex i to vertex j
  void removeEdge(int i, int j);

  // is there an edge from vertex i to vertex j?
  bool isEdge(int i, int j) const;

  // return weight of edge from i to j
  // will throw an exception if there is no edge from i to j
  T getEdgeWeight(int i, int j) const;

  // returns number of vertices in the graph
  int size() const;

  // alias a const iterator to our adjacency list type to iterator
  using iterator = 
  typename std::vector<std::unordered_map<int, T> >::const_iterator;

  // cbegin returns const iterator pointing to first element of adjList
  iterator begin() const {
    return adjList.cbegin();
  }

  iterator end() const {
    return adjList.cend();
  }

  // return iterator to a particular vertex
  iterator neighbours(int a) const {
    return adjList.begin() + a;
  }
};

template <typename T>
Graph<T>::Graph(int N) : adjList(N), numVertices {N} {}

template <typename T>
Graph<T>::Graph(const std::string& inputFile) {
  std::ifstream infile {inputFile};
  if (!infile) {
    std::cerr << inputFile << " could not be opened\n";
    return;
  }
  // first line has number of vertices
  infile >> numVertices;
  adjList.resize(numVertices);
  int i {};
  int j {};
  double weight {};
  // assume each remaining line is of form
  // origin dest weight
  while (infile >> i >> j >> weight) {
    addEdge(i, j, static_cast<T>(weight));
  }
}

template <typename T>
int Graph<T>::size() const {
  return numVertices;
}

template <typename T>
void Graph<T>::addEdge(int i, int j, T weight) {
  if (i < 0 or i >= numVertices or j < 0 or j >= numVertices) {
    throw std::out_of_range("invalid vertex number");
  }
  adjList[i].insert({j, weight});
}

template <typename T>
void Graph<T>::removeEdge(int i, int j) {
  // check if i and j are valid
  if (i >= 0 && i < numVertices && j >= 0 && j < numVertices) {
    adjList[i].erase(j);
  }
}

template <typename T>
bool Graph<T>::isEdge(int i, int j) const {
  if (i >= 0 && i < numVertices && j >= 0 && j < numVertices) {
    return adjList.at(i).contains(j);
  }
  return false;
}

template <typename T>
T Graph<T>::getEdgeWeight(int i, int j) const {
  return adjList.at(i).at(j);
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Graph<T>& G) {
  for (int i = 0; i < G.size(); ++i) {
    out << i << ':';
    for (const auto& [neighbour, weight] : *(G.neighbours(i))) {
      out << " (" << i << ", " << neighbour << ")[" << weight << ']';
    }
    out << '\n';
  }
  return out;
}

// End of functions from Graph class

// return a number of type T to stand in for "infinity"
template <typename T>
T infinity() {
  if (std::numeric_limits<T>::has_infinity) {
    return std::numeric_limits<T>::infinity();
  } else if constexpr (std::is_same_v<T, MyInteger>) {
    return MyInteger {std::numeric_limits<int>::max()};
  } else {
    return std::numeric_limits<T>::max();
  }
}

// Shortest path tree check functions
template <typename T>
bool isSubgraph(const Graph<T>& H, const Graph<T>& G) {
  if (H.size() > G.size()) {
    return false;
  }
  for (int i = 0; i < H.size(); i++) {
    for (const auto& neighbour : *(H.neighbours(i))) {
      if (!G.isEdge(i, neighbour.first) || G.getEdgeWeight(i, neighbour.first) != neighbour.second) {
        return false;
      }
    }
  }
  return true;
}

template <typename T>
bool isTreePlusIsolated(const Graph<T>& G, int root) {
  if (root < 0 || root >= G.size()) {
    throw std::out_of_range("Bad root");
  }
  std::vector<bool> visitedList(G.size(), false);
  std::queue<int> q;
  std::vector<int> parentsList(G.size());
  q.push(root);
  visitedList[root] = true;
  while (!q.empty()) {
    int currentVertex = q.front();
    q.pop();
    for (const auto& neighbour : *(G.neighbours(currentVertex))) {
      if (!visitedList[neighbour.first]) {
        visitedList[neighbour.first] = true;
        q.push(neighbour.first);
        parentsList[neighbour.first] = currentVertex;
      } else {
        if (currentVertex != parentsList[neighbour.first]) {
          return false;
        }
      }
    }
  }
  for (int i = 0; i < G.size(); i++) {
    if (!G.neighbours(i) -> empty() && !visitedList[i]) {
      return false;
    }
  }
  return true;
}

template <typename T>
std::vector<T> pathLengthsFromRoot(const Graph<T>& tree, int root) {
  if (root < 0 || root >= tree.size()) {
    throw std::out_of_range("Bad root");
  }
  std::vector<T> bestDistanceTo(tree.size(), infinity<T>());  
  std::queue<int> q;
  if constexpr (std::is_same_v<T, MyInteger>) {
    bestDistanceTo[root] = MyInteger(0);
  } else {
    bestDistanceTo[root] = 0;
  }
  
  q.push(root);
  while(!q.empty()) {
    int currentVertex = q.front();
    q.pop();
    for (const auto& neighbour : *(tree.neighbours(currentVertex))) {
      if (bestDistanceTo[neighbour.first] == infinity<T>()) {
        bestDistanceTo[neighbour.first] = bestDistanceTo[currentVertex] + neighbour.second;
        q.push(neighbour.first);
      }
    }
  }
  return bestDistanceTo;
}

template <typename T>
bool allEdgesRelaxed(const std::vector<T>& bestDistanceTo, const Graph<T>& G, int source) {
  if (source < 0 || source >= G.size()) {
    throw std::out_of_range("Bad source");
  }
  if (G.size() == 1) {
    if constexpr (std::is_same_v<T, MyInteger>) {
      return bestDistanceTo[source] == MyInteger(0);
    } else {
      return bestDistanceTo[source] == 0; 
    }
  } else {
    for (int i = 0; i < G.size(); i++) {
      for (const auto& neighbour : *(G.neighbours(i))) {
        if (bestDistanceTo[i] != infinity<T>() && bestDistanceTo[neighbour.first] > bestDistanceTo[i] + neighbour.second) {
          return false;
        }
      }
    }
    return true;
  }
}

template <typename T>
bool correctShortestPathTree(const Graph<T>& originalGraph, const Graph<T>& shortestPathTree, int source) {  
  return isSubgraph(shortestPathTree, originalGraph) && isTreePlusIsolated(shortestPathTree, source) && allEdgesRelaxed(pathLengthsFromRoot(shortestPathTree, source), originalGraph, source);
}

// lazy solution as in Tutorial Week 10
template <typename T>
std::vector<T> singleSourceLazyDistance(const Graph<T>& G, int source) {
  // alias the long name for a minimum priority queue holding
  // objects of type DistAndVertex
  using DistAndVertex = std::pair<T, int>;
  using minPQ = std::priority_queue<DistAndVertex,
                                  std::vector<DistAndVertex>,
                                  std::greater<DistAndVertex> >;
  minPQ queue {};
  queue.push({T {}, source});
  // record best distance to vertex found so far
  int N = G.size();
  std::vector<T> bestDistanceTo(N, infinity<T>());
  bestDistanceTo.at(source) = T {};
  // being in visited means we have already explored a vertex's neighbours
  // the bestDistanceTo for a vertex in visited is the true distance.
  std::vector<bool> visited(N);
  while (!queue.empty()) {
    auto [dist, current] = queue.top();
    queue.pop();
    // as we use a lazy version of Dijkstra a vertex can appear multiple
    // times in the queue.  If we have already visited the vertex we
    // take out of the queue we just go on to the next one
    if (visited.at(current)) {
      continue;
    }
    visited.at(current) = true;
    // relax all outgoing edges of current
    for (const auto& [neighbour, weight] : *(G.neighbours(current))) {
      if (weight < T{0}) {
        throw std::runtime_error("G contains negative weight");
      }
      T distanceViaCurrent = bestDistanceTo.at(current) + weight;
      if (bestDistanceTo.at(neighbour) > distanceViaCurrent) {
        bestDistanceTo.at(neighbour) = distanceViaCurrent;
        // lazy dijkstra: nextPoint could already be in the queue
        // we don't update it with better distance just found.
        queue.push({distanceViaCurrent, neighbour});
      }
    }
  }
  return bestDistanceTo;
}


// Implement your solution using an index priority queue here
template <typename T>
Graph<T> singleSourceIndex(const Graph<T>& G, int source) {

  int size = G.size();

  IndexPriorityQueue<T> pq(size);
  std::vector<T> bestDistanceTo(size, infinity<T>());
  std::vector<int> parentList(size, -9999);

  bestDistanceTo[source] = T{};
  parentList[source] = source;
  pq.push(T{}, source);

  while (!pq.empty()) {
    auto [currentDistance, currentNode] = pq.top();
    pq.pop();
    if (currentDistance <= bestDistanceTo[currentNode]) {
      for (const auto& [neighbour, weight] : *(G.neighbours(currentNode))) {
        if (weight < T{0}) {
          throw std::runtime_error("G contains negative weight");
        }
        T distanceViaCurrent = bestDistanceTo[currentNode] + weight;
        if (distanceViaCurrent < bestDistanceTo[neighbour]) {
          bestDistanceTo[neighbour] = distanceViaCurrent;
          parentList[neighbour] = currentNode;
          pq.changeKey(distanceViaCurrent, neighbour); // Update or push neighbour in the queue
        }
      }
    }
  }
  Graph<T> shortestPathTree(size);
  for (int i = 0; i < size; i++) {
    if (parentList[i] != -9999 && i != source) {
      shortestPathTree.addEdge(parentList[i], i, G.getEdgeWeight(parentList[i], i));
    }
  }
  return shortestPathTree;
} 

// Implement your lazy solution using std::priority_queue here
template <typename T>
Graph<T> singleSourceLazy(const Graph<T>& G, int source) {

  using DistAndVertex = std::pair<T, int>;
  using MinPQ = std::priority_queue<DistAndVertex, std::vector<DistAndVertex>, std::greater<DistAndVertex>>;
  
  int size = G.size();
  MinPQ q = {};
  q.push({T{}, source});

  std::vector<T> bestDistanceTo(size, infinity<T>());
  std::vector<int> parentList(size, -9999);
  std::vector<bool> visitedList(size);

  bestDistanceTo[source] = T{};
  parentList[source] = source;

  while (!q.empty()) {
    auto [currentDistance, currentNode] = q.top();
    q.pop();
    if (!visitedList[currentNode]) {
      visitedList[currentNode] = true;
      for (const auto& [neighbour, weight] : *(G.neighbours(currentNode))) {
        if (weight < T{0}) {
          throw std::runtime_error("G contains negative weight");
        }
        T distanceViaCurrent = bestDistanceTo[currentNode] + weight;
        if (bestDistanceTo[neighbour] > distanceViaCurrent) {
          bestDistanceTo[neighbour] = distanceViaCurrent;
          parentList[neighbour] = currentNode;
          q.push({distanceViaCurrent, neighbour});
        }
      }
    } 
  }
  Graph<T> shortestPathTree(size);
  for (int i = 0; i < size; i++) {
    if (parentList[i] != -9999 && i != source) {
      shortestPathTree.addEdge(parentList[i], i, G.getEdgeWeight(parentList[i], i));
    }
  }
  return shortestPathTree;
}

// Implement your solution using std::set here
template <typename T>
Graph<T> singleSourceSet(const Graph<T>& G, int source) {
  int size = G.size();

  using DistVertexPair = std::pair<T, int>;
  std::set<DistVertexPair> pq; 
  std::vector<T> bestDistanceTo(size, infinity<T>());
  std::vector<int> parentList(size, -9999); 

  bestDistanceTo[source] = T{};
  parentList[source] = source;
  pq.insert({T{}, source});

  while (!pq.empty()) {
    auto [currentDistance, currentNode] = *pq.begin();
    pq.erase(pq.begin());
    for (const auto& [neighbour, weight] : *(G.neighbours(currentNode))) {
      if (weight < T{0}) {
        throw std::runtime_error("G contains negative weight");
      }
      T distanceViaCurrent = currentDistance + weight;
      if (distanceViaCurrent < bestDistanceTo[neighbour]) {
        if (bestDistanceTo[neighbour] != infinity<T>()) {
          pq.erase({bestDistanceTo[neighbour], neighbour});
        }
        
        parentList[neighbour] = currentNode;
        bestDistanceTo[neighbour] = distanceViaCurrent;
        pq.insert({distanceViaCurrent, neighbour});
        }
      }
    }
    Graph<T> shortestPathTree(size);
    for (int i = 0; i < size; i++) {
      if (parentList[i] != -9999 && i != source) {
        shortestPathTree.addEdge(parentList[i], i, G.getEdgeWeight(parentList[i], i));
      }
    }
    return shortestPathTree;
}

// put your "best" solution here
// this is the one we will use for performance testing
template <typename T>
Graph<T> singleSourceShortestPaths(const Graph<T>& G, int source) {
  return singleSourceIndex(G, source); 
}

#endif      // GRAPH_HPP_
