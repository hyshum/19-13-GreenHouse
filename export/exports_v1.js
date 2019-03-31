//Turn on the heater if the temperature is too low


'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
//const request = require('request');
//Readfrom DB
AWS.config.update({ region: 'us-east-1' });
var ddb = new AWS.DynamoDB({ apiVersion: '2012-08-10' });
//Readfrom DB

exports.handler = (event, context, callback) => {
    var today = new Date();
    var Year = today.getFullYear().toString();
    var Month = today.getMonth();
    var Day = today.getDate();
    var Hour = today.getHours() - 4;
    var Minute = today.getMinutes();
    var Second = today.getSeconds();
    if (Month.toString().length == 1) {
        Month = '0' + (Month + 1).toString();
    }
    if (Day.toString().length == 1) {
        Day = '0' + (Day).toString();
    }
    if (Hour.toString().length == 1) {
        Hour = '0' + (Hour).toString();
    }
    if (Minute.toString().length == 1) {
        Minute = '0' + (Minute).toString();
    }
    if (Second.toString().length == 1) {
        Second = '0' + (Second).toString();
    }
    var YMD = Year.toString()+Month.toString()+Day.toString();
    var HMS = Hour.toString()+Minute.toString()+Second.toString();
    Read_Lowest_temperature(function (Lowest_temperature) {
        Read_Highest_temperature(function (Highest_temperature) {
            Read_Specific_time(function (Specific_time) {
                Read_Last_time(function (Last_time) {
                    Read_Heater_current(function (Heater_current) {
                        Read_IOT_Data(event, YMD, HMS, function (IOT_data) {
                            console.log(IOT_data);
                            Write_Item_dynamoDB(IOT_data);
                            Write_Current_time(IOT_data[1]);
                            Send_Update_Message(IOT_data, Highest_temperature, Lowest_temperature, context, Last_time, Specific_time);
                            // Send_Warning_Message(IOT_data, Highest_temperature, Lowest_temperature, context);
                            Switching_On_OFF(IOT_data, Lowest_temperature, Heater_current);
                        });
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
            'reported': { S: 'Lowest_temperature' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Lowest_temperature, function (err, data, response) {
        if (err) {
            console.log("Error Reading Lowest_temperature", err);
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
            'reported': { S: 'Highest_temperature' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Highest_temperature, function (err, data, response) {
        if (err) {
            console.log("Error Reading Highest_temperature", err);
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
    var params_Interval = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Specific_time' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Interval, function (err, data, response) {
        if (err) {
            console.log("Error Reading Specific_time", err);
        }
        else {
            console.log("Success Reading Specific_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Interval = JSON.parse(Jsondata).Item.value.N;
            return callback(Interval);
        }
    });
}

function Read_Last_time(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Last_time' },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Last_time, function (err, data, response) {
        if (err) {
            console.log("Error Reading Last_time", err);
        }
        else {
            console.log("Success Reading Last_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Last_time = JSON.parse(Jsondata).Item.value.N;
            return callback(Last_time);
        }
    });
}

function Read_Heater_current(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Heater_current' },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Last_time, function (err, data, response) {
        if (err) {
            console.log("Error Reading Heater_current", err);
        }
        else {
            console.log("Success Reading Heater_current");
            var Jsondata = JSON.stringify(data, null, 2);
            var Heater_current = JSON.parse(Jsondata).Item.value.N;
            return callback(Heater_current);
        }
    });
}

function Read_IOT_Data(event, YMD, HMS, callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    console.log(datapackage);
    var Temperature_inside = JSON.parse(datapackage).reported.ti;
    var Temperature_outside = JSON.parse(datapackage).reported.to;
    var Humidmity = JSON.parse(datapackage).reported.h;
    var Light_level = JSON.parse(datapackage).reported.l;
    var Soil_Moisture_1 = JSON.parse(datapackage).reported.a;
    var Soil_Moisture_2 = JSON.parse(datapackage).reported.b;
    var Soil_Moisture_3 = JSON.parse(datapackage).reported.c;
    var IOT_data = [YMD, HMS, Temperature_inside, Temperature_outside, Humidmity, Light_level, Soil_Moisture_1, Soil_Moisture_2, Soil_Moisture_3];
    return callback(IOT_data);
}



function Write_Item_dynamoDB(IOT_data) {
    var params_IOT_data = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Sensor_Data' },
            'index': { S: IOT_data[0].toString() + IOT_data[1].toString() },
            'Date': { N: IOT_data[0].toString() },
            'Time': { N: IOT_data[1].toString() },
            'Temperature_inside': { N: IOT_data[2].toString() },
            'Temperature_outside': { N: IOT_data[3].toString() },
            'Humidmity': { N: IOT_data[4].toString() },
            'Light_level': { N: IOT_data[5].toString() },
            'Soil_Moisture_1': { N: IOT_data[6].toString() },
            'Soil_Moisture_2': { N: IOT_data[7].toString() },
            'Soil_Moisture_3': { N: IOT_data[8].toString() }
        }
    };
    ddb.putItem(params_IOT_data, function (err, data) {
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
            'reported': { S: 'Last_time' },
            'index': { S: '0001' },
            'value': { N: Current_time_input.toString() }
        }
    };
    ddb.putItem(params_Current_time, function (err, data) {
        if (err) {
            console.log("Error Writing Current_time", err);
        } else {
            console.log("Success Writing Current_time");
        }
    });
}

function Write_Heater_current(Heater_current) {
    var params_Heater_current = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Heater_current' },
            'index': { S: '0001' },
            'value': { N: Heater_current.toString() }
        }
    };
    ddb.putItem(params_Heater_current, function (err, data) {
        if (err) {
            console.log("Error Writing Heater_current", err);
        } else {
            console.log("Success Writing Heater_current");
        }
    });
}

function Send_Update_Message(IOT_data, Highest_temperature, Lowest_temperature, context, Last_time, Specific_time) {
    var DateYMD = IOT_data[0];
    var Time = IOT_data[1];
    var Temperature_inside = IOT_data[2];
    var Temperature_outside = IOT_data[3];
    var Humidmity = IOT_data[4];
    var Light_level = IOT_data[5];
    var Soil_Moisture_1 = IOT_data[6];
    var Soil_Moisture_2 = IOT_data[7];
    var Soil_Moisture_3 = IOT_data[8];
    var message1 = 'Current Time: ' + DateYMD + '-' + Time + '\n';
    var message2 = 'The Temperature inside the greenhouse is: ' + Temperature_inside + ' degrees.\n';
    var message3 = 'The Temperature outside the greenhouse is: ' + Temperature_outside + ' degrees.\n';
    var message4 = 'The Humidity inside the greenhouse is ' + Humidmity + ' %' + '\n';
    var message5 = 'The Light level inside the greenhouse is ' + Light_level + '\n';
    var message6 = 'Soil moisture 1 is ' + Soil_Moisture_1 + '\n';
    var message7 = 'Soil moisture 2 is ' + Soil_Moisture_2 + '\n';
    var message8 = 'Soil moisture 3 is ' + Soil_Moisture_3 + '\n';
    var messageupdate = message1 + message2 + message3 + message4 + message5 + message6 + message7 + message8;

    var messagewarning_temperature_too_low = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;

    var messagewarning_temperature_too_high = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;


    var messagesent;
    if (Temperature_inside < Lowest_temperature) {
        messagesent = messagewarning_temperature_too_low;
    }
    else if (Temperature_inside > Highest_temperature) {
        messagesent = messagewarning_temperature_too_high;
    }
    else {
        messagesent = messageupdate;
    }

    var sns = new AWS.SNS();
    var params = {
        Message: messagesent,
        TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
    };

    if (Specific_time > Last_time && Specific_time < Time) {
        console.log(messagesent);
        sns.publish(params, context.done);
    }

    //SendMessage-END
}

function Send_Warning_Message(IOT_data, Highest_temperature, Lowest_temperature, context) {
    var Temperature_inside = IOT_data[2];

    var index = 0;
    var messagewarning_temperature_too_low = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    messagewarning_temperature_too_low += ' degrees, it is lower than the lowest desired temperature.\n';

    var messagewarning_temperature_too_high = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    messagewarning_temperature_too_high += ' degrees, it is higher than the highest desired temperature.\n';

    var messagesent;
    if (Temperature_inside < Lowest_temperature) {
        messagesent = messagewarning_temperature_too_low;
        index = 1;
    }
    else if (Temperature_inside > Highest_temperature) {
        messagesent = messagewarning_temperature_too_high;
        index = 1;
    }
    else {
        index = 0;
    }


    var sns = new AWS.SNS();
    var params = {
        Message: messagesent,
        TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
    };
    if (index == 1) {
        sns.publish(params, context.done);
    }
}

function Switch_On() {
    console.log('Running Switch');
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseON/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    request(option, function (error, response, body) {
        console.log('error:', error); // Print the error if one occurred
        console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
        console.log('body:', body); // Print the HTML for the Google homepage.
    });
}

function Switch_OFF() {
    console.log('Running Switch');
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    request(option, function (error, response, body) {
        console.log('error:', error); // Print the error if one occurred
        console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
        console.log('body:', body); // Print the HTML for the Google homepage.
    });
}

function Switching_On_OFF(IOT_data, Lowest_temperature, Heater_current) {
    var Temperature_inside = IOT_data[2];
    var Heater_next;
    if (Temperature_inside < Lowest_temperature && Heater_current == 0) {
        //Switch_On();
        console.log('Switch Heater On');
        Heater_next = 1;
        Write_Heater_current(Heater_next);
    }
    else if (Temperature_inside > Lowest_temperature && Heater_current == 1) {
        //Switch_OFF();
        console.log('Switch Heater Off');
        Heater_next = 0;
        Write_Heater_current(Heater_next);
    }
    else {
    }
}

