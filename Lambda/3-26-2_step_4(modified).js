//If temperature in the greenhouse > highest_temperature
    //Send the alarm
//Else if temperature in the greenhouse < lowest_temperature
    //Send the alarm
//Else

//If Specifictime(input on web, time that the client wants to receive the update)> last_time && Specifictime < current_time
    //If Date = Week_number(input on web, the day in a week that the client wants to receive the update)
        //Send the regular update

//Originally, sensor data is sent to database using IOT
//This version sent data to database via lambda

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
            Read_Interval(function(Interval){
                Read_Last_time(function(Last_time){
                    Read_IOT_Data(event,function(IOT_data){
                        Write_Item_dynamoDB(IOT_data);
                        Write_Current_time(IOT_data[1]);
                        console.log(Lowest_temperature);
                        console.log(Highest_temperature);
                        console.log(Interval);
                        console.log(Last_time);
                        console.log(IOT_data);
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
                'index' : {S: '0002'},
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
            'index' : {S: '0002'},
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

// function Read_Current_time(callback) {
//     var params_Current_time = {
//         TableName: 'GreenhouseDatabase',
//         Key: {
//             'reported' : {S: 'Current_time'},
//             'index' : {S: '0001'},
//         }
//     };
//     ddb.getItem(params_Current_time, function(err, data,response) {
//         if (err) {
//             console.log("Error Reading Current_time",err);
//         } 
//         else {
//             console.log("Success Reading Current_time");
//             var Jsondata = JSON.stringify(data, null, 2);
//             var Current_time= JSON.parse(Jsondata).Item.value.N;
//             return callback(Current_time);
//         }
//     });
// }


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

function Read_IOT_Data(event,callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    var Date =  1000;//JSON.parse(datapackage).reported.Date;
    var Time =  1000;//JSON.parse(datapackage).reported.Time;
    var Temperature_inside =  1000;//JSON.parse(datapackage).reported.Temperature_inside;
    var Temperature_outside =  1000;//JSON.parse(datapackage).reported.Temperature_outside;
    var Humidmity = 1000;// JSON.parse(datapackage).reported.Humidity;
    var IOT_data=[Date,Time,Temperature_inside,Temperature_outside,Humidmity];
    return callback(IOT_data);
}

function Write_Item_dynamoDB(IOT_data) {
    var params_IOT_data = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported' : {S: 'Sensor_Data'},
            'index' : {S: IOT_data[0].toString() + IOT_data[1].toString()},
            'Temperature_inside': {N: IOT_data[2].toString()},
            'Temperature_outside': {N: IOT_data[3].toString()},
            'Humidmity': {N: IOT_data[4].toString()}
        }
    };
        ddb.putItem(params_IOT_data, function(err, data) {
        if (err) {
            console.log("Error Creating Item", err);
        } else {
            console.log("Success Creating Item");
        }
    });
}

function Write_Current_time(Current_time_input) {
    var params_Current_time = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported' : {S: 'Last_time'},
            'index' : {S: '0001'},
            'value': {N: Current_time_input.toString()}
        }
    };
        ddb.putItem(params_Current_time, function(err, data) {
        if (err) {
            console.log("Error Writing Current_time", err);
        } else {
            console.log("Success Writing Current_time");
        }
    });
}