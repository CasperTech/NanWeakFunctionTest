const test = require('node-cmake')('NanWeakFunctionTest');

const obj = new test.TestObject();

function runTest()
{
    let testFunc = function(num)
    {
        return num+1;
    }
    for (let x = 0; x < 10000; x++)
    {
        const cbID = 'callback' + x;
        obj.createCallback(cbID, testFunc);
    }
    testFunc = undefined;
    global.gc();
}

runTest();
setInterval(() => {
    runTest();
}, 5000);

