//
//  StoreDialogLayer.hpp
//  emojimuncher
//
//  Created by Louis on 2/25/16.
//
//

#ifndef StoreDialogLayer_hpp
#define StoreDialogLayer_hpp

#include <stdio.h>
#include "cocos2d.h"

using namespace cocos2d;
enum
{
    BTN_COIN_100 = 0,
    BTN_COIN_250,
    BTN_COIN_500,
    BTN_COIN_5000
    
};
class StoreDialogLayer:public Layer
{
public:
    virtual bool init();
    
    void menuFreeCoinCallback(Ref* pSender);
    void menuPurchaseCoinCallback(Ref* pSender);
    void menuCloseCallback(Ref* pSender);
    
    void completeGetFreecoins();
    
    CREATE_FUNC(StoreDialogLayer);
};

#endif /* StoreDialogLayer_hpp */
