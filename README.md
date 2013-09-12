clocxml
===

Description
---
Clocxml is a [Clang Tool](http://clang.llvm.org/docs/LibTooling.html) that produces an XML representation of Objective-C types for a specified set of files. Accurate and thorough metadata for Objective-C interface, category, and protocol declarations is output with natural XML element names and formatting. You can use this tool to quickly list out all instance and class methods that an interface declares, the type, kind and name of each parameter, all protocols that an interface conforms to and other useful information. Clocxml has an easy-to-use command line front-end and outputs XML which makes for quick integration with applications that need Objective-C metadata.

Motivation
---
After using the Mac OS [gen_bridge_metadata](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/gen_bridge_metadata.1.html) and spending hours trying to alter the Ruby scripts to output information intuitively, it became apparent that I needed a transparent and extensibile solution to retreiving Objective-C metadata. While gen_bridge_metadata uses native C extensions for Ruby to interface with libclang and Ruby for consuming the AST and providing a front-end, clocxml uses C++ - providing direct feedback from Clang when parsing a file and removing an unnecessary layer of overhead. The source code for clocxml is arguably much more organized and terse compared to what I've seen from [gen_bridge_metadata](https://github.com/mobiruby/BridgeSupport) allowing for easy development and contributions.

Samples
---
Want to generate code which interfaces with NSDictionary methods? Need metadata? Here's how to do that with clocxml.

**Step 1: Determine the clang options necessary to compile NSDictionary.h**
Clang needs to know how to build the AST for the file we're parsing, so we need to specify this to clocxml.

Compiling NSDictionary.h in the following way works for me:
`clang -ObjC -resource-dir /usr/local/ -arch armv7 -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/4.2/include/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk -c tests/NSDictionary.h`

We'll need to pass these same compiler flags into clocxml to get the analagous metadata, so let's define a shell variable to make this easier.

`clang_flags="-ObjC -resource-dir /usr/local/ -arch armv7 -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/4.2/include/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk -c"`

**Step 2: Run clocxml**
Now all we need to do is call clocxml with the correct flags:

`-t: Tabulates the XML output`
`-c: Passes the corresponding argument to clang`

So to generate metadata:
`clocxml -t -c"$clang_flags" NSDictionary.h`
