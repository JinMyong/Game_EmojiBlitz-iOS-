#ifndef __IOSManager_h__
#define __IOSManager_h__

class IOSManager
{
public:
    static IOSManager* shared();
    void loginGameCenter();
    void showLeaderBoard();
    void sendScore(int score, const char* cagatory);
};


#endif