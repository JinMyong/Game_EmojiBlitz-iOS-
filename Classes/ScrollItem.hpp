//
//  ScrollItem.hpp
//  emojimuncher
//
//  Created by Louis on 2/25/16.
//
//

#ifndef ScrollItem_hpp
#define ScrollItem_hpp

#include <stdio.h>
#include "cocos2d.h"

using namespace cocos2d;

class ScrollItem : public Node
{
public:
    static ScrollItem* create(int cost, const char* nor_fileName, const char* prs_fileName, int index);    
    
};

#endif /* ScrollItem_hpp */
