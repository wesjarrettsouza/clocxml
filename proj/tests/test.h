#import <Foundation/NSObject.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSString.h>
#import <CoreLocation/CLError.h>

@class foobar;

enum {
	zero,
	one,
	two
};

@protocol testProtocol
	
	+(void)testStaticProtocolFunction:(NSArray*)array;
	-(void)testMemberProtocolFunction:(NSDictionary*)dict;
@end

typedef int (^handler)(const id uniquename[], int two, void* three, BOOL four);


@interface foobar : NSObject {

}

@property (nonatomic, assign) IBOutlet id<NSObject> foobar;

+(void)testFunctionPointer:(handler)foo;
-(void)hello:(NSString*)hi something:(BOOL)qux another:(NSUInteger)what;
-(void)testProtoclInline:(void (^)(NSString* one))huh;
@end

@interface qux: foobar <NSObject> {

}

+(void)testSomething:(qux *)arg;

@end




