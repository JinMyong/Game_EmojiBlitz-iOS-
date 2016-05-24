//
//  SocialGame.h
//  AlephBet
//
//  Created by Louis on 2/1/16.
//
//

#ifndef SocialGame_h
#define SocialGame_h

class SocialGame {
public:
    static bool showRewardVideo();
    static void getCoin(int cost);
    static void completeGetCoin(int coins);
    static void completeRewardVideo();
    
    static void confirmMessage();
    static void getConfirmResult(bool result);
};

#endif /* SocialGame_h */
