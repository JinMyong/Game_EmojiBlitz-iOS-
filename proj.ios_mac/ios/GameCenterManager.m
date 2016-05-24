#import "GameCenterManager.h"
#include "Config.h"

@implementation GameCenterManager

static GameCenterManager* _shareGameCenter = nil;

+ (GameCenterManager*)shared
{
    if (_shareGameCenter == nil) {
        _shareGameCenter = [[self alloc] init];
    }
    
    return _shareGameCenter;
}


- (void)reportScore:(int64_t)score forCategory:(NSString *)category
{
    
    GKScore *myScoreValue = [[[GKScore alloc] initWithCategory:category] autorelease];
    myScoreValue.value = score;
    
    [myScoreValue reportScoreWithCompletionHandler:^(NSError *error){
        if(error != nil){
            NSLog(@"Score Submission Failed");
        } else {
            NSLog(@"Score Submitted");
        }
        
    }];
    
}

- (void)showLeaderBoard
{
    UIWindow* window = [[UIApplication sharedApplication] keyWindow];
    if (!window) {
        window = (UIWindow*)[[[UIApplication sharedApplication] windows] objectAtIndex:0];
    }
    
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error) {
        UIViewController* root = [window rootViewController];
        GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
        if (leaderboardController != NULL)
        {
            leaderboardController.timeScope = GKLeaderboardTimeScopeWeek;
            leaderboardController.leaderboardDelegate = self;
            [root presentViewController: leaderboardController animated: YES completion:nil];
            [leaderboardController release];
        }
    }];
    
}


- (void) postScore:(const char*)idName score:(NSNumber*)score
{
    GKAchievement* achievement = [[[GKAchievement alloc] init] autorelease];
    achievement.identifier = [NSString stringWithUTF8String:idName];
    achievement.percentComplete = [score doubleValue];
    achievement.showsCompletionBanner = YES;
    
    [achievement reportAchievementWithCompletionHandler:^(NSError* error) {
        if (error) {
            NSLog(@"[GameCenter] postAchievement for %s failed: %@", idName, error.localizedDescription);
        }
    }];
}


- (void)login {
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error) {
        if (!error) {
            NSLog(@"login success!");
        }
    } ];
}

- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [viewController dismissViewControllerAnimated:YES completion:nil];
}

- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController
{
    [viewController dismissViewControllerAnimated:YES completion:nil];
}


@end