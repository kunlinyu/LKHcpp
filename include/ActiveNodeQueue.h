#pragma once

#include <set>
#include <queue>

class Node;

class ActiveQueue {
private:
    std::set<Node *> nodes;
    std::queue<Node *> queue;

public:
    /* If a node is made "active", attempts are made to find an improving
     * move with the node as anchor node, t1.
     * The push function makes a node active by inserting it into a
     * queue of active nodes.
     * The function is called from the LinKernighan and StoreTour function.
     */
    void push(Node *node) {
        if (nodes.find(node) != nodes.end()) return;
        nodes.insert(node);
        queue.push(node);
    }

    /* The pop function removes the first node in the queue of "active" nodes
     * (i.e., nodes to be tried as an anchor node, t1, by the LinKernighan algorithm).
     * The function returns a pointer to the removed node.
     * The list must not be empty before the call.
     */
    Node *pop() {
        if (queue.empty()) return nullptr;
        Node *node = queue.front();
        queue.pop();
        nodes.erase(node);
        return node;
    }
};
