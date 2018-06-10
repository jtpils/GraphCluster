/*
Copyright (c) 2018, Yu Chen
All rights reserved.
*/

/** \file UnionFind.h
 *	\brief data structures
 */
#ifndef UNION_FIND_H
#define UNION_FIND_H

namespace bluefish
{
    /** @brief Union-find data structure, used in various graph algorithms
    */
    class UnionFind
    {
    public:
        UnionFind(size_t n);
        ~UnionFind();
        size_t Find(size_t x);
        bool UnionSet(size_t x, size_t y);

        size_t *father;
        size_t set_num;
        size_t size;
    };

}	// end of namespace bluefish


#endif	//bluefish_UNIONFIND_H