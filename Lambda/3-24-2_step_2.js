// Write the current time to DynamoDB, replace the current_time

'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
var highest;
var Lowest;
//Readfrom DB
AWS.config.update({region: 'us-east-1'});
var ddb = new AWS.DynamoDB({apiVersion: '2012-08-10'});
//Readfrom DB

exports.handler = (event, context, callback) => {


    Read_Lowest_temperature(function(Lowest_temperature){
        Read_Highest_temperature(function(Highest_temperature){
            Read_Current_time(function(Current_time){
                Read_Interval(function(Interval){
                    Read_Last_time(function(Last_time){
                        console.log(Lowest_temperature);
                        console.log(Highest_temperature);
                        console.log(Current_time);
                        console.log(Interval);
                        console.log(Last_time);
                    });
                });
            });
        });
    });



}

function Read_Lowest_temperature(callback) {
        var params_Lowest_temperature = {
            TableName: 'GreenhouseDatabase',
            Key: {
                'reported' : {S: 'Lowest_temperature'},
                'index' : {S: '0001'},
            }
        };
        ddb.getItem(params_Lowest_temperature, function(err, data,response) {
            if (err) {
                console.log("Error Reading Lowest_temperature",err);
            } 
            else {
                console.log("Success Reading Lowest_temperature");
                var Jsondata = JSON.stringify(data, null, 2);
                var Lowest_temperature = JSON.parse(Jsondata).Item.value.N;
                return callback(Lowest_temperature);
            }
        });
}

function Read_Highest_temperature(callback) {
    var params_Highest_temperature = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported' : {S: 'Highest_temperature'},
            'index' : {S: '0001'},
        }
    };
    ddb.getItem(params_Highest_temperature, function(err, data,response) {
        if (err) {
            console.log("Error Reading Highest_temperature",err);
        } 
        else {
            console.log("Success Reading Highest_temperature");
            var Jsondata = JSON.stringify(data, null, 2);
            var Highest_temperature = JSON.parse(Jsondata).Item.value.N;
            return callback(Highest_temperature);
        }
    });
}

function Read_Current_time(callback) {
    var params_Current_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported' : {S: 'Current_time'},
            'index' : {S: '0001'},
        }
    };
    ddb.getItem(params_Current_time, function(err, data,response) {
        if (err) {
            console.log("Error Reading Current_time",err);
        } 
        else {
            console.log("Success Reading Current_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Current_time= JSON.parse(Jsondata).Item.value.N;
            return callback(Current_time);
        }
    });
}


function Read_Interval(callback) {
    var params_Interval = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported' : {S: 'Interval'},
            'index' : {S: '0001'},
        }
    };
    ddb.getItem(params_Interval, function(err, data,response) {
        if (err) {
            console.log("Error Reading Interval",err);
        } 
        else {
            console.log("Success Reading Interval");
            var Jsondata = JSON.stringify(data, null, 2);
            var Interval= JSON.parse(Jsondata).Item.value.N;
            return callback(Interval);
        }
    });
}

function Read_Last_time(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported' : {S: 'Last_time'},
            'index' : {S: '0001'},
        }
    };
    ddb.getItem(params_Last_time, function(err, data,response) {
        if (err) {
            console.log("Error Reading Last_time",err);
        } 
        else {
            console.log("Success Reading Last_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Last_time= JSON.parse(Jsondata).Item.value.N;
            return callback(Last_time);
        }
    });
}