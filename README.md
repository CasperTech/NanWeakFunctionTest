# NanWeakFunctionTest
Demonstrate a possible flaw in Node's ref management

```
npm install
npm install -g node-cmake
ncmake configure
ncmake build
node --expose-gc test.js
```
