#!/usr/bin/env node

const spawn = require('child_process').spawn;
const ec = spawn('./echoclient', ['localhost', '1235']);

const cluster = require('cluster');
const numCPUs = require('os').cpus().length;
var deadFork = 0;

if (cluster.isMaster) {
    for (let i = 0; i < numCPUs; i++) {
        cluster.fork();
    }

    cluster.on('exit', (worker, code, signal) => {
        console.log(`woker ${worker.process.pid} died`);
        if (++deadFork >= numCPUs) {
            process.exit(0);
        }
    });
} else {
    console.log(`[${cluster.worker.id}]woker (${process.pid}) ready~!`);

    for (let i = 10; i < 200; i ++) {
        startWriting(ec, `localhost 1234 ${cluster.worker.id}-${i}\n`);
    }

    process.exit(0);
}

function startWriting(client, string) {
    process.stdout.write(` ${string}`);
    client.stdin.write(string);
}

cluster.on('online', (worker) => {
    if (worker.id >= 4) {

    }
});

ec.stderr.on('data', (data) => {
    console.log(`stderr: ${data}`);
});

ec.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
});
