//
// Created by Gianmarco Cherchi on 07/03/24.
//

#ifndef MESH_ARRANGEMENT_CUSTOM_STACK_H
#define MESH_ARRANGEMENT_CUSTOM_STACK_H

#include <vector>
#include <cassert>

class CustomStack
{
    CustomStack(int preallocate_size)
    {
        stack.resize(preallocate_size);
        cursor = -1;
    }

    std::vector<uint> pop()
    {
        cursor -= 1;
        return stack.at(cursor +1);
    }

    void push(std::vector<uint> new_vec)
    {
        if(cursor == stack.size() -1)
            stack.push_back(new_vec);
        else
            stack[cursor +1] = new_vec;

        cursor++;
    }

    const std::vector<std::vector<uint>> &getStack(int &size)
    {
        size = cursor +1;
        return stack;
    }

    std::vector<uint> getSingleVector(int index)
    {
        assert(index <= cursor && "Index out of range");
        return stack[index];
    }

    void clearSingleVector(int index)
    {
        assert(index <= cursor && "Index out of range");
        stack.at(index).clear();
    }

    int findTriplet(uint v0, uint v1, uint v2)
    {
        for(uint i = 0; i <= cursor; i++)
        {
            assert(stack[i].size() >= 3);
            if(stack[i][0] == v0 && stack[i][1] == v2 && stack[i][2] == v2)
                return i;
        }

        assert(false && "Triplet not found!");
        return -1; // Triplet not found
    }

    private:
        std::vector<std::vector<uint>> stack;
        int cursor;

};


#endif //MESH_ARRANGEMENT_CUSTOM_STACK_H
