#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>
#import "AppController.h"


@interface GameCenterManager : NSObject <GKLeaderboardViewControllerDelegate,GKAchievementViewControllerDelegate> {
}

+ (GameCenterManager*)shared;

- (void)login;

- (void) reportScore: (int64_t) score forCategory: (NSString*) category;

- (void) showLeaderBoard;

- (void) postScore:(const char*)idName score:(NSNumber*)score;

@end
