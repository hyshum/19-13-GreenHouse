//Record the time by the specific month

//Include this information in update information


'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
//const request = require('request');
//Readfrom DB
AWS.config.update({ region: 'us-east-1' });
var ddb = new AWS.DynamoDB({ apiVersion: '2012-08-10' });
//Readfrom DB

exports.handler = (event, context, callback) => {

    Read_Lowest_temperature(function (Lowest_temperature) {
        Read_Highest_temperature(function (Highest_temperature) {
            Read_Specific_time(function (Specific_time) {
                Read_IOT_Data(event, function (IOT_data) {
                    Decide_month(IOT_data, function (Month) {
                        Read_Last_time(function (Last_time) {
                            Read_Heater_runningtime(Month, function (Heater_runningtime) {
                                Read_Heater_current(function (Heater_current) {
                                    Count_Heater(Month, Heater_current, parseInt(Last_time, 10), IOT_data, parseInt(Heater_runningtime, 10), function(Heater_runningtime_total) {
                                        Write_Item_dynamoDB(IOT_data);
                                        Write_Current_time(IOT_data[1]);
                                        Send_Update_Message(Heater_runningtime_total, Month, IOT_data, Highest_temperature, Lowest_temperature, context, Last_time, Specific_time);
                                        // Send_Warning_Message(IOT_data, Highest_temperature, Lowest_temperature, context);
                                        Switching_On_OFF(IOT_data, Lowest_temperature, Heater_current);
                                    })
                                })
                            });
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

function Read_IOT_Data(event, callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    var Date = 20190331;//JSON.parse(datapackage).reported.Date;
    var Time = 220010;//JSON.parse(datapackage).reported.Time;
    var Temperature_inside = 70;//JSON.parse(datapackage).reported.Temperature_inside;
    var Temperature_outside = 1000;//JSON.parse(datapackage).reported.Temperature_outside;
    var Humidmity = 1000;// JSON.parse(datapackage).reported.Humidity;
    var IOT_data = [Date, Time, Temperature_inside, Temperature_outside, Humidmity];
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
            'Humidmity': { N: IOT_data[4].toString() }
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

function Send_Update_Message(Heater_runningtime_total, Month, IOT_data, Highest_temperature, Lowest_temperature, context, Last_time, Specific_time) {
    var DateYMD = IOT_data[0];
    var Time = IOT_data[1];
    var Temperature_inside = IOT_data[2];
    var Temperature_outside = IOT_data[3];
    var Humidmity = IOT_data[4];
    var message1 = 'Current Time: ' + DateYMD + '-' + Time + '\n';
    var message2 = 'The Temperature inside the greenhouse is: ' + Temperature_inside + 'degrees.\n';
    var message3 = 'The Temperature outside the greenhouse is: ' + Temperature_outside + 'degrees.\n';
    var message4 = 'The Humidity inside the greenhouse is ' + Humidmity + '%' + '\n';
    var message5 = 'Highest Temperature is ' + Highest_temperature + '\n';
    var message6 = 'Lowest Temperature is ' + Lowest_temperature + '\n';
    var message7 = 'Total power consumpation in ' + Month + ' is ' + Heater_runningtime_total;
    var messageupdate = message1 + message2 + message3 + message4 + message5 + message6  + message7;

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
    // request(option, function (error, response, body) {
    //     console.log('error:', error); // Print the error if one occurred
    //     console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
    //     console.log('body:', body); // Print the HTML for the Google homepage.
    // });
}

function Switch_OFF() {
    console.log('Running Switch');
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    // request(option, function (error, response, body) {
    //     console.log('error:', error); // Print the error if one occurred
    //     console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
    //     console.log('body:', body); // Print the HTML for the Google homepage.
    // });
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



function Count_Heater(Month, Heater_current, Last_time, IOT_data, Heater_runningtime,callback) {
    var Time = parseInt(IOT_data[1], 10);
    var Heater_runningtime_total;
    if (Heater_current == 1) {
        console.log(Last_time);
        console.log(Time);
        console.log(Heater_runningtime);
        Heater_runningtime_total = Heater_runningtime + (Time - Last_time);
        console.log(Heater_runningtime_total);
        Update_Heater_runningtime(Month, Heater_runningtime_total);
        return callback(Heater_runningtime_total);
    }
}

function Update_Heater_runningtime(Month, Heater_runningtime) {
    var params_Heater_runningtime = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Power_consumption_' + Month },
            'index': { S: '0001' },
            'value': { N: Heater_runningtime.toString() }
        }
    };
    ddb.putItem(params_Heater_runningtime, function (err, data) {
        if (err) {
            console.log("Error Writing Heater_runningtime", err);
        } else {
            console.log("Success Writing Heater_runningtime");
        }
    });
}



function Read_Heater_runningtime(Month, callback) {
    var params_Heater_runningtime = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Power_consumption_' + Month },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Heater_runningtime, function (err, data, response) {
        if (err) {
            console.log("Error Reading Heater_runningtime", err);
        }
        else {
            console.log("Success Reading Heater_runningtime");
            var Jsondata = JSON.stringify(data, null, 2);
            var Heater_runningtime = JSON.parse(Jsondata).Item.value.N;
            return callback(Heater_runningtime);
        }
    });
}

function Decide_month(IOT_data, function (Month) {
    var DateYMD = IOT_data[0];
    var DateYMD_chars = DateYMD.split('');
    var Month_number = DateYMD_chars[4] + DateYMD_chars[5];
    var Month;
    switch (Month_number) {
        case '01':
            Month = 'Jan';
            break;
        case '02':
            Month = 'Feb';
            break;
        case '03':
            Month = 'Mar';
            break;
        case '04':
            Month = 'Apr';
            break;
        case '05':
            Month = 'May';
            break;
        case '06':
            Month = 'Jun';
            break;
        case '07':
            Month = 'Jul';
            break;
        case '08':
            Month = 'Aug';
            break;
        default:
            console.log('Wrong Month Number');
    }
    return callback(Month);

}

function Read_Power_consumption_month(Month)


function Read_IOT_Data(event, callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    var Date = 20190403;//JSON.parse(datapackage).reported.Date;
    var Time = 120010;//JSON.parse(datapackage).reported.Time;
    var Temperature_inside = 70;//JSON.parse(datapackage).reported.Temperature_inside;
    var Temperature_outside = 1000;//JSON.parse(datapackage).reported.Temperature_outside;
    var Humidmity = 1000;// JSON.parse(datapackage).reported.Humidity;
    var IOT_data = [Date, Time, Temperature_inside, Temperature_outside, Humidmity];
    return callback(IOT_data);
}