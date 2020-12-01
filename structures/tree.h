#ifndef TREE_H
#define TREE_H

#include "predicates/indirect_predicates.h"

#include <vector>

typedef unsigned int uint;

struct Node
{
    Node(){}

    Node(const uint &_v0, const uint &_v1, const uint &_v2) :v0(_v0), v1(_v1), v2(_v2)
    {
        children[0] = -1;
        children[1] = -1;
        children[2] = -1;
    }

    uint v0, v1, v2;
    int children[3];
};


class Tree
{
    public:

        Tree() {}

        Tree(const uint &size)
        {
            nodes.reserve(size);
        }

        inline uint addNode(const uint &v0, const uint &v1, const uint &v2)
        {
            nodes.emplace_back(v0, v1, v2);
            return static_cast<uint>(nodes.size()) -1;
        }

        inline const Node &getNode(const uint &node_id) const
        {
            assert(node_id < nodes.size() && "out fo range node id");
            return nodes[node_id];
        }

        inline void addChildren(const uint &node_id, const uint &c0, const uint &c1)
        {
            assert(node_id < nodes.size() && "out fo range node id");
            assert(nodes[node_id].children[0] == -1 && "assigning no empty children list");

            nodes[node_id].children[0] = static_cast<int>(c0);
            nodes[node_id].children[1] = static_cast<int>(c1);
        }

        inline void addChildren(const uint &node_id, const uint &c0, const uint &c1, const uint &c2)
        {
            assert(node_id < nodes.size() && "out fo range node id");
            assert(nodes[node_id].children[0] == -1 && "assigning no empty children list");

            nodes[node_id].children[0] = static_cast<int>(c0);
            nodes[node_id].children[1] = static_cast<int>(c1);
            nodes[node_id].children[2] = static_cast<int>(c2);
        }


    private:
        std::vector<Node> nodes;
};




#endif // TREE_H
