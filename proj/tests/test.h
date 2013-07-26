#import <Foundation/NSObject.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSString.h>
#import <CoreLocation/CLError.h>

@class foobar;

typedef enum {
	zero,
	one,
	two
} enumtype;

enum tag{
	three = 3,
	four,
	five
};

typedef enum tag2{
	six = 6,
	seven,
	eight
} enumtype2;

@protocol anotherProtocol <NSObject>
	+(void) someOtherStaticFunction;

@end

@protocol testProtocol <anotherProtocol, NSObject>
	
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

@interface qux: foobar <testProtocol> {

}

+(void)testSomething:(qux *)arg;
-(void)testEnumType:(enumtype)arg;
-(enum tag)testEnumTag;
-(enum tag2)testEnumTag2:(enumtype2)arg;

@end




