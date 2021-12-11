'use strict';

const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const config = require('./config');
const router = express.Router();
const Model = require('./model/model');

const app = express();

app.use(express.json());
app.use(cors());
app.use(bodyParser.json());

// app.listen(config.port, () => console.log('App is listening on url http://localhost:' + config.port));


var SerialPort = require('serialport');

// Firestore 등록
const admin = require('firebase-admin');

var serviceAccount = require("./path/smart-sensor-project-firebase-adminsdk-cbw64-41abdda153.json");
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount)
});
const db = admin.firestore();


const parsers = SerialPort.parsers;
const parser = new parsers.Readline(
        {
            delimiters: '\r\n'
        }
);

var port = new SerialPort('COM10',{
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false
});

port.pipe(parser);

parser.on('data', function(data){
    console.log(data);
    const promise = new Promise((resolve, reject)=>{
        if(data.startsWith('2')){
            resolve('센서 인식');
        }else{
            reject('센서 오류');
        }
    });

    promise
        .then((message) => {
            try {
                if(data.charAt(10) == 'O'){
                    const table = db.collection('Table_Use_Information').doc(data.substring(0, data.length-3));
                    const seatIn = {
                        useInfo: true,
                        toilet: false
                    };
                    table.update(seatIn);
                    console.log("Use Success");
                }
                else if(data.charAt(10) == 'X'){
                    const table = db.collection('Table_Use_Information').doc(data.substring(0, data.length-3));
                    const seatOut = {
                        useInfo: false,
                        toilet: false
                    };
                    if(data.charAt(11) == 'O'){
                        seatOut.toilet = true;
                    }
                    table.update(seatOut);
                    console.log("Unuse Success"); 
                }
                if(data.substring(10, data.length-1) == "20" || data.substring(10, data.length-1) == "30"){
                    const table = db.collection('Table_Use_Information').doc(data.substring(0, data.length-3));
                    const absentData = {
                        useInfo: false,
                        toilet: true,
                        absent: true
                    };
                    table.update(absentData);
                    console.log("Absent");
                }
                       
            } catch (error) {
                console.log("Fail");
            }
        })
        .catch((error) => {
            console.error(error);
        })
        .finally(() => {
            //console.log('promise 작동중');
        })
});

const getAllUseInfo = async (req, res, next) => {
    try {
        const info = await db.collection('Table_Use_Information');
        const data = await info.get();
        const infoArray = [];
        if(data.empty) {
            res.status(404).send('No Table record found');
        }else {
            data.forEach(doc => {
                const model = new Model(
                    doc.id,
                    doc.data().useInfo,
                    doc.data().toilet,
                    doc.data().absent
                );
                infoArray.push(model);
            });
            res.send(infoArray);
        }
    } catch (error) {
        res.status(400).send(error.message);
    }
}
router.get('/Table_Use_Information', getAllUseInfo);

app.use('/info', router);
app.listen(config.port, () => console.log('App is listening on url http://localhost:' + config.port));