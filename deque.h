// File: deque.h
// Uros Bojanic 2019/0077
#ifndef DEQUE_H_
#define DEQUE_H_

// Preprocessor directives
#include "lock.h"
#include <stdio.h>

// Deque
template <class T>
class Deque {
public:
	// Constructor - creates a new empty deque
	Deque() : head(NULL), tail(NULL), curr(NULL), numOfNodes(0) {
		/* Initializes a doubly linked list with head and tail.
		 * Also, initial number of threads in the deque is 0.
		 * No dynamic memory is allocated at this point.
		 */
	};
	// Destructor - deletes this deque
	~Deque() {
		/* Deletes the doubly linked list with head and tail.
		 * All dynamic memory is deleted by the end of this method.
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		while (head) {
			Node* temp = head;
			head = head->next;
			delete temp;
			temp = NULL;
		}
		head = NULL;
		tail = NULL;
		numOfNodes = 0;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};

	// Adds element to the front of this deque
	void push_front(T data) {
		/* Inserts an element into the deque (at the beginning).
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (!data) return;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		Node* node = new Node(data);
		if (!node) { // overflow
			printf("Allocation failed! (deque.h)\n");
			return;
		}
		if (numOfNodes++ == 0) {
			tail = node;
		} else {
			node->next = head;
			head->prev = node;
		}
		head = node;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};
	// Adds element to the back of this deque
	void push_back(T data) {
		/* Inserts an element into the deque (at the end).
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (!data) return;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		Node* node = new Node(data);
		if (!node) { // overflow
			printf("Allocation failed! (deque.h)\n");
			return;
		}
		if (numOfNodes++ == 0) {
			head = node;
		} else {
			tail->next = node;
			node->prev = tail;
		}
		tail = node;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};
	// Removes element from the front of this deque
	void pop_front() {
		/* Removes an element from the deque (from the end).
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (numOfNodes == 0) return; // underflow
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		Node* temp = head;
		head = head->next;
		if (head == NULL) {
			tail = NULL;
		} else {
			head->prev = NULL;
		}
		delete temp;
		temp = NULL;
		numOfNodes--;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};
	// Removes element from the back of this deque
	void pop_back() {
		/* Removes an element from the deque (from the end).
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (numOfNodes == 0) return; // underflow
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		Node* temp = tail;
		tail = tail->prev;
		if (tail == NULL) {
			head = NULL;
		} else {
			tail->next = NULL;
		}
		delete temp;
		temp = NULL;
		numOfNodes--;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};
	// Removes specific element from this deque
	void remove(T data) {
		/* Removes a specific element from the deque.
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (!data) return;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		Node* temp = head;
		while (temp) {
			if (temp->data == data) {
				Node* next = temp->next;
				Node* prev = temp->prev;
				temp->next = NULL;
				temp->prev = NULL;
				if (next) {
					next->prev = prev;
				} else {
					tail = prev;
				}
				if (prev) {
					prev->next = next;
				} else {
					head = next;
				}
				delete temp;
				temp = NULL;
				numOfNodes--;
				break;
			}
			temp = temp->next;
		}
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
	};

	// Returns the element from the front of this deque
	T front() {
		if (!head) return 0;
		return head->data;
	};
	// Returns the element from the back of this deque
	T back() {
		if (!tail) return 0;
		return tail->data;
	};
	// Returns if specific element is in this deque
	int isFound(T data) {
		/* Checks if a specific element is present in this deque.
		 * This method is locked because it interferes
		 * directly with element ordering and records.
		 */
		if (!data) return 0;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_lock(); // lock
#endif
		int dataFound = 0;
		Node* temp = head;
		while (temp) {
			if (temp->data == data) {
				dataFound = 1;
				break;
			}
			temp = temp->next;
		}
		temp = NULL;
#ifndef BCC_BLOCK_IGNORE
		prevent_context_switching_unlock(); // unlock
#endif
		return dataFound;
	};

	// Returns number of elements in this deque
	int numOfElements() {
		return numOfNodes;
	};
	// Returns if this deque is empty (has 0 elements)
	int isEmpty() {
		return (numOfNodes == 0);
	};

	// Erases iterator value (sets is to NULL)
	void toNull() {
		curr = NULL;
	};
	// Checks if the current element (iterator) exists
	int isThere() {
		return (curr != NULL);
	};
	// Returns current element (pointed to by iterator)
	T getCurr() {
		if (curr) return curr->data;
		return 0;
	}
	// Moves iterator to the start (front of this deque)
	void toFront() {
		curr = head;
	};
	// Moves iterator to the end (back of this deque)
	void toBack() {
		curr = tail;
	};
	// Moves iterator to next element (or NULL if curr is tail)
	void toNext() {
		if (curr) curr = curr->next;
	};
	// Moves iterator to previous element (or NULL if curr is head)
	void toPrev() {
		if (curr) curr = curr->prev;
	};

private:
	// Deque will be implemented as a doubly linked list of Nodes
	class Node {
	public:
		// Each Node holds a single piece of template class data
		T data;
		// And pointers to the next and previous Node in the deque
		Node *next, *prev;
		// Constructor - allocates a new Node (but linking has to be done later)
		Node(T data) : data(data), next(NULL), prev(NULL) {};
	};
	Node *head, *tail, *curr;
	int numOfNodes;
};

#endif /* DEQUE_H_ */
