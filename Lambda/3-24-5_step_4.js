
//If temperature in the greenhouse > highest_temperature
    //Send the alarm
//Else if temperature in the greenhouse < lowest_temperature
    //Send the alarm
//Else
    //If Specifictime > last_time && Specifictime < current_time
        //Send the alarm



'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
//Readfrom DB
AWS.config.update({region: 'us-east-1'});
var ddb = new AWS.DynamoDB({apiVersion: '2012-08-10'});
//Readfrom DB
exports.handler = (event, context, callback) => {
var Current_time;

    Read_Lowest_temperature(function(Lowest_temperature){
        Read_Highest_temperature(function(Highest_temperature){
            Read_Specific_time(function(Specific_time){
                Read_Interval(function(Interval){
                    Read_Last_time(function(Last_time){
                        Read_IOT_Data(event,function(IOT_data){
                            Current_time = IOT_data[1];
                            Write_Current_time(Current_time);
                            //Send_message(IOT_data,Lowest_temperature,Highest_temperature,Specific_time,Last_time);
                            console.log(Lowest_temperature);
                            console.log(Highest_temperature);
                            console.log(Specific_time);
                            console.log(Interval);
                            console.log(Last_time);
                            console.log(IOT_data);
                        });
                    });
                });
            });
        });
    });
};
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

function Read_Specific_time(callback) {
    var params_Specific_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported' : {S: 'Specific_time'},
            'index' : {S: '0001'},
        }
    };
    ddb.getItem(params_Specific_time, function(err, data,response) {
        if (err) {
            console.log("Error Reading Specific_time",err);
        } 
        else {
            console.log("Success Reading Specific_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Specific_time= JSON.parse(Jsondata).Item.value.N;
            return callback(Specific_time);
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

function Read_IOT_Data(event,callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    var DateYMD =  JSON.parse(datapackage).reported.Date;
    var Time =  JSON.parse(datapackage).reported.Time;
    var Temperature_inside =  JSON.parse(datapackage).reported.Temperature_inside;
    var Temperature_outside =  JSON.parse(datapackage).reported.Temperature_outside;
    var Humidmity =  JSON.parse(datapackage).reported.Humidity;
    var IOT_data=[DateYMD,Time,Temperature_inside,Temperature_outside,Humidmity];
    return callback(IOT_data);
}


function Write_Current_time(Current_time_input) {
    var params_Current_time = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported' : {S: 'Last_time'},
            'index' : {S: '0001'},
            'value': {N: Current_time_input}
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

function Send_message(IOT_data,Lowest_temperature,Highest_temperature,Specific_time,Last_time) {
    var DateYMD =  IOT_data[0];
    var Time =  IOT_data[1];
    var Temperature_inside =  IOT_data[2];
    var Temperature_outside =  IOT_data[3];
    var Humidmity =  IOT_data[4];
    var message1 = 'Current Time: ' + DateYMD + '-' + Time +'\n';
    var message2 = 'The Temperature inside the greenhouse is: ' + Temperature_inside + 'degrees.\n';
    var message3 = 'The Temperature outside the greenhouse is: ' + Temperature_outside + 'degrees.\n';
    var message4 = 'The Humidity inside the greenhouse is ' + Humidmity + '%' + '\n';
    var messageupdate = message1 + message2 + message3 + message4;
    
    var messagewarning_temperature_too_low = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    messagewarning_temperature_too_low += ' degrees, it is lower than the lowest desired temperature.\n';

    var messagewarning_temperature_too_high = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    messagewarning_temperature_too_high += ' degrees, it is higher than the highest desired temperature.\n';
    var messagesent;
    var sns;
    var params;
    if (Temperature_inside > Highest_temperature) {
        messagesent = messagewarning_temperature_too_high;
        sns = new AWS.SNS();
        params = {
            Message: messagesent,
            TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
        };
        sns.publish(params, context.done);
    }
    else if (Temperature_inside < Lowest_temperature) {
        messagesent = messagewarning_temperature_too_low;
        sns = new AWS.SNS();
        params = {
            Message: messagesent,
            TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
        };
        sns.publish(params, context.done);
    }
    else {
        if (Specific_time > Last_time &&  Specific_time < Time){
            messagesent = messageupdate;
            sns = new AWS.SNS();
            params = {
                Message: messagesent,
                TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
            };
            sns.publish(params, context.done);
        }
    }
}