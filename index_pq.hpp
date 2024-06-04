#ifndef INDEX_PRIORITY_QUEUE_HPP_
#define INDEX_PRIORITY_QUEUE_HPP_

#include <vector>
#include <algorithm>
#include <stdexcept>

template <typename T>
class IndexPriorityQueue {
 private:
  // vector to hold priorities.  
  // priorities.at(i) is the priority associated to index i
  // the heap will only contain indices, we look up priorities
  // in this vector as needed
  std::vector<T> priorities {};
  // priorityQueue stores indices: priorityQueue.at(i) is an index
  // priorityQueue functions as the heap and is heap ordered: 
  // priorities.at(priorityQueue.at(i)) <= priorities.at(priorityQueue.at(2 * i)) 
  // priorities.at(priorityQueue.at(i)) <= priorities.at(priorityQueue.at(2 * i) + 1) 
  std::vector<int> priorityQueue {};
  // indexToPosition.at(i) is the position in priorityQueue of index i
  // priorityQueue.at(indexToPosition.at(i)) = i
  // indexToPosition.at(priorityQueue.at(j)) = j
  std::vector<int> indexToPosition {};
  // Size of priority queue
  int size_ = 0;

 public:
  // Constructor with N indices
  explicit IndexPriorityQueue(int n);
  // Push new element with priority p and index i
  void push(const T& p, int i);
  // Pop element with highest priority
  void pop();
  // Remove element at index i
  void erase(int i);
  // Check if queue contains index i
  bool contains(int i) const;
  // Change the priority p and of element at index i
  void changeKey(const T& p, int i);
  // Get element with highest priority
  std::pair<T, int> top() const;
  // Return if queue is empty
  bool empty() const;
  // Return size of queue
  int size() const;

 private:
  // Useful helper functions for heap operations
  int leftChild(int i) {
    return 2*i;
  }

  int rightChild(int i) {
    return 2*i + 1;
  }

  int parent(int i) {
    return i/2;
  }

  // update heap order by moving element up
  void swim(int i) {
    while (i > 1 && higherPriority(i, parent(i))) {
      swap(i, parent(i));
      i = parent(i);
    }
  }

  // update heap order by moving element down
  void sink(int i) {
    while (leftChild(i) <= size_) {
      int j = leftChild(i);
      // Increment j if next element has higher priority
      if (j < size_ && higherPriority(j + 1, j)) {
        j++;
      } 
      if (higherPriority(j, i)) {
        swap(i, j);
        i = j;
      } else {
        break;
      }
    }
  }

  // Check if priority at index i is higher than j
  bool higherPriority(int i, int j) const {
    if (priorityQueue[i] == -9999 || priorityQueue[j] == -9999) {
      throw std::runtime_error("Bad Index");
    }
    return priorities[priorityQueue[i]] < priorities[priorityQueue[j]];
  }

  // Swap elements at index i and j in the queue and update related variables
  void swap(int i, int j) {
    std::swap(priorityQueue[i], priorityQueue[j]);
    indexToPosition[priorityQueue[i]] = i;
    indexToPosition[priorityQueue[j]] = j;
  }
 };

// IndexPriorityQueue member functions
// Constructor which initialises queue to store N indices. Indexing starts at 1 for priorityQueue
template <typename T>
IndexPriorityQueue<T>::IndexPriorityQueue(int N) {
  if (N <= 0) {
    throw std::invalid_argument("Size needs to be > 0");
  }
  // Init the entries to blank values
  priorities = std::vector<T>(N, T());
  indexToPosition = std::vector<int>(N, -9999);
  priorityQueue.resize(N + 1, -9999);
}

// Check if queue is empty
template <typename T>
bool IndexPriorityQueue<T>::empty() const {
  return size_ == 0;
}

// Return the size of the queue
template <typename T>
int IndexPriorityQueue<T>::size() const {
  return size_;
}

// Push a new index with a given priority
template <typename T>
void IndexPriorityQueue<T>::push(const T& priority, int index) {
  if (index < 0 || index >= static_cast<int>(priorities.size())) {
    throw std::invalid_argument("Out of bounds");
  }
  if (contains(index)) {
    return;
  } else if (index >= 0 && index < static_cast<int>(priorities.size())) {
    size_++; // Increment size
    indexToPosition[index] = size_; // Update index position to subsequently add to end of heap
    priorityQueue[size_] = index; // Add index to end of heap
    priorities[index] = priority; // Define priority
    swim(size_); // Reorder the heap
  } else {
    throw std::runtime_error("Bad index");
  }
}

// Pop element with highest priority
template <typename T>
void IndexPriorityQueue<T>::pop() {
  if (!empty()) { // Check queue not empty before popping
    swap(1, size_); // Swap last element and first element in priorityQueue
    indexToPosition[priorityQueue[size_]] = -9999; // Set last element to default
    size_--; // Decrease size of queue
    if (!empty()) { // Ensure queue is not empty before altering heap
      sink(1); // Reorder heap
    }
  }
  else {
    throw std::runtime_error("Empty pop attempt");
  }
}

// Remove item at given index
template <typename T>
void IndexPriorityQueue<T>::erase(int index) {
  if (contains(index)) { // Ensure queue contains the provided index
    int position = indexToPosition[index]; // Get position of index
    swap(position, size_); // Swap last element with index
    indexToPosition[priorityQueue[size_]] = -9999; // Set last element to be default value
    size_--; // Reduce size
    // Reorder heap
    swim(position); // Swim to top
    sink(position); // Sink element to correct location
  } else {
    return;
  }
}

// Return element with highest priority
template <typename T>
std::pair<T, int> IndexPriorityQueue<T>::top() const {
  if (empty()) { // Ensure not queue is not empty
    throw std::runtime_error("Empty priority queue");
  } else {
    return {priorities[priorityQueue[1]], priorityQueue[1]};
  }
}

// if vertex i is not present, insert it with key
// otherwise change the associated key value of i to key
template <typename T>
void IndexPriorityQueue<T>::changeKey(const T& key, int index) {
  if (index < 0 || index >= static_cast<int>(priorities.size())) {
    throw std::invalid_argument("Out of bounds");
  }
  // Update value of index to key
  if (contains(index)) {
    priorities[index] = key; // Set priority to key
    int position = indexToPosition[index];
    // Reorder heap
    swim(position); // Swim to top
    sink(position); // Sink down to correct position
  // Insert index with key
  } else {
    push(key, index);
  }
}

// Check if queue contains element at index
template <typename T>
bool IndexPriorityQueue<T>::contains(int index) const {
  return index >= 0 && index < static_cast<int>(indexToPosition.size()) && indexToPosition[index] != -9999;
}

#endif      // INDEX_PRIORITY_QUEUE_HPP_
