const fs = require('fs');
const crypto = require('crypto');

const filePath = './nrf52810_xxaa.bin';
const privateKeyFile = '../../private.key';
const algo = 'sha256';
const curve = 'p256';


const initPacket_header = new Buffer.from([0x12 ,0x8a ,0x01 ,0x0a ,0x44 ,0x08 ,0x01 ,0x12 ,0x40]);
let initPacket = new Buffer.from([0x08 ,0x01 ,0x10 ,0x34 ,0x1a ,0x02 ,0x83 ,0x02 ,0x20 ,0x00 ,0x28 ,0x00 ,0x30 ,0x00 ,0x38 ,0x84 ,0x8c ,0x03 ,0x42 ,0x24 ,0x08 ,0x03 ,0x12 ,0x20]);

if (process.argv.length < 2) {
    console.log("base64 adv key expected");
    exit();
}

const file = fs.readFileSync(filePath);
const str = file.toString("hex");
let keyBuffer = new Buffer.from(process.argv[2], 'base64');
let offlineBuffer = new Buffer.from("OFFLINEFINDINGPUBLICKEYHERE!", 'ascii');
let newStr = str.replace(offlineBuffer.toString("hex"), keyBuffer.toString("hex"));
let buffer = Buffer.from( newStr, "hex" );
fs.writeFileSync(filePath + ".patched", buffer);

fs.readFile(filePath +".patched", (err, data) => {
    if (err) {
        console.error(err);
        return;
    }
    const hash = crypto.createHash('sha256');
    hash.update(data);
	const hashBuffer = hash.digest();

    // Añadimos SHA256 en little endian
    initPacket = Buffer.concat([initPacket, hashBuffer.reverse()]);

    // Añadimos data fijo
    initPacket = Buffer.concat([initPacket, new Buffer.from([0x48, 0x00, 0x52, 0x04, 0x08, 0x01, 0x12, 0x00])]);

    // Firmamos lo que tenemos con ecdsa
    fs.readFile('../../private.key', 'utf8', (err, sk_pem) => {
        /*
        if (err) throw err;
        const EC = require('elliptic').ec;
        const ec = new EC('p256');

        console.log("initPacket to be signed: ", initPacket);

        const sk = ec.keyFromPrivate(sk_pem, 'hex');
        const hash = crypto.createHash('sha256');
        hash.update(initPacket);
        const sha256packet = hash.digest();
        console.log("initPacket", initPacket.length, initPacket);
        console.log("sha256packet", sha256packet);
        const sig = sk.sign(sha256packet);
        const sigBuffer1 = Buffer.from(sig.toDER().slice(0,32)).reverse();
        const sigBuffer2 = Buffer.from(sig.toDER().slice(32,64)).reverse();
        console.log("sig", sig.toDER());
        console.log("sigBuffer1", sigBuffer1);
        console.log("sigBuffer2", sigBuffer2);
        console.log("sigBufferR", Buffer.from(sig.r.toArray()));
        console.log("sigBufferS", Buffer.from(sig.s.toArray()));
        initPacket = Buffer.concat([initPacket, Buffer.from([0x10, 0x00, 0x1a, 0x40]), Buffer.from(sig.r.toArray()).reverse(), Buffer.from(sig.s.toArray()).reverse()]);
        //initPacket = Buffer.concat([initPacket, sigBuffer1, sigBuffer2]);
        */

        //fs.writeFileSync("initPacket.bin", initPacket);
        const sign = crypto.createSign('SHA256');
        sign.update(initPacket);
        sign.end();


        const signature = sign.sign({ key: sk_pem, dsaEncoding: 'ieee-p1363', curve: 'p256' });
        console.log(signature);
        //fs.writeFileSync("sign.bin", signature);
        let r = signature.subarray(0,32);
        let s = signature.subarray(32,64);
        console.log(r,s);
        initPacket = Buffer.concat([initPacket, Buffer.from([0x10, 0x00, 0x1a, 0x40]), Buffer.from(r).reverse(), Buffer.from(s).reverse()]);

        /*
        let r = signature.subarray(0, 32);
        let s = signature.subarray(32, 64);
        console.log("sigBufferR", r.reverse());
        console.log("sigBufferR", s.reverse());
        */

        /*
        const elliptic = require("elliptic").ec;
        const ec = new elliptic("p256");
        const key = ec.keyFromPrivate(sk_pem);
        const signature = key.sign(initPacket);
        let r = new Buffer.from(signature.r.toArray());
        let s = new Buffer.from(signature.s.toArray());
        console.log(r, s);

        initPacket = Buffer.concat([initPacket, r.reverse(), s.reverse()]);

        */


        // Finalizamos packet
        initPacket = Buffer.concat([initPacket_header, initPacket]);

        fs.writeFile('my.dat', initPacket, (err) => {
            if (err) throw err;
            console.log('The buffer was written to the file!');
        });
    });

    //const hashArray = Array.from(hashBuffer);
    //const littleEndian = hashArray.reverse().map((val) => ('0' + val.toString(16)).slice(-2)).join('');
    //console.log(`The SHA-256 hash of ${filePath} in little-endian format is: ${littleEndian}`);

});
