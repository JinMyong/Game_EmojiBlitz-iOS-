#include "IOSManager.h"
#import "GameCenterManager.h"

static IOSManager   _shareIOSManger;

IOSManager* IOSManager::shared()
{
    return & _shareIOSManger;
}

void IOSManager::showLeaderBoard()
{
    [[GameCenterManager shared] showLeaderBoard];
}

void IOSManager::sendScore(int score, const char *cagatory)
{
    [[GameCenterManager shared] reportScore:score forCategory:[NSString stringWithUTF8String:cagatory]];
}

void IOSManager::loginGameCenter() {
    [[GameCenterManager shared] login];
}